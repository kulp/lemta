#define _POSIX_C_SOURCE
#include <link.h>
#include <dlfcn.h>

#include <cxxabi.h>
#include <cstdint>
#include <cerrno>
#include <signal.h> /* for sigaction */
#include <setjmp.h>
#include <ucontext.h>
#include <cstring>
#include <cstdlib>
#include <cstdio>

#include <sys/mman.h> /* for mprotect */

#include "interface.hh"

template<class T>
struct CallSite
{
    // Define a type that calls, but with no arguments, and expects no response
    typedef void (T::*Type)(void);
    Type holder;
    intptr_t pc;

    template<typename _>
    CallSite<T>(_ T::*in)
        : holder(reinterpret_cast<Type>(in))
        , pc(0)
    {
        // no body
    }

    operator Type()
    {
        return holder;
    }
};

template<class T>
struct List
{
    static T array[];
    static T *start;
    static T const *end;
};

#define countof(X) (sizeof(X) / sizeof (X)[0])

static volatile intptr_t *current_pc = nullptr;

#define CAT_(a,b) a##b
#define CAT(a,b) CAT_(a,b)

#define Record(Type,Method) &Type::Method,
#define METHODS(Type) CAT(CAT(METHODS_,Type),_)
#define DESCRIPTORS(T) \
    template<> CallSite<T> List< CallSite<T> >::array[] = { METHODS(T)(Record) }; \
    template<> CallSite<T> * List< CallSite<T> >::start = &List< CallSite<T> >::array[0]; \
    template<> CallSite<T> const * List< CallSite<T> >::end = &List< CallSite<T> >::array[countof(List< CallSite<T> >::array) ]; \
    // end DESCRIPTORS

#include "types.xi"

TYPE_LIST(DESCRIPTORS)

static jmp_buf bounce;

static void segv_handler(int /*signo*/, siginfo_t * /*info*/, void *context)
{
    ucontext_t *c = static_cast<ucontext_t*>(context);
    *current_pc = c->uc_mcontext.gregs[REG_RIP];
    siglongjmp(bounce, 1);
}

struct Fundamental
{
    typedef Model *model_ctor_t(const char *);
    typedef int model_dtor_t(Model *);

    static model_ctor_t *model_ctor;
    static model_dtor_t *model_dtor;
};

template<typename T>
struct BaseBehavior : public Fundamental
{
    static void * load(int &argc, char **&argv)
    {
        if (argc > 0) {
            argc--;
            void *handle = dlopen(*argv++, RTLD_GLOBAL | RTLD_NOW);
            if (handle != nullptr) {
                model_ctor = reinterpret_cast<model_ctor_t*>(dlsym(handle, "model_ctor"));
                model_dtor = reinterpret_cast<model_dtor_t*>(dlsym(handle, "model_dtor"));
            }
            return handle;
        } else {
            return nullptr;
        }
    }

    static int unload(void *handle)
    {
        return dlclose(handle);
    }

    static int dump_results()
    {
        std::size_t buflen = 256;
        char *buf = static_cast<char*>(malloc(buflen)); // need malloc for __cxa_demangle
        for (auto & c : List< CallSite<T> >::array) {
            void * pc = reinterpret_cast<void*>(c.pc);
            Dl_info info;
            if (dladdr(pc, &info) == 0)
                return __LINE__;

            int status = -4;
            abi::__cxa_demangle(info.dli_sname, buf, &buflen, &status);
            printf("status = %d: got %p = %s\n", status, pc, buf);
        }
        free(buf);

        return 0;
    }

    static void run_tests(T *t)
    {
        typedef List< CallSite<T> > ThisList;
        if (ThisList::start < ThisList::end) {
            current_pc = &ThisList::start->pc;
            (t->*(*ThisList::start++))();
        }
    }

};

template<typename T>
struct DerivedBehavior : public BaseBehavior<T>
{
    // no body
};

template<>
struct DerivedBehavior<Model> : public BaseBehavior<Model>
{
    static Model *create(int &argc, char **&argv)
    {
        return argc > 0 ? (argc--, model_ctor(*argv++)) : nullptr;
    }

    static int destroy(Model *victim)
    {
        return model_dtor(victim);
    }
};

template<>
struct DerivedBehavior<Core> : public BaseBehavior<Core>
{
    static Core *create(int &argc, char **&argv)
    {
        Model *parent = DerivedBehavior<Model>::create(argc, argv);
        if (parent == nullptr)
            return nullptr;
        return parent->getCore(0);
    }

    static int destroy(Core *victim)
    {
        return model_dtor(victim->getModel());
    }
};

Fundamental::model_ctor_t *Fundamental::model_ctor = nullptr;
Fundamental::model_dtor_t *Fundamental::model_dtor = nullptr;

template<typename T>
static int execute(int &argc, char **&argv)
{
    void *handle = DerivedBehavior<T>::load(argc, argv);
    if (!handle) {
        printf("dlopen: %s\n", dlerror());
        return __LINE__;
    }

    T *rec = DerivedBehavior<T>::create(argc, argv);
    if (rec == nullptr)
        return __LINE__;

    Dl_info info;

    if (dladdr(dlsym(RTLD_DEFAULT, "model_ctor"), &info) == 0)
        return __LINE__;

    void *end = dlsym(RTLD_NEXT, "_fini");
    ptrdiff_t len = static_cast<char*>(end) - static_cast<char*>(info.dli_fbase);

    if (mprotect(info.dli_fbase, len, PROT_READ) != 0)
        perror("mprotect");

    struct sigaction action = {};
    action.sa_sigaction = segv_handler;
    action.sa_flags = SA_SIGINFO;

    sigsetjmp(bounce, 1/*anything nonzero*/);

    if (sigaction(SIGSEGV, &action, NULL) != 0)
        perror("sigaction");

    DerivedBehavior<T>::run_tests(rec);

    DerivedBehavior<T>::dump_results();

    if (mprotect(info.dli_fbase, len, PROT_READ | PROT_EXEC) != 0)
        perror("mprotect");

    if (DerivedBehavior<T>::destroy(rec))
        return __LINE__;

    if (DerivedBehavior<T>::unload(handle))
        return __LINE__;

    return 0;
}

int main(int argc, char **argv)
{
    int rc = 0;

    // skip command argument
    argc--;
    argv++;

    if (argc < 3)
        rc = __LINE__;

#define Execute(Type) \
    if (rc == 0 && argv && *argv && strcmp(*argv, #Type) == 0) \
        rc = execute<Type>(--argc, ++argv);

    TYPE_LIST(Execute)

    if (rc)
        printf("Error on %s line %d\n", __FILE__, rc);

    return rc;
}

