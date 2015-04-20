SOURCES := $(wildcard src/*.cpp)

all: ninedof

ninedof: $(SOURCES)
	g++ -std=c++0x -o $@ $(SOURCES)

clean:
	rm -f ninedof
