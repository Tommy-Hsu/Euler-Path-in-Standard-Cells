CXX = g++
CXXFLAGS = -std=c++11 -g
OPTFLAGS = -O3 -pthread
TARGET = Lab3
SRCDIR = src
OBJS = main.o StandardCell.o # Add your other object files here

.PHONY: all debug release clean

all: release

debug: CXXFLAGS += -Wall -DDEBUG_FLAG
debug: $(TARGET)_debug
	./$(TARGET)_debug ./input/input1 output_debug.out

release: CXXFLAGS += $(OPTFLAGS)
release: $(TARGET)_release
	./$(TARGET)_release ./input/input1 output_release.out

$(TARGET)_debug: $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)_debug

$(TARGET)_release: $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)_release

main.o: main.cpp $(SRCDIR)/StandardCell.h
	$(CXX) $(CXXFLAGS) -c main.cpp

StandardCell.o: $(SRCDIR)/StandardCell.cpp $(SRCDIR)/StandardCell.h
	$(CXX) $(CXXFLAGS) -c $(SRCDIR)/StandardCell.cpp

clean:
	rm -f $(OBJS) $(TARGET) $(TARGET)_debug $(TARGET)_release output_debug.out output_release.out
