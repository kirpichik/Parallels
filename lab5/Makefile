
CC=mpicc
CFLAGS=-c -Wall -std=c99
LDFLAGS=
SOURCES=main.c model.c executor.c
HEADERS=
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=lab

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) $(HEADERS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

%.o: %.c Makefile
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(OBJECTS) $(EXECUTABLE)

clear: clean

rebuild: clean all

