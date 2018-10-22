CXXFLAGS += -g
CPPFLAGS += -I/usr/share/verilator/include
CPPFLAGS += -I/usr/share/verilator/include/vltstd

all: main

main: LDFLAGS += -Llib
main: LDLIBS += -lattinyxc

clean:
	$(RM) main *.o
