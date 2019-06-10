SRCS = $(filter-out main.c, $(wildcard *.c))
OBJS = $(SRCS:.c=.o)

all: scc

scc: main.o $(OBJS)
	$(CC) -o $@ $^

.PHONY: clean
clean:
	rm -f $(OBJS) scc
