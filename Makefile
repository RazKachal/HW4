CC = gcc
CLINK = $(CC)
CFLAGS = -g -Wall -std=c99
LDFLAGS = -shared
LIB_PATH = .  # Use the current folder for the linked-list library
LIBS = -L$(LIB_PATH) -llinked-list
OBJS = grades.o

# The first rule is invoked by default
all: libgrades.so

libgrades.so: $(OBJS)
	$(CLINK) $(LDFLAGS) $(OBJS) $(LIBS) -o libgrades.so

# Full syntax for building grades.o
grades.o: grades.c grades.h
	$(CC) $(CFLAGS) -c grades.c -o grades.o

# Custom rule - we can invoke it manually by running "make clean"
clean:
	rm -f *.o libgrades.so
