

# compile all the runners

CC = gcc

# link math library
LIBS = -lm -lstdc++

# filters .cpp files in filters dir which needs to be compiled with runnners
FILTERS = $(wildcard ../filters/*.cpp)

# runners
RUNNERS = $(wildcard *.cpp)

# all runners
ALL = $(RUNNERS:.cpp=)

# compile all runners
all: $(ALL)

# compile each runner from its .cpp file to .o file
$(ALL): %: %.cpp
	$(CC) -o $@.out $< $(FILTERS) $(LIBS)

# clean all runners
clean:
	rm -f *.out
