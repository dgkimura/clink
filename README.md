# clink

[![Build Status](https://travis-ci.org/dgkimura/scc.svg?branch=master)](https://travis-ci.org/dgkimura/scc)

The clink project is a basic non-optimizing C compiler for the grammar defined
in "The C Programming Language" [1]. It implements Knuth's bottom-up CLR(1)
parse algorithm to build a parse table, construct an AST, and generate x64
assembly.


## Quick start

```
$ make -C src/
$ ./src/clink examples/primes.c
$ as -arch x86_64 examples/primes.s -o examples/primes.o
$ # MacOSX specific
$ ld -arch x86_64 -L /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/lib -lSystem  -o examples/primes examples/primes.o
```


## References
[1] Kernighan, B., & Ritchie D. (1978). The C Programming Language (2nd ed.). pp. 234-239.
