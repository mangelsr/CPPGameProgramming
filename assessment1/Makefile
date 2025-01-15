# Makefile for building the SFML application

# Compiler
CXX = g++
# Compiler flags
CXXFLAGS = -std=c++17
# SFML libraries
LIBS = -lsfml-graphics -lsfml-window -lsfml-system
# Target executable
TARGET = sfml_app.out
# Source files
SRC = main.cpp
# Object files
OBJ = $(SRC:.cpp=.o)

# Default rule
all: $(TARGET)

# Rule to build the target
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ) $(LIBS)

# Rule to build object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean rule
clean:
	rm -f $(OBJ) $(TARGET)

# Phony targets
.PHONY: all clean
