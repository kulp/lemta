#include <verilated.h>

#include <cstdio>

class Model_device;

extern "C" int model_api_ver();
extern "C" Model_device *model_ctor(const char *);
extern "C" int model_dtor(Model_device *);

int main()
{
    std::printf("model_api_ver() = %#x\n", model_api_ver());

    Model_device *dev = model_ctor("ATtiny1616");
    std::printf("dev = %#p\n", dev);

    Verilated::internalsDump();

    int rc = model_dtor(dev);
    std::printf("rc = %#d\n", rc);

    return 0;
}

/* vi:set ts=4 sw=4 et cindent: */
