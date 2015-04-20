SOURCES := $(wildcard src/*.cpp)
TEST_SOURCES := $(wildcard src/test/*.cpp)
TEST_TARGETS := $(patsubst %.cpp, %.run, $(TEST_SOURCES))
TEST_RUN := $(patsubst %.run, %, $(TEST_TARGETS))

all: ninedof
	
ninedof: $(SOURCES)
	g++ -DHASMAIN -std=c++0x -o $@ $(SOURCES)

clean:
	rm -f ninedof
	rm -f src/test/*.run

test_%.run: test_%.cpp
	g++ -std=c++0x -lcppunit -o $@ $< $(SOURCES)

.PHONY: $(TEST_RUN)
$(TEST_RUN): $(TEST_TARGETS)
	$@.run
         

test: $(TEST_RUN)
