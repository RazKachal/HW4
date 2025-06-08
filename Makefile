CC = gcc
CLINK = $(CC)
CFLAGS = -g -Wall -std=c99
OBJS = grades.o

# The first rule is invoked by default
all: libgrades.so

libgrades.so: $(OBJS)
	$(CLINK) -shared $(OBJS) -o libgrades.so liblinked-list.so

# Full syntax for building grades.o
grades.o: grades.c grades.h 
	$(CC) $(CFLAGS) -fpic -c grades.c

# Custom rule - we can invoke it manually by running "make clean"
clean:
	rm -f *.o libgrades.so
