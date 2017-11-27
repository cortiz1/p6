CC=gcc
CCFLAGS=-g -Wall
LDFLAGS=-v
SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)
TARGET=p6

all: $(TARGET)

$(TARGET): $(OBJECTS)
		$(CC) -o $@ $^ $(LDFLAGS)
%.o: %.c %.h
		$(CC) $(CCFLAGS) -c $<
.o: %.c
		$(CC) $(CCFLAGS) -c $<
clean:
	        rm -f *.o $(TARGET) output.txt
