#include "model.hh"

template<class T>
struct Monologuist
{
    // Define a type that calls, but with no arguments, and expects no response
    typedef void (T::*Type)(void);
    Type holder;

    template<typename _>
    Monologuist<T>(_ T::*in)
    {
        holder = reinterpret_cast<Type>(in);
    }

    operator Type()
    {
        return holder;
    }
};

template<class T>
struct CallSite
{
    Monologuist<T> method;
};

template<class T, int N>
static void run_tests(T *t, CallSite<T> (&methods)[N])
{
    for (CallSite<T> & c : methods) {
        (t->*c.method)();
    }
}

#include "methods.Model_device.xi"
#include "methods.Model_core.xi"

template<class T>
struct CallSiteList
{
    static CallSite<T> methods[];
};

#define CAT_(a,b) a##b
#define CAT(a,b) CAT_(a,b)

#define Record(Type,Method) &Type::Method,
#define METHODS(Type) CAT(CAT(METHODS_,Type),_)
#define DESCRIPTORS(T) template<> CallSite<T> CallSiteList<T>::methods[] = { METHODS(T)(Record) };

#define TYPE_LIST(_) \
    _(Model_device) \
    _(Model_core) \
    // end TYPE_LIST

TYPE_LIST(DESCRIPTORS)

int main()
{
    Model_device *rec = model_ctor("attiny1616");
    Model_core *core = rec->getCore(0);

    run_tests(rec, CallSiteList<Model_device>::methods);
    run_tests(core, CallSiteList<Model_core>::methods);
}

