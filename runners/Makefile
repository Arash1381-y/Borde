CC = nvcc
CC2 = gcc

CPU_FILTERS = $(wildcard ../cpu/*.cpp)
GPU_FILTERS = $(wildcard ../gpu/*.cu)

CPU_LIBS = -lm -lstdc++

# helper file
HELPER = helper.cpp

# runners (all .cpp files in this dir except helper.cpp)
RUNNERS = $(filter-out $(HELPER), $(wildcard *.cpp))

# all runners
ALL = $(RUNNERS:.cpp=)

.PHONY: all cpu gpu clean

all: gpu cpu

cpu: $(ALL:%=%_cpu.out)

gpu: $(ALL:%=%_gpu.out)

%_cpu.out: %.cpp
	$(CC2) -o $@ $< $(CPU_FILTERS) $(CPU_LIBS) $(HELPER)

%_gpu.out: %.cpp
	$(CC) -o $@ $< $(GPU_FILTERS) $(HELPER)


clean:
	rm -f *.out