CXX = g++
CXXFLAGS = -std=c++11 -g
OPTFLAGS = -O3 -pthread
TARGET = Lab3
SRCDIR = src
OBJS = main.o EP.o  # Added EP.o here

all: release

debug: CXXFLAGS += -Wall -DDEBUG_FLAG -g
debug: TARGET := $(TARGET)_debug
debug: $(TARGET)

release: CXXFLAGS += $(OPTFLAGS)
release: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

main.o: main.cpp $(SRCDIR)/StandardCell.h  # Dependency on EP.h
	$(CXX) $(CXXFLAGS) -c main.cpp

EP.o: $(SRCDIR)/EP.cpp $(SRCDIR)/StandardCell.h  # Compile EP.cpp
	$(CXX) $(CXXFLAGS) -c $(SRCDIR)/StandardCell.cpp

debug:
	./$(TARGET)_debug ./input/input1 output_debug.out

release:
	./$(TARGET) ./input/input1 output_release.out

clean:
	rm -f $(OBJS) $(TARGET) $(TARGET)_debug output_debug.out output_release.out
