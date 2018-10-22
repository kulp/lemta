#include <verilated.h>
#include <svdpi.h>

#include <cassert>
#include <cstdio>

class Model_device;

extern "C" int model_api_ver();
extern "C" Model_device *model_ctor(const char *);
extern "C" int model_dtor(Model_device *);

int main()
{
    std::printf("svDpiVersion() = %s\n", svDpiVersion());
    std::printf("model_api_ver() = %#x\n", model_api_ver());

    Model_device *dev = model_ctor("ATtiny1616");
    assert(("device constructed", dev != NULL));

    svScope scope = svGetScopeFromName("TOP.sim_top");
    assert(("found scope", scope != NULL));

    ((VerilatedScope*)scope)->scopeDump();

    int rc = model_dtor(dev);
    assert(("destructor succeeded", rc == 0));

    return 0;
}

/* vi:set ts=4 sw=4 et cindent: */
