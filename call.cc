#define _GNU_SOURCE
#include <link.h>
#include <dlfcn.h>

#include <cxxabi.h>
#include <cstdint>
#include <cerrno>
#include <unistd.h> /* for sysconf */

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

template<class T, int N>
static void run_tests(T *t, CallSite<T> (&methods)[N])
{
    for (CallSite<T> & c : methods) {
        (t->*c)();
    }
}

#include "methods.Model_device.xi"
#include "methods.Model_core.xi"

template<class T>
struct List
{
    static T array[];
};

#define CAT_(a,b) a##b
#define CAT(a,b) CAT_(a,b)

#define Record(Type,Method) &Type::Method,
#define METHODS(Type) CAT(CAT(METHODS_,Type),_)
#define DESCRIPTORS(T) template<> CallSite<T> List< CallSite<T> >::array[] = { METHODS(T)(Record) };

#define TYPE_LIST(_) \
    _(Model_device) \
    _(Model_core) \
    // end TYPE_LIST

TYPE_LIST(DESCRIPTORS)

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

    run_tests(rec, List< CallSite<Model_device> >::array);
    run_tests(core, List< CallSite<Model_core> >::array);

    if (mprotect(info.dli_fbase, len, PROT_READ | PROT_EXEC) != 0)
        perror("mprotect");

}

