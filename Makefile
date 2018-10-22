CXXFLAGS += -g
CPPFLAGS += -I/usr/share/verilator/include

all: main

main: LDFLAGS += -Llib
main: LDLIBS += -lattinyxc

clean:
	$(RM) main *.o
