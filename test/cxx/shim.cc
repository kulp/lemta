#include "interface.hh"

#include <new> /* for placement new */

extern "C" Breakpoint *Breakpoint__ctor(Breakpoint *self)
{
    return new (self) Breakpoint();
}
