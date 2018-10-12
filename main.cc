#include <cstdio>

class Model_device;

extern "C" int model_api_ver();
extern "C" Model_device *model_ctor(const char *);
extern "C" void model_dtor(Model_device *);

int main()
{
    std::printf("model_api_ver() = %#x\n", model_api_ver());

    Model_device *dev = model_ctor("ATtiny1616");

    model_dtor(dev);

    return 0;
}

