#include <verilated.h>
#include <svdpi.h>

#include <cassert>
#include <cstdio>

#include <dlfcn.h>

#include "model.hh"

class Library
{
public:
    void *handle;

    template<typename FunctionPtr>
    class Calls
    {
        Library &lib;
        const char *name;
    public:
        Calls(Library &lib, const char *name) : lib(lib), name(name) { }
        template<typename ...Params>
        auto operator()(Params... p) -> decltype(static_cast<FunctionPtr>(0)(p...))
        {
            return reinterpret_cast<FunctionPtr>(dlsym(lib.handle, name))(p...);
        }
    };

    Library(const char *path)
        : handle(dlopen(path, RTLD_LAZY | RTLD_LOCAL))
    {
        // no body
    }

    #define get_function(Func) get_function_(static_cast<decltype(Func)*>(0),#Func)
    template<typename F>
    auto get_function_(F *, const char *name) -> Calls<F*>
    {
        return Calls<F*>(*this, name);
    }
};

int main(int argc, char **argv)
{
    if (argc < 2)
        return __LINE__;

    Library lib(argv[1]);

    std::printf("svDpiVersion() = %s\n", lib.get_function(svDpiVersion)());
    std::printf("model_api_ver() = %#x\n", lib.get_function(model_api_ver)());

    Model *dev = lib.get_function(model_ctor)("attiny1616");
    assert(("device constructed", dev != NULL));

    svScope scope = lib.get_function(svGetScopeFromName)("TOP.sim_top");
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

    int rc = lib.get_function(model_dtor)(dev);
    assert(("destructor succeeded", rc == 0));

    return 0;
}

/* vi:set ts=4 sw=4 et cindent: */
