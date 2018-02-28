
CC=mpicc
CFLAGS=-c -Wall -std=c99
LDFLAGS=-lm
SOURCES=main.c matrix.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=lab

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@
	
clean:
	rm -rf $(OBJECTS) $(EXECUTABLE)

clear: clean

rebuild: clean all

