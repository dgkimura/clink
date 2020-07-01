void print_factorial(int n)
{
    int i;
    int product;
    product = 1;

    for (i=1; i<=n; i++)
    {
        product *= i;
    }
    printf("%d\n", product);
}

void print_factorials(int n)
{
    int i;

    for (i=1; i<=n; i++)
    {
        print_factorial(i);
    }
}

int main()
{
    print_factorials(5);
}
