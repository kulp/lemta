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

static void step_cb(Core *m, void *userdata)
{
    printf("%s(%p, %p)\n", __func__, m, userdata);
}

static void cycle_cb(Model *m, void *userdata)
{
    printf("%s(%p, %p)\n", __func__, m, userdata);
}

static void stopper_cb(Core *c, void *userdata)
{
    static int count;
    printf("%s(%p, %p) : count = %d\n", __func__, c, userdata, count);
    if (count++ >= reinterpret_cast<long>(userdata))
        c->stop();
}

static void mstopper_cb(Model *m, void *userdata)
{
    static int count;
    printf("%s(%p, %p) : count = %d\n", __func__, m, userdata, count);
    if (count++ >= reinterpret_cast<long>(userdata))
        m->stop();
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

    Core *mc = dev->getCore(0);
    assert(("model core exists", mc != NULL));

    Model *md = mc->getModel();
    assert(("model device is same", md == dev));

    int sc = mc->addStepCallback(step_cb, md);
    int cc = md->addCycleCallback(cycle_cb, mc);

    puts("cycle and step");
    dev->cycle(1);
    mc->step(1);

    mc->removeStepCallback(sc);
    puts("cycle only");
    dev->cycle(1);
    mc->step(1);

    md->removeCycleCallback(cc);
    puts("neither");
    dev->cycle(1);
    mc->step(1);

    {
        puts("testing Core stop");
        int r = mc->addStepCallback(stopper_cb, reinterpret_cast<void*>(5));
        mc->run(0);
        mc->removeStepCallback(r);

        puts("testing Model stop");
        md->addCycleCallback(mstopper_cb, reinterpret_cast<void*>(5));
        mc->run(0);
    }

    puts("testing callback registration");
    for (int j = 0; j < 3; ++j) {
        for (int i = 0; i < 4; ++i) {
            sc = mc->addStepCallback(step_cb, md);
            printf("step callback = %d\n", sc);
            cc = md->addCycleCallback(cycle_cb, mc);
            printf("cycle callback = %d\n", cc);
        }

        for (int i = 0; i < 4; ++i) {
            mc->removeStepCallback(sc);
            md->removeCycleCallback(cc);
        }
    }

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
        ul = -1;
        char buf[1024];
        int result;

        result = mc->getIntProperty(i, &ul, "");
        if (result != -1) {
            std::printf("(C) index = %d = %#x, result = %d, ul = %lu = %#lx\n", i, i, result, ul, ul);
        }
        result = mc->getStringProperty(i, sizeof buf, buf, nullptr);
        if (result != -1) {
            std::printf("(C) index = %d = %#x, result = %d, buf = '%s'\n", i, i, result, buf);
        }

        result = md->getIntProperty(i, &ul, "");
        if (result != -1) {
            std::printf("(M) index = %d = %#x, result = %d, ul = %lu = %#lx\n", i, i, result, ul, ul);
        }
        result = md->getStringProperty(i, sizeof buf, buf, nullptr);
        if (result != -1) {
            std::printf("(M) index = %d = %#x, result = %d, buf = '%s'\n", i, i, result, buf);
        }
    }

    int rc = lib.get_function(model_dtor).invoke(dev);
    assert(("destructor succeeded", rc == 0));

    return 0;
}

/* vi:set ts=4 sw=4 et cindent: */
