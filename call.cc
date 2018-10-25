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
    T *base;
    Monologuist<T> method;
};

int main()
{
    Model_device *rec = model_ctor("attiny1616");

    SiteDescriptor<Model_device> methods[] = {
        rec, &Model_device::step,
    };

    for (SiteDescriptor<Model_device> & c : methods) {
        (c.base->*(c.method))();
    }

    Model_core *core = rec->getCore(0);

    test(core, &Model_core::step);
}

