#define _POSIX_C_SOURCE

#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cxxabi.h>
#include <dlfcn.h>
#include <link.h>
#include <setjmp.h>
#include <signal.h>   /* for sigaction */
#include <sys/mman.h> /* for mprotect */
#include <ucontext.h>

#include "dynamic.hh"
#include "interface.hh"

template <class T>
struct CallSite
{
    // Define a type that calls, but with no arguments, and expects no response
    typedef void (T::*Type)(void);
    Type holder;
    intptr_t pc;

    template <typename _>
    CallSite<T>(_ T::*in)
        : holder(reinterpret_cast<Type>(in))
        , pc(0)
    {
        // no body
    }

    operator Type() { return holder; }
};

template <class T>
struct List
{
    static T array[];
    static T *start;
    static T const *end;
};

#define countof(X) (sizeof(X) / sizeof(X)[0])

static volatile intptr_t *current_pc = nullptr;

#define CAT_(a, b) a##b
#define CAT(a, b) CAT_(a, b)

#define Record(Type, Method) &Type::Method,
#define METHODS(Type) CAT(CAT(METHODS_, Type), _)
#define DESCRIPTORS(T)                                                         \
    template <>                                                                \
    CallSite<T> List<CallSite<T>>::array[] = {METHODS(T)(Record)};             \
    template <>                                                                \
    CallSite<T> *List<CallSite<T>>::start = &List<CallSite<T>>::array[0];      \
    template <>                                                                \
    CallSite<T> const *List<CallSite<T>>::end =                                \
        &List<CallSite<T>>::array[countof(List<CallSite<T>>::array)];          \
    // end DESCRIPTORS

#include "types.xi"

TYPE_LIST(DESCRIPTORS)

static jmp_buf bounce;

static void segv_handler(int /*signo*/, siginfo_t * /*info*/, void *context)
{
    ucontext_t *c = static_cast<ucontext_t *>(context);
    *current_pc = c->uc_mcontext.gregs[REG_RIP];
    siglongjmp(bounce, 1);
}

struct Fundamental
{
    static Library *lib;
    static Library::Calls<model_ctor_func *> model_ctor_p;
    static Library::Calls<model_dtor_func *> model_dtor_p;

    static ptrdiff_t get_text_len() { return lib->get_text_len(); }
    static void *get_load_base() { return lib->get_load_base(); }
};

template <typename T>
struct BaseBehavior : public Fundamental
{
    static bool load(int &argc, char **&argv)
    {
        if (argc > 0) {
            argc--;
            lib = new Library(*argv++);

            if (lib->handle) {
                model_ctor_p = lib->get_function(model_ctor);
                model_dtor_p = lib->get_function(model_dtor);
            }

            return lib->handle != nullptr;
        } else {
            return false;
        }
    }

    static void unload()
    {
        delete lib;
        lib = nullptr;
    }

    static int dump_results()
    {
        std::size_t buflen = 256;
        // We need malloc for __cxa_demangle
        char *buf = static_cast<char *>(malloc(buflen));
        for (auto &c : List<CallSite<T>>::array) {
            void *pc = reinterpret_cast<void *>(c.pc);
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
        typedef List<CallSite<T>> ThisList;
        if (ThisList::start < ThisList::end) {
            current_pc = &ThisList::start->pc;
            (t->*(*ThisList::start++))();
        }
    }
};

template <typename T>
struct DerivedBehavior : public BaseBehavior<T>
{
    // no body
};

template <>
struct DerivedBehavior<Model> : public BaseBehavior<Model>
{
    static Model *create(int &argc, char **&argv)
    {
        return argc > 0 ? (argc--, model_ctor_p(*argv++)) : nullptr;
    }

    static void destroy(Model *victim) { model_dtor_p(victim); }
};

template <>
struct DerivedBehavior<Core> : public BaseBehavior<Core>
{
    static Core *create(int &argc, char **&argv)
    {
        Model *parent = DerivedBehavior<Model>::create(argc, argv);
        if (parent == nullptr)
            return nullptr;
        return parent->getCore(0);
    }

    static void destroy(Core *victim) { model_dtor_p(victim->getModel()); }
};

Library *Fundamental::lib;
Library::Calls<model_ctor_func *> Fundamental::model_ctor_p;
Library::Calls<model_dtor_func *> Fundamental::model_dtor_p;

template <typename T>
static int execute(int &argc, char **&argv)
{
    bool success = DerivedBehavior<T>::load(argc, argv);
    if (!success) {
        printf("dlopen: %s\n", dlerror());
        return __LINE__;
    }

    T *rec = DerivedBehavior<T>::create(argc, argv);
    if (rec == nullptr)
        return __LINE__;

    ptrdiff_t len = DerivedBehavior<T>::get_text_len();
    void *base = DerivedBehavior<T>::get_load_base();

    if (mprotect(base, len, PROT_READ) != 0)
        perror("mprotect");

    struct sigaction action = {};
    action.sa_sigaction = segv_handler;
    action.sa_flags = SA_SIGINFO;

    if (sigaction(SIGSEGV, &action, NULL) != 0)
        perror("sigaction");

    sigsetjmp(bounce, 1 /*anything nonzero*/);

    DerivedBehavior<T>::run_tests(rec);

    DerivedBehavior<T>::dump_results();

    if (mprotect(base, len, PROT_READ | PROT_EXEC) != 0)
        perror("mprotect");

    DerivedBehavior<T>::destroy(rec);

    DerivedBehavior<T>::unload();

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

#define Execute(Type)                                                          \
    if (rc == 0 && argv && *argv && strcmp(*argv, #Type) == 0)                 \
        rc = execute<Type>(--argc, ++argv);

    TYPE_LIST(Execute)

    if (rc)
        printf("Error on %s line %d\n", __FILE__, rc);

    return rc;
}
