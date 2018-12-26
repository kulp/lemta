#ifndef DYNAMIC_HH_
#define DYNAMIC_HH_

#include <cstddef>
#include <cstdio>
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
        if (handle == nullptr) {
            fprintf(stderr, "%s\n", dlerror());
        }
    }

    ~Library()
    {
        dlclose(handle);
    }

    #define get_function(Func) get_function_(static_cast<decltype(Func)*>(0), #Func)
    template<typename F>
    auto get_function_(F *, const char *name) -> Calls<F*>
    {
        return Calls<F*>(*this, name);
    }

    void *get_load_base()
    {
        Dl_info info;
        if (dladdr(dlsym(handle, "_init"), &info) == 0)
            return nullptr; // TODO return MAP_FAILED perhaps ?

        return info.dli_fbase;
    }

    ptrdiff_t get_text_len()
    {
        Dl_info info;
        if (dladdr(dlsym(handle, "_init"), &info) == 0)
            return __LINE__;

        void *end = dlsym(handle, "_fini");
        return static_cast<char*>(end) - static_cast<char*>(info.dli_fbase);
    }
};

#endif

