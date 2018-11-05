#include <verilated.h>
#include <svdpi.h>

#include <cassert>
#include <cstdio>

#include "model.hh"
#include "dynamic.hh"

int main(int argc, char **argv)
{
    if (argc < 2)
        return __LINE__;

    Library lib(argv[1]);

    std::printf("svDpiVersion() = %s\n", lib.get_function(svDpiVersion).invoke());
    std::printf("model_api_ver() = %#x\n", lib.get_function(model_api_ver).invoke());

    Model *dev = lib.get_function(model_ctor).invoke("attiny1616");
    assert(("device constructed", dev != NULL));

    svScope scope = lib.get_function(svGetScopeFromName).invoke("TOP.sim_top");
    assert(("found scope", scope != NULL));

    // cannot call C++ methods if we do not know their manglings
    //static_cast<VerilatedScope*>(scope)->scopeDump();

    Core *mc = dev->getCore(0);
    assert(("model core exists", mc != NULL));

    Model *md = mc->getModel();
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

    int rc = lib.get_function(model_dtor).invoke(dev);
    assert(("destructor succeeded", rc == 0));

    return 0;
}

/* vi:set ts=4 sw=4 et cindent: */
