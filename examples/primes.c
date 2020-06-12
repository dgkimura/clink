void print_primes(int n)
{
    int i;
    int j;
    int array[n];
    for (i=0; i<n; i++) {
        array[i] = 1;
    }

    for (i=2; i<n; i++) {
        for (j=i*2; j<n; j+=i) {
            array[j] = 0;
        }
    }

    for (i=0; i<n; i++) {
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
