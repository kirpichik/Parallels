
CC=mpicc
CFLAGS=-c -Wall -std=c99 -Wno-deprecated-declarations -DDEBUG_LEVEL
LDFLAGS=-lm
SOURCES=main.c
HEADERS=
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

