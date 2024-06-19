CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -lm
SRC = common.c config.c parser.c reducer.c io.c main.c hash-table/hash_table.c
OBJDIR = obj
OBJS = $(patsubst %.c, $(OBJDIR)/%.o, $(SRC))
EXECUTABLE = lc

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p temp
	mkdir -p $(OBJDIR)/hash-table
	mkdir -p $(OBJDIR)

run: $(EXECUTABLE)
	./$(EXECUTABLE)

clean:
	rm -f $(OBJS) $(EXECUTABLE)
	rm -rf $(OBJDIR)

.PHONY: all run clean
