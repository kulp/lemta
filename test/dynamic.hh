#ifndef DYNAMIC_HH_
#define DYNAMIC_HH_

#include <dlfcn.h>

class Library
{
public:
    void *handle;

    template<typename FunctionPtr>
    class Calls
    {
        Library *lib;
        const char *name;
    public:
        Calls() : lib(nullptr), name("") { }
        Calls(Library &lib, const char *name) : lib(&lib), name(name) { }
        template<typename ...Params>
        auto invoke(Params... p) -> decltype(static_cast<FunctionPtr>(0)(p...))
        {
            return reinterpret_cast<FunctionPtr>(dlsym(lib->handle, name))(p...);
        }

        // syntax sugar
        template<typename ...Params>
        auto operator()(Params... p) -> decltype(invoke(p...))
        {
            return invoke(p...);
        }
    };

    Library(const char *path)
        : handle(dlopen(path, RTLD_LAZY | RTLD_LOCAL))
    {
        // no body
    }

    ~Library()
    {
        dlclose(handle);
    }

    #define get_function(Func) get_function_(static_cast<decltype(Func)*>(0),#Func)
    template<typename F>
    auto get_function_(F *, const char *name) -> Calls<F*>
    {
        return Calls<F*>(*this, name);
    }
};

#endif

