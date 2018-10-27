#define _GNU_SOURCE
#include <link.h>
#include <dlfcn.h>

#include <cxxabi.h>
#include <cstdint>
#include <cerrno>
#include <signal.h> /* for sigaction */
#include <setjmp.h>
#include <ucontext.h>

#include <sys/mman.h> /* for mprotect */

#include "model.hh"

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
    longjmp(bounce, 1);
}

template<typename T>
struct BaseBehavior
{
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
struct DerivedBehavior<Model_device> : public BaseBehavior<Model_device>
{
    static Model_device *create(int argc, char *argv[])
    {
        return argc > 1 ? model_ctor(argv[1]) : nullptr;
    }

    static int destroy(Model_device *victim)
    {
        return model_dtor(victim);
    }
};

template<>
struct DerivedBehavior<Model_core> : public BaseBehavior<Model_core>
{
    static Model_core *create(int argc, char *argv[])
    {
        Model_device *parent = DerivedBehavior<Model_device>::create(argc, argv);
        if (parent == nullptr)
            return nullptr;
        return parent->getCore(0);
    }

    static int destroy(Model_core *victim)
    {
        return model_dtor(victim->getModel());
    }
};

template<>
struct DerivedBehavior<Avr8> : public DerivedBehavior<Model_core>
{
    static Avr8 *create(int argc, char *argv[])
    {
        return dynamic_cast<Avr8*>(DerivedBehavior<Model_core>::create(argc, argv));
    }
};

template<typename T>
static int execute(int argc, char *argv[])
{
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

    setjmp(bounce);

    if (sigaction(SIGSEGV, &action, NULL) != 0)
        perror("sigaction");

    sigset_t sigs = {};
    sigaddset(&sigs, SIGSEGV);
    sigprocmask(SIG_UNBLOCK, &sigs, NULL);

    DerivedBehavior<T>::run_tests(rec);

    DerivedBehavior<T>::dump_results();

    if (mprotect(info.dli_fbase, len, PROT_READ | PROT_EXEC) != 0)
        perror("mprotect");

    return DerivedBehavior<T>::destroy(rec);
}

int main(int argc, char *argv[])
{
    return execute<Avr8>(argc, argv);
}

