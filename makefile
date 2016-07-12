# source files.
SRC = $(wildcard src/*.cpp)

OBJ = $(SRC:.cpp=.o)

OUT = savestuff

# C++ compiler flags (-g -O2 -Wall)
CXXFLAGS = -fPIC -Wall -Werror -ggdb -std=c++1z -Wno-unused-but-set-variable -I/include

# compiler
CXX = g++

# library paths
LIBS =

.SUFFIXES: .cpp

default: src/variable.o
	$(CXX) -shared -o libsavestuff.so src/variable.o $(LIBS)

demo: $(OUT)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OUT): $(OBJ)
	$(CXX) -o $(OUT) $(OBJ) $(LIBS)

test: demo
	./$(OUT)

debug: demo
	gdb $(OUT) -ex run

clean:
	rm -f $(OBJ) $(OUT)
