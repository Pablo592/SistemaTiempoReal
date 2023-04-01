#include <stdio.h>
#include <math.h>

int main()
{

    float num_float = pow(-2,31);

    short num_short;

    num_short =  (short)round(num_float);

    printf("El valor redondeado de %f es %d\n", num_float, num_short);

    return 0;
}