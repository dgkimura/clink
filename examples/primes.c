/*
 * TODO allow n through function arguments
 */
void print_primes(int n)
{
    int i;
    int j;
    int array[100];
    for (i=0; i<100; i++) {
        array[i] = 1;
    }

    for (i=2; i<100; i++) {
        for (j=i*2; j<100; j+=i) {
            array[j] = 0;
        }
    }

    for (i=0; i<100; i++) {
        if (array[i] == 1)
        {
            printf("%d\n", i);
        }
    }
}

int main()
{
    print_primes(100);
}
