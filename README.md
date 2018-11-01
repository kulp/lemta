# Goal
This project seeks to derive, by good-faith clean-room engineering, sufficient
interface definitions to support the development of F/OSS simulation tools that
use downloadable [pack files](http://packs.download.atmel.com) from Atmel /
Microchip.

## Non-goal
While these pack files include compiled GNU/Linux-compatible shared libraries
which in some cases contain Verilog models converted to C++ using
[Verilator](https://www.veripool.org/wiki/verilator), no attempt is made to
reverse-engineer the underlying hardware description. Instead, the intent is
merely to provide header files sufficient to `dlopen()` the library and
usefully call its public interfaces (i.e. whatever interfaces the Atmel
simulator needs to use in order to provide its simulation functionality).

It would be truly regrettable if this project were to cause Atmel / Microchip
to change their packs format, but given the implied desire for backward
compatibility, this is not expected to happen.

# Status
As of this writing, the project consists essentially of the partially-inferred
interface heaers in `include/` and a few testbench programs in `test/`. Run
`make -C test check` from the top level to run self-tests.

# Requirements
The interface header files in `include/` were derived on a GNU/Linux-compatible
system and may require such a system to function, although in theory they
should be portable to Windows (since the manufacturer supplies Windows DLLs as
well).

The testbenches use [CastXML](https://github.com/CastXML/CastXML) and
[xmllint](http://xmlsoft.org/xmllint.html) to extract information from the
inferred interface headers.
