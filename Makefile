CC = gcc
CFLAGS = -Wall -g
SRC = parser.c io.c main.c
OBJS = $(SRC:.c=.o)
EXECUTABLE = converter

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

run: $(EXECUTABLE)
	./$(EXECUTABLE)

clean:
	rm -f $(OBJS) $(EXECUTABLE)

.PHONY: all run clean
