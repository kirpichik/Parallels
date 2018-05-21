
CC=mpicc
CFLAGS=-c -Wall -std=c99 -DDEBUG_LEVEL
LDFLAGS=-lm
SOURCES=main.c matrix.c
HEADERS=matrix.h
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=lab

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) $(HEADERS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(OBJECTS) $(EXECUTABLE)

clear: clean

rebuild: clean all

