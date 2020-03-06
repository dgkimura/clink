UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S), Linux)
	TEST_LIBS =-lcheck_pic -lsubunit -lpthread -lrt -lm
endif
ifeq ($(UNAME_S), Darwin)
	TEST_LIBS =-lcheck
endif

CFLAGS = -g

all: scc test_scc

.ONESHELL:
scc:
ifeq (,$(wildcard parsetable.h))
	$(CC) -DGENPT=1 parser.c utilities.c -o genpt
	./genpt
endif
	$(CC) -g -o ast.o -c ast.c
	$(CC) -g -o main.o -c main.c
	$(CC) -g -o parser.o -c parser.c
	$(CC) -g -o scanner.o -c scanner.c
	$(CC) -g -o generator.o -c generator.c
	$(CC) -g -o utilities.o -c utilities.c
	$(CC) main.o ast.o parser.o scanner.o generator.o utilities.o -o scc

test_scc: scc
	$(CC) -g -o test_scc.o -c test_scc.c
	$(CC) ast.o parser.o scanner.o generator.o utilities.o test_scc.o -o test_scc ${TEST_LIBS}

.PHONY: clean
clean:
	rm -f *.o scc test_scc
