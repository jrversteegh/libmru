TARGET := ninedof
LIBS := -lCGAL
# Rounding math required by CGAL
CFLAGS := -frounding-math -std=c++11 -O2 
SOURCES := $(wildcard src/*.cc)
TARGET_SOURCE := src/$(TARGET).cpp
TARGET_OBJ := src/$(TARGET).o
HEADERS := $(wildcard include/*.h)
DEFINES := -DCGAL_NDEBUG
OBJS := $(patsubst %.cc, %.o, $(SOURCES))
TEST_SOURCES := $(wildcard src/test/*.cpp)
TEST_TARGETS := $(patsubst %.cpp, %.run, $(TEST_SOURCES))
TEST_RUN := $(patsubst %.run, %, $(TEST_TARGETS))


all: $(TARGET)
	
%.o: %.cc $(HEADERS) Makefile
	g++ -c $(CFLAGS) $(DEFINES) -o $@ $< 

%.o: %.cpp $(HEADERS) Makefile
	g++ -c $(CFLAGS) $(DEFINES) -o $@ $<

$(TARGET): $(OBJS) $(TARGET_OBJ)
	g++ $(CFLAGS) -o $@ $(OBJS) $(LIBS) $(TARGET_OBJ) 

run: $(TARGET)
	./$(TARGET)

timeit: $(TARGET)
	time ./$(TARGET)

clean:
	rm -f $(TARGET) $(TEST_TARGETS) $(OBJS)

test_%.run: test_%.cpp $(OBJS) 
	g++ $(CFLAGS) -o $@ $< $(OBJS) $(LIBS) -lcppunit


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

show:
	@echo $(HEADERS)
	@echo $(SOURCES)
	@echo $(OBJS)

