SOURCES := $(wildcard src/*.cpp)

all: ninedof

ninedof: $(SOURCES)
	g++ -o $@ $(SOURCES)
