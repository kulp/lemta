CXXFLAGS += -g
CPPFLAGS += -I/usr/share/verilator/include
CPPFLAGS += -I/usr/share/verilator/include/vltstd
DEBUGGER = gdb

LINK.o = $(LINK.cc)

TARGETS = main ctors call

all: $(TARGETS)

$(TARGETS): LDFLAGS += -Llib
$(TARGETS): LDLIBS += -lattinyxc

ctors: CXXFLAGS += -O
call: CXXFLAGS += -O3

main.o: model.h

run debug: export LD_LIBRARY_PATH=lib
debug: prefix = $(DEBUGGER)
run debug: main
	$(prefix) $(realpath $<)

clean:
	$(RM) $(TARGETS) *.o
