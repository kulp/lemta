#include "model.hh"

template<class T>
struct CallSite
{
    // Define a type that calls, but with no arguments, and expects no response
    typedef void (T::*Type)(void);
    Type holder;

    template<typename _>
    CallSite<T>(_ T::*in)
    {
        holder = reinterpret_cast<Type>(in);
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

    run_tests(rec, List< CallSite<Model_device> >::array);
    run_tests(core, List< CallSite<Model_core> >::array);
}

