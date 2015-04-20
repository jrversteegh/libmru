SOURCES := $(wildcard src/*.cpp)
HEADERS := $(wildcard include/*.h)
TEST_SOURCES := $(wildcard src/test/*.cpp)
TEST_TARGETS := $(patsubst %.cpp, %.run, $(TEST_SOURCES))
TEST_RUN := $(patsubst %.run, %, $(TEST_TARGETS))

all: ninedof
	
ninedof: $(SOURCES) $(HEADERS)
	g++ -DHASMAIN -std=c++0x -o $@ $(SOURCES)

clean:
	rm -f ninedof
	rm -f src/test/*.run

test_%.run: test_%.cpp $(SOURCES) $(HEADERS)
	g++ -std=c++0x -lcppunit -o $@ $< $(SOURCES)

.PHONY: $(TEST_RUN) test test_message
$(TEST_RUN): $(TEST_TARGETS)
	$@.run

test_message:
	@echo '*******************************************************************'
	@echo 'CPPUnit is required for compiling tests.'
	@echo 'e.g on Debian(like) do: "\$ sudo apt-get install libcppunit-dev"'
	@echo 'You may also want to specify the I2C bus number for the 9dof stick'
	@echo 'e.g. "\$ export NINEDOF_BUSNO=1". It defaults to 0'
	@echo '*******************************************************************'

test: test_message $(TEST_RUN)

