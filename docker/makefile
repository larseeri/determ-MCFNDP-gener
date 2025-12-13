EXT = cpp
CXX = g++
TARGET = determ_gen

ADDFLAGS = -g -Wextra -Wall -static #-O3
CXXFLAGS = -std=c++17
INCFLAGS = -I.
LDFLAGS  = -L. 

SRC = $(wildcard *.$(EXT))
OBJ = $(SRC:.$(EXT)=.o)
OBJDIR = .

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) -o $@ $^  $(ADDFLAGS) $(CXXFLAGS) $(LDFLAGS)

$(OBJDIR)/%.o: %.$(EXT) *.h
	$(CXX) -o $@ -c $<  $(ADDFLAGS) $(CXXFLAGS) $(INCFLAGS)

clean:
	@rm -rf *.o
	@rm -f $(TARGET)
