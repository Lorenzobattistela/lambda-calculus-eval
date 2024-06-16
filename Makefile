CC = gcc
CFLAGS = -Wall -g
SRC = parser.c reducer.c io.c main.c hash-table/hash_table.c
OBJDIR = obj
OBJS = $(patsubst %.c, $(OBJDIR)/%.o, $(SRC))
EXECUTABLE = lc

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)/hash-table
	mkdir -p $(OBJDIR)

run: $(EXECUTABLE)
	./$(EXECUTABLE)

clean:
	rm -f $(OBJS) $(EXECUTABLE)
	rm -rf $(OBJDIR)

.PHONY: all run clean
