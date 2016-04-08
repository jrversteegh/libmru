# Directory tracking: push this directory
dirstack := $(dirstack).x
d_$(dirstack) := $(d)
d := $(dir $(lastword $(MAKEFILE_LIST)))

TEST_SOURCES := $(wildcard $(d)*.cpp)
TEST_TARGETS := $(patsubst %.cpp, %.run, $(TEST_SOURCES))
TEST_RUN := $(patsubst %.run, %, $(TEST_TARGETS))

TEST_EXTRA_SOURCES := $(d)boost_copy_file.cc
TEST_EXTRA_OBJS := $(patsubst %.cc, %.o, $(TEST_EXTRA_SOURCES))

CLEAN_TARGETS := $(CLEAN_TARGETS) $(d)calibration/temp.ini $(d)calibration/copy.ini $(TEST_EXTRA_OBJS)

$(TEST_EXTRA_OBJS): $(TEST_EXTRA_SOURCES) 
	g++ -c -O2 -o $@ $^    

$(d)test_%.run: $(d)test_%.cpp $(OBJS) $(TEST_EXTRA_OBJS)
	g++ $(CFLAGS) -o $@ $< $(OBJS) $(TEST_EXTRA_OBJS) -lcppunit $(LIBS)  
 
.PHONY: $(TEST_RUN) test test_message 
$(TEST_RUN): $(TEST_TARGETS) 
	$@.run 
 
test_message: 
	@echo '*******************************************************************' 
	@echo 'CPPUnit is required for compiling tests.' 
	@echo 'e.g on Debian(like) do: "$$ sudo apt-get install libcppunit-dev"' 
	@echo 'You may also want to specify the I2C bus number for the 9dof stick' 
	@echo 'e.g. "$$ export NINEDOF_BUSNO=1".'  
	@echo '*******************************************************************' 
 
test: test_message $(TEST_RUN) 

# Directory tracking: restore previous directory
d := $(d_$(dirstack))
dirstack := $(basename $(dirstack))

