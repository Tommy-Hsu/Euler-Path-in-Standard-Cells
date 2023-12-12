CXX = g++
# CXXFLAGS = -std=c++11 -g -O3 -pthread
CXXFLAGS = -std=c++11 -g
TARGET = Lab3
SRCDIR = src
OBJS = main.o StandardCell.o # Add your other object files here

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

main.o: main.cpp $(SRCDIR)/StandardCell.h
	$(CXX) $(CXXFLAGS) -c main.cpp

StandardCell.o: $(SRCDIR)/StandardCell.cpp $(SRCDIR)/StandardCell.h
	$(CXX) $(CXXFLAGS) -c $(SRCDIR)/StandardCell.cpp

clean:
	rm -f $(OBJS) $(TARGET) output_debug.out output_release.out
