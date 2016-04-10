TARGETS := ninedof tendof
LIBS := -lCGAL -lboost_system -lboost_filesystem
# Rounding math required by CGAL
# CFLAGS := -frounding-math -std=c++11 -O2 
# Debug flags
CFLAGS := -frounding-math -std=c++11 -g
SOURCES := $(wildcard src/*.cc)
HEADERS := $(wildcard include/*.h)
DEFINES := -DCGAL_NDEBUG
OBJS := $(patsubst %.cc, %.o, $(SOURCES))


all: $(TARGETS)
	
.PRECIOUS: %.o
%.o: %.cc $(HEADERS) Makefile
	g++ -c $(CFLAGS) $(DEFINES) -o $@ $< 

%.o: %.cpp $(HEADERS) Makefile
	g++ -c $(CFLAGS) $(DEFINES) -o $@ $<


%: src/%.o Makefile $(OBJS) 
	g++ -o $@ $< $(OBJS) $(LIBS) 

run: $(TARGETS)
	./$<

run9: ninedof
	./$<

run10: tendof
	./$<

timeit: $(TARGETS)
	time ./$<

clean:
	rm -f $(TARGETS) $(TEST_TARGETS) $(OBJS) $(CLEAN_TARGETS)

include src/test/module.mk

show:
	@echo $(HEADERS)
	@echo $(SOURCES)
	@echo $(OBJS)
	@echo $(TEST_SOURCES)
	@echo $(TEST_TARGETS)
	@echo $(TEST_RUN)

