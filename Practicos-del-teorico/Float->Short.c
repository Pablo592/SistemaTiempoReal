#include <stdio.h>
#include <math.h>

int main()
{

    float num_float = pow(2, 10);

    short num_short;

    if (num_float > 32767)
        num_short = 32767;
    else
        if ((num_float < 0) && (num_float < -32768))
            num_short = -32768;
        else
            num_short = num_float;


    printf("El valor float de %f es un short %d\n", num_float, num_short);

    return 0;
}