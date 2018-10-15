# source files:
INIT_SRC=initialize.cpp
GA_SRC=EvalXOverMutate.cpp
PROG_SRC=Tsp_nix.cpp
TIMER_SRC=Timer.cpp

# object files:
TIMER_OBJ=Timer.o
GA_OBJ=EvalXOverMutate.o

# output files:
INIT=initialize
OUTFILE=Tsp

# detect operating system for setting of c++ compiler and standard
CXX_NIX=g++
CXX_MAC=clang++

STD_NIX=-std=c++14
STD_MAC=-std=c++1z

SYS := $(shell $(CXX) -dumpmachine)

ifneq (, $(findstring linux, $(SYS)))
	STD := $(STD_NIX)
	CXX := $(CXX_NIX)
else
	ifneq (, $(findstring apple, $(SYS)))
		STD := $(STD_MAC)
		CXX := $(CXX_MAC)
	else
		$(error OS not supported!)
	endif
endif

# generic flags:
CFLAGS := $(STD) -Werror -pipe -Ofast -msse4 -ffast-math -ffinite-math-only
LAB_CFLAGS := $(STD) -Werror -pipe -Ofast -msse4 
COMP_ONLY=-c
DRY_RUN_FLAGS := -fsyntax-only $(STD)

# file specific flags:
PROG_FLAGS=-fopenmp
GA_FLAGS=-fopenmp

# dependencies:
PROG_DEPS=$(TIMER_OBJ) $(GA_OBJ)

dry_run:
	$(CXX) $(DRY_RUN_FLAGS) $(INIT_SRC) $(GA_SRC) $(PROG_SRC) $(TIMER_SRC)

all: $(OUTFILE)

clean_build: clean
	make all

$(OUTFILE): $(GA_OBJ) $(TIMER_OBJ)
	$(CXX) $(PROG_SRC) $(PROG_DEPS) $(PROG_FLAGS) $(CFLAGS) -o $(OUTFILE)
	
$(GA_OBJ): 
	$(CXX) $(COMP_ONLY) $(GA_SRC) $(GA_FLAGS) $(CFLAGS)

$(TIMER_OBJ):
	$(CXX) $(COMP_ONLY) $(TIMER_SRC) $(CFLAGS)

$(INIT):
	$(CXX) $(INIT_SRC) $(CFLAGS) -o $(INIT)

clean:
	rm -f $(OUTFILE) $(INIT) $(GA_OBJ) $(TIMER_OBJ) 

lab: $(TIMER_OBJ) monte integral monte_omp integral_omp

clean_lab:
	rm -f monte monte_omp integral integral_omp Timer.o

monte:
	$(CXX) pi_monte.cpp Timer.o $(LAB_CFLAGS) -o monte

integral:
	$(CXX) pi_integral.cpp Timer.o $(LAB_CFLAGS) -o integral

monte_omp:
	$(CXX) pi_monte_omp.cpp Timer.o $(PROG_FLAGS) $(LAB_CFLAGS) -o monte_omp

integral_omp:
	$(CXX) pi_integral_omp.cpp Timer.o $(PROG_FLAGS) $(LAB_CFLAGS) -o integral_omp


