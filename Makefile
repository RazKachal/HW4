CC = gcc
CFLAGS = -Wall -std=c99 -fPIC
LDFLAGS = -shared
LIB_PATH = .  # Use the current folder for the linked-list library
LIBS = -L$(LIB_PATH) -llinked-list

all: libgrades.so

grades.o: grades.c grades.h
	$(CC) $(CFLAGS) -c grades.c -o grades.o

libgrades.so: grades.o
	$(CC) $(LDFLAGS) grades.o $(LIBS) -o libgrades.so

clean:
	rm -f grades.o libgrades.so

.PHONY: all clean