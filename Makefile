TARGETS := output/ninedof output/tendof
LIBS := -lCGAL -lboost_system -lboost_filesystem
# Rounding math required by CGAL
# CFLAGS := -frounding-math -std=c++11 -O2 
# Debug flags
CFLAGS := -frounding-math -std=c++11 -g
SOURCES := $(wildcard src/*.cc)
HEADERS := $(wildcard include/*.h)
DEFINES := -DCGAL_NDEBUG
OBJS := $(patsubst src/%.cc, objs/%.o, $(SOURCES))


all: $(TARGETS)
	
.PRECIOUS: objs/%.o
objs/%.o: src/%.cc $(HEADERS) Makefile objs
	g++ -c $(CFLAGS) $(DEFINES) -o $@ $< 

objs/%.obj: src/%.cpp $(HEADERS) Makefile objs
	g++ -c $(CFLAGS) $(DEFINES) -o $@ $<


output/%: objs/%.obj Makefile $(OBJS) output 
	g++ -o $@ $< $(OBJS) $(LIBS) 

output:
	mkdir -p output

objs:
	mkdir -p objs

run: $(TARGETS)
	./$<

run9: output/ninedof
	./$<

run10: output/tendof
	./$<

timeit: $(TARGETS)
	time ./$<

clean:
	rm -rf $(TARGETS) $(TEST_TARGETS) $(OBJS) $(CLEAN_TARGETS) output objs

include src/test/module.mk

show:
	@echo $(HEADERS)
	@echo $(SOURCES)
	@echo $(OBJS)
	@echo $(TEST_SOURCES)
	@echo $(TEST_TARGETS)
	@echo $(TEST_RUN)

