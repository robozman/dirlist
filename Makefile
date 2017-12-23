CC = gcc

CFLAGS = --std=c11 -Wall -g

TARGET = dirlist

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

clean:
	$(RM) $(TARGET)