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

template<class T, typename U>
static void test(T *that, U T::* method)
{
    Monologuist<T> wrap(method);
    (that->*wrap)();
}

template<class T>
struct SiteDescriptor
{
    Monologuist<T> method;
};

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

        for (SiteDescriptor<Model_device> & c : methods) {
            (rec->*c.method)();
        }
    }

    Model_core *core = rec->getCore(0);
    {
        SiteDescriptor<Model_core> methods[] = {
            METHODS_Model_core_(Record)
        };

        for (SiteDescriptor<Model_core> & c : methods) {
            (core->*c.method)();
        }
    }
}

