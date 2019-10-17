UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S), Linux)
	TEST_LIBS =-lcheck_pic -lsubunit -lpthread -lrt -lm
endif
ifeq ($(UNAME_S), Darwin)
	TEST_LIBS =-lcheck
endif

TEST_SRCS = $(wildcard test_*.c)
TEST_OBJS = $(TEST_SRCS:.c=.o)

SRCS = $(filter-out main.c, $(filter-out $(TEST_SRCS), $(wildcard *.c)))
OBJS = $(SRCS:.c=.o)
CFLAGS = -g

all: scc test_scc

scc: main.o $(OBJS)
	$(CC) -g -o $@ $^

test_scc: $(TEST_OBJS) $(OBJS)
	$(CC) -g $^ -o $@ $(TEST_LIBS)

.PHONY: clean
clean:
	rm -f $(OBJS) $(TEST_OBJS) main.o scc
