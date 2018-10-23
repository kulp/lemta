CXXFLAGS += -g
CPPFLAGS += -I/usr/share/verilator/include
CPPFLAGS += -I/usr/share/verilator/include/vltstd
DEBUGGER = gdb

LINK.o = $(LINK.cc)

all: main

main: LDFLAGS += -Llib
main: LDLIBS += -lattinyxc

main.o: model.h

run debug: export LD_LIBRARY_PATH=lib
debug: prefix = $(DEBUGGER)
run debug: main
	$(prefix) $(realpath $<)

clean:
	$(RM) main *.o
