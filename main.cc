#include <verilated.h>
#include <svdpi.h>

#include <cassert>
#include <cstdio>

#include "model.hh"

int main()
{
    std::printf("svDpiVersion() = %s\n", svDpiVersion());
    std::printf("model_api_ver() = %#x\n", model_api_ver());

    Model_device *dev = model_ctor("ATtiny1616");
    assert(("device constructed", dev != NULL));

    svScope scope = svGetScopeFromName("TOP.sim_top");
    assert(("found scope", scope != NULL));

    ((VerilatedScope*)scope)->scopeDump();

    Model_core *mc = dev->getCore(0);
    assert(("model core exists", mc != NULL));

    Model_device *md = mc->getModel();
    assert(("model device is same", md == dev));

    dev->cycle(1);

    unsigned long ul = -1;
    void *result;
    // Avr8:
    // ; 0x400 -> 0x0
    // ; 0x404 -> 0x20
    // ; 0x405 -> 0x0
    // ; 0x406 -> 0x1
    // ; else  -> failure
    result = mc->getIntProperty(0x404, &ul, "testy");
    std::printf("result = %p, ul = %lu\n", result, ul);

    int rc = model_dtor(dev);
    assert(("destructor succeeded", rc == 0));

    return 0;
}

/* vi:set ts=4 sw=4 et cindent: */
