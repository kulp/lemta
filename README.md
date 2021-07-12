# Goal
This project seeks to derive, by good-faith clean-room reverse-engineering
techniques, sufficient interface definitions and library code to support the
development of [FOSS](http://freeopensourcesoftware.org) simulation tools that
use [packs](https://www.keil.com/pack/doc/CMSIS/Pack/html/packFormat.html) from
[Atmel / Microchip](http://packs.download.atmel.com).

## Non-goal
While these pack files include compiled GNU/Linux-compatible shared libraries
which in some cases contain Verilog models converted to C++ using
[Verilator](https://www.veripool.org/wiki/verilator), no attempt is made to
reverse-engineer the underlying hardware description. Instead, the intent is
to provide header and library files sufficient to `dlopen()` the library and
usefully call its public interfaces (i.e. whatever interfaces the Atmel
simulator needs to use in order to provide its simulation functionality).

# Status
As of this writing, the project consists essentially of the partially-inferred
interface heaers in `include/` and a few testbench programs in `test/`. Run
`make -C test check` from the top level to run self-tests. The test suite is
run as [a "build" on GitHub Actions][1]: ![CI Status][2]

A primordial [LuaJIT](http://luajit.org)-based interface is present in `lua/`.
LuaJIT (as opposed to [Lua](http://www.lua.org)) is specifically required
because [LuaJIT's FFI library](http://luajit.org/ext_ffi.html) is used heavily.

# Requirements
The interface header files in `include/` were derived on a GNU/Linux-compatible
system and may require such a system to function, although in theory they
should be portable to Windows (since the manufacturer supplies Windows DLLs as
well).

The testbenches use [CastXML](https://github.com/CastXML/CastXML) and
[xmllint](http://xmlsoft.org/xmllint.html) to extract information from the
inferred interface headers.

[1]: https://github.com/kulp/lemta/actions/workflows/simple.yaml
[2]: https://github.com/kulp/lemta/actions/workflows/simple.yaml/badge.svg
