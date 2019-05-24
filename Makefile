UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S), Linux)
	TEST_LIBS =-lcheck_pic -lpthread -lrt -lm
endif
ifeq ($(UNAME_S), Darwin)
	TEST_LIBS =-lcheck
endif

all: scc

scc:
	$(CC) -g main.c scanner.c -o $@ ${LIBS}

clean:
	rm -f scc *.o *.a
