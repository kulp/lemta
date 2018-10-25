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

template<class T>
struct SiteDescriptorArray
{
    static SiteDescriptor<T> methods[];
};

#define Record(Type,Method) &Type::Method,
template<>
SiteDescriptor<Model_device> SiteDescriptorArray<Model_device>::methods[] = { METHODS_Model_device_(Record) };

template<>
SiteDescriptor<Model_core> SiteDescriptorArray<Model_core>::methods[] = { METHODS_Model_core_(Record) };

int main()
{
    Model_device *rec = model_ctor("attiny1616");
    {
        run_tests(rec, SiteDescriptorArray<Model_device>::methods);
    }

    Model_core *core = rec->getCore(0);
    {
        run_tests(core, SiteDescriptorArray<Model_core>::methods);
    }
}

