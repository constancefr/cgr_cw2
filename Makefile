# Compiler and flags
ifeq ($(OS),Windows_NT)
    # Windows-specific compiler and flags
    CXX = g++
    CXXFLAGS = -fdiagnostics-color=always -g -Wall
    RM = del
    EXE = .exe
else
    # Unix-like systems (Linux/macOS) compiler and flags
    CXX = g++
    CXXFLAGS = -fdiagnostics-color=always -g -Wall
    RM = rm -f
    EXE =
endif

# name of output executable
TARGET = raytracer$(EXE)

# find all source files
SRC = $(wildcard *.cpp)

# generate object files
OBJ = $(SRC:.cpp=.o)

# build the executable
all: $(TARGET)

# link object files into the executable
$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET)

# compile source files into object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ) $(TARGET)

# run executable with arguments passed to make
run: $(TARGET)
	./$(TARGET) $(ARGS)

# debug build (`make debug`)
debug: CXXFLAGS += -DDEBUG -O0
debug: $(TARGET)

# run in debug mode
run-debug: debug
	./$(TARGET) $(ARGS)

# Use `make run ARGS="arg1 arg2 arg3 arg4"` to pass arguments when running the executable
