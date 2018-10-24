#include "model.h"

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

int main()
{
    Model_device *rec = model_ctor("attiny1616");
    Monologuist<Model_device> method(&Model_device::step);
    (rec->*method)();
}

