#include <iostream>
#include <cstdlib>

int a[512], b[512], c[512];
int main()
{
    for (int i = 0; i < 51; i++)
    {
        c[i] = a[i] * b[i];
    }
    return 0;
}
