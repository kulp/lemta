CXXFLAGS += -g

all: main

main: LDFLAGS += -Llib
main: LDLIBS += -lattinyxc

clean:
	$(RM) main *.o
