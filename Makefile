CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -lm
SRC = common.c config.c parser.c reducer.c typechecker.c io.c main.c hash-table/hash_table.c
OBJDIR = obj
OBJS = $(patsubst %.c, $(OBJDIR)/%.o, $(SRC))
EXECUTABLE = lc

TESTDIR = tests
TESTSRC = $(wildcard $(TESTDIR)/*.c)
TESTOBJS = $(patsubst $(TESTDIR)/%.c,$(OBJDIR)/$(TESTDIR)/%.o,$(TESTSRC))
TESTEXE = $(patsubst $(TESTDIR)/%.c,$(TESTDIR)/%,$(filter-out $(TESTDIR)/tests.c,$(TESTSRC)))

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

$(OBJDIR)/%.o: %.c | $(OBJDIR) $(OBJDIR)/hash-table $(OBJDIR)/$(TESTDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR) $(OBJDIR)/hash-table $(OBJDIR)/$(TESTDIR):
	mkdir -p $@

run: $(EXECUTABLE)
	./$(EXECUTABLE)

# Test target
test: | $(OBJDIR)/$(TESTDIR)
	$(MAKE) $(TESTEXE)
	@for test in $(TESTEXE); do \
		echo "Running $$test"; \
		./$$test; \
	done

# Compile tests.c first
$(OBJDIR)/$(TESTDIR)/tests.o: $(TESTDIR)/tests.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compile other test files
$(OBJDIR)/$(TESTDIR)/%.o: $(TESTDIR)/%.c $(OBJDIR)/$(TESTDIR)/tests.o
	$(CC) $(CFLAGS) -c $< -o $@

# Link test executables
$(TESTDIR)/%: $(OBJDIR)/$(TESTDIR)/%.o $(OBJDIR)/$(TESTDIR)/tests.o $(filter-out $(OBJDIR)/main.o,$(OBJS))
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

clean:
	rm -f $(OBJS) $(EXECUTABLE) $(TESTEXE) $(TESTOBJS)
	rm -rf $(OBJDIR)

.PHONY: all run clean test
