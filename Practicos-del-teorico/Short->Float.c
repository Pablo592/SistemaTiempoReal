#include <stdio.h>
#include <math.h>

int main()
{

    float num_float;

    short num_short = pow(-2,10);

    num_float =  (float)num_short;

    printf("El valor de la variable short %d  es de %f como una variable float\n",num_short, num_float);

    return 0;
}