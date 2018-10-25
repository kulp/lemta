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

#define CAT_(a,b) a##b
#define CAT(a,b) CAT_(a,b)

#define Record(Type,Method) &Type::Method,
#define METHODS(Type) CAT(CAT(METHODS_,Type),_)
#define DESCRIPTORS(T) template<> SiteDescriptor<T> SiteDescriptorArray<T>::methods[] = { METHODS(T)(Record) };

#define TYPE_LIST(_) \
    _(Model_device) \
    _(Model_core) \
    // end TYPE_LIST

TYPE_LIST(DESCRIPTORS)

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

