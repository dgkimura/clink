SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

all: scc

scc: $(OBJS)
	$(CC) -o $@ $^

.PHONY: clean
clean:
	rm -f $(OBJS) scc
