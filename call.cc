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
struct SiteDescriptor
{
    Monologuist<T> method;
};

template<class T, int N>
static void run_tests(T *t, SiteDescriptor<T> (&methods)[N])
{
    for (SiteDescriptor<T> & c : methods) {
        (t->*c.method)();
    }
}


#include "methods.Model_device.xi"
#include "methods.Model_core.xi"

int main()
{
#define Record(Type,Method) &Type::Method,

    Model_device *rec = model_ctor("attiny1616");
    {
        SiteDescriptor<Model_device> methods[] = {
            METHODS_Model_device_(Record)
        };
        run_tests(rec, methods);
    }

    Model_core *core = rec->getCore(0);
    {
        SiteDescriptor<Model_core> methods[] = {
            METHODS_Model_core_(Record)
        };
        run_tests(core, methods);
    }
}

