#define _GNU_SOURCE
#include <link.h>
#include <dlfcn.h>

#include <cxxabi.h>
#include <cstdint>
#include <cerrno>
#include <unistd.h> /* for sysconf */
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

#include "methods.Model_device.xi"
#include "methods.Model_core.xi"

template<class T>
struct List
{
    static T array[];
    static T *start;
    static T const *end;
};

#define countof(X) (sizeof(X) / sizeof (X)[0])

template<class T>
static void run_tests(T *t)
{
    typedef List< CallSite<T> > ThisList;
    if (ThisList::start < ThisList::end) {
        (t->*(*ThisList::start++))();
    }
}

#define CAT_(a,b) a##b
#define CAT(a,b) CAT_(a,b)

#define Record(Type,Method) &Type::Method,
#define METHODS(Type) CAT(CAT(METHODS_,Type),_)
#define DESCRIPTORS(T) \
    template<> CallSite<T> List< CallSite<T> >::array[] = { METHODS(T)(Record) }; \
    template<> CallSite<T> * List< CallSite<T> >::start = &List< CallSite<T> >::array[0]; \
    template<> CallSite<T> const * List< CallSite<T> >::end = &List< CallSite<T> >::array[countof(List< CallSite<T> >::array) ]; \
    // end DESCRIPTORS

#define TYPE_LIST(_) \
    _(Model_device) \
    _(Model_core) \
    // end TYPE_LIST

TYPE_LIST(DESCRIPTORS)

static jmp_buf bounce;
static volatile sig_atomic_t bounce_state;

static void segv_handler(int signo, siginfo_t *info, void *context)
{
    ucontext_t *c = static_cast<ucontext_t*>(context);
    uintptr_t pc = c->uc_mcontext.gregs[REG_RIP];
    longjmp(bounce, ++bounce_state);
}

int main()
{
    Model_device *rec = model_ctor("attiny1616");
    Model_core *core = rec->getCore(0);

    typedef void voidfunc(void);

    Dl_info info;

    if (dladdr(dlsym(RTLD_DEFAULT, "model_ctor"), &info) == 0)
        return __LINE__;

    printf("%s loaded at %p\n", info.dli_fname, info.dli_fbase);
    void *begin = dlsym(RTLD_NEXT, "_init");
    void *end   = dlsym(RTLD_NEXT, "_fini");
    ptrdiff_t len = (char*)end - (char*)info.dli_fbase;

    long pagesize = sysconf(_SC_PAGESIZE);

    if (mprotect(info.dli_fbase, len, PROT_READ) != 0)
        perror("mprotect");

    struct sigaction action = {};
    action.sa_sigaction = segv_handler;
    action.sa_flags = SA_SIGINFO;

    long val = setjmp(bounce);
    switch (val) {
        case 0: puts("inited setjmp"); break;
        default: puts("bounced"); break;
    }

    if (sigaction(SIGSEGV, &action, NULL) != 0)
        perror("sigaction");

    sigset_t sigs = {};
    sigaddset(&sigs, SIGSEGV);
    sigprocmask(SIG_UNBLOCK, &sigs, NULL);

    run_tests(rec);
    run_tests(core);

    if (mprotect(info.dli_fbase, len, PROT_READ | PROT_EXEC) != 0)
        perror("mprotect");

}

