CXXFLAGS += -g
CPPFLAGS += -I/usr/share/verilator/include
CPPFLAGS += -I/usr/share/verilator/include/vltstd
DEBUGGER = gdb

LINK.o = $(LINK.cc)

all: main ctors

main ctors: LDFLAGS += -Llib
main ctors: LDLIBS += -lattinyxc

ctors: CXXFLAGS += -O

main.o: model.h

run debug: export LD_LIBRARY_PATH=lib
debug: prefix = $(DEBUGGER)
run debug: main
	$(prefix) $(realpath $<)

clean:
	$(RM) main ctors *.o
