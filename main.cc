#include <verilated.h>
#include <svdpi.h>

#include <cassert>
#include <cstdio>

#include "model.h"

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

    int rc = model_dtor(dev);
    assert(("destructor succeeded", rc == 0));

    return 0;
}

/* vi:set ts=4 sw=4 et cindent: */
