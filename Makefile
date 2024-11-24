# Compiler and flags
CXX = g++
CXXFLAGS = -fdiagnostics-color=always -g -Wall

# Define the name of the output executable
TARGET = raytracer.exe

# Automatically find all .cpp files in the workspace
SRC = $(wildcard *.cpp)

# Generate object files from source files
OBJ = $(SRC:.cpp=.o)

# Default rule: build the executable
all: $(TARGET)

# Rule to link object files into the executable
$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET)

# Rule to compile each .cpp file into an .o file
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up object files and the executable
clean:
	rm -f $(OBJ) $(TARGET)

# Run the executable with arguments passed to make
run: $(TARGET)
	./$(TARGET) $(ARGS)

# Optional debug build (can be invoked with `make debug`)
debug: CXXFLAGS += -DDEBUG -O0
debug: $(TARGET)

# Run in debug mode
run-debug: debug
	./$(TARGET) $(ARGS)

# Use `make run ARGS="arg1 arg2 arg3"` to pass arguments when running the executable

