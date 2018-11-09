#include <verilated.h>
#include <svdpi.h>

#include <cassert>
#include <cstdio>
#include <setjmp.h>
#include <signal.h>

#include "model.hh"
#include "dynamic.hh"

static jmp_buf jmp;
static void segv_handler(int /*signo*/, siginfo_t * /*info*/, void *)
{
    siglongjmp(jmp, 1); // this `1` increments the counter in the crashing loop
}

static void cycle_cb(Model *m, void *userdata)
{
    printf("%s(%p, %p)\n", __func__, m, userdata);
}

int main(int argc, char **argv)
{
    if (argc < 3)
        return __LINE__;

    Library lib(argv[1]);

    std::printf("svDpiVersion() = %s\n", lib.get_function(svDpiVersion).invoke());
    std::printf("model_api_ver() = %#x\n", lib.get_function(model_api_ver).invoke());

    Model *dev = lib.get_function(model_ctor).invoke(argv[2]);
    assert(("device constructed", dev != NULL));

    svScope scope = lib.get_function(svGetScopeFromName).invoke("TOP.sim_top");
    assert(("found scope", scope != NULL));

    // cannot call C++ methods if we do not know their manglings
    //static_cast<VerilatedScope*>(scope)->scopeDump();

    Core *mc = dev->getCore(0);
    assert(("model core exists", mc != NULL));

    Model *md = mc->getModel();
    assert(("model device is same", md == dev));

    md->addCycleCallback(cycle_cb, mc);

    dev->cycle(1);

    unsigned long ul = -1;
    // Avr8:
    // ; 0x400 -> 0x0
    // ; 0x404 -> 0x20
    // ; 0x405 -> 0x0
    // ; 0x406 -> 0x1
    // ; else  -> failure

    struct sigaction action = {};
    action.sa_sigaction = segv_handler;
    action.sa_flags = SA_SIGINFO;

    if (sigaction(SIGSEGV, &action, NULL) != 0)
        perror("sigaction");

    for (volatile int i = 0; i < 0x500; ++i) {
        if (sigsetjmp(jmp, 1/*nonzero*/)) {
            std::printf("index = %d = %#x caused SIGSEGV, skipping\n", i, i);
            i++;
        }
        int result = mc->getIntProperty(i, &ul, "");
        if (result != -1) {
            std::printf("index = %d = %#x, result = %d, ul = %lu\n", i, i, result, ul);
        }
    }

    int rc = lib.get_function(model_dtor).invoke(dev);
    assert(("destructor succeeded", rc == 0));

    return 0;
}

/* vi:set ts=4 sw=4 et cindent: */
