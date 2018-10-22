CXXFLAGS += -g
CPPFLAGS += -I/usr/share/verilator/include
CPPFLAGS += -I/usr/share/verilator/include/vltstd

all: main

main: LDFLAGS += -Llib
main: LDLIBS += -lattinyxc

main.o: model.h

run: main
	LD_LIBRARY_PATH=lib $(realpath $<)

clean:
	$(RM) main *.o
