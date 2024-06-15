CC = gcc
CFLAGS = -Wall -g
SRC = parser.c reducer.c io.c main.c
OBJDIR = obj
OBJS = $(patsubst %.c, $(OBJDIR)/%.o, $(SRC))
EXECUTABLE = converter

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

run: $(EXECUTABLE)
	./$(EXECUTABLE)

clean:
	rm -f $(OBJS) $(EXECUTABLE)
	rm -rf $(OBJDIR)

.PHONY: all run clean
