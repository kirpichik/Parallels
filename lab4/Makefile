
CC=mpic++
CFLAGS=-c -Wall -std=c++11 -DDEBUG_LEVEL
LDFLAGS=
SOURCES=main.cpp solve_data.cpp area.cpp
HEADERS=
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=lab

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) $(HEADERS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

%.o: %.cpp Makefile
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(OBJECTS) $(EXECUTABLE)

clear: clean

rebuild: clean all

