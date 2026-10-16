EXT = cpp
CXX = g++
TARGET = determ_feasib

CPLEX_SOLVER = 0
HIGHS_SOLVER = 0

# CPLEX locations
# Note: specify absolute path of CPLEX_HOME
CPLEX_HOME   = /home/ibm/cplex-studio/22.1.1/
CPLEX_INCDIR = $(CPLEX_HOME)/cplex/include
CONCERT_INCDIR = $(CPLEX_HOME)/concert/include
CPLEX_LIBDIR   = $(CPLEX_HOME)/cplex/lib/x86-64_linux/static_pic/
CONCERT_LIBDIR   = $(CPLEX_HOME)/concert/lib/x86-64_linux/static_pic/

# HIGHS locations
# Note: specify absolute path of HIGHS_HOME
HIGHS_HOME   = ./HIGHS/HiGHS-1.11.0/install
HIGHS_INCDIR = $(HIGHS_HOME)/include/highs/
HIGHS_LIBDIR   = $(HIGHS_HOME)/lib/

ADDFLAGS = -std=gnu++17 -g -Wall #-Wextra #-O3

ifeq ($(CPLEX_SOLVER), 1)
    CXXFLAGS = -DIL_STD -DNDEBUG -DCPLEX
    INCFLAGS = -I. -I$(CPLEX_INCDIR) -I$(CONCERT_INCDIR)
    LDFLAGS  = -L. -L$(CPLEX_LIBDIR) -L $(CONCERT_LIBDIR) -lilocplex -lcplex -lconcert -lm -lpthread -ldl
else 
    ifeq ($(HIGHS_SOLVER), 1)
        CXXFLAGS = -DHIGHS
        INCFLAGS = -I. -I$(HIGHS_INCDIR)
        LDFLAGS  = -L. -L$(HIGHS_LIBDIR) -lhighs -Wl,-rpath,$(HIGHS_LIBDIR)
    else
        $(error ERROR: Specify either CPLEX_SOLVER=1 or HIGHS_SOLVER=1)
    endif
endif

SRC = $(wildcard *.$(EXT))
OBJ = $(SRC:.$(EXT)=.o)
OBJDIR = .

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) -o $@ $^  $(OPTFLAGS) $(LDFLAGS) $(ADDFLAGS)

$(OBJDIR)/%.o: %.$(EXT) *.h
	$(CXX) -o $@ -c $<  $(ADDFLAGS) $(CXXFLAGS) $(INCFLAGS) $(LDFLAGS)

clean:
	@rm -rf *.o 
	@rm -f $(TARGET)
	
