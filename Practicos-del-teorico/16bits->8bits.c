#include <stdio.h>
#include <stdint.h>

int main()
{
    uint16_t var16bits = 0xABCD; // variable de 16 bits
    uint8_t var8bits;            // variable de 8 bits

    var8bits = (var16bits >> 8) & 0xFF; // convertir a 8 bits mediante desplazamiento y máscara

    // imprimir el resultado
    printf("Valor de 16 bits: %x\n", var16bits);
    printf("Valor de 8 bits: %x\n", var8bits);
}

/*
En este ejemplo, la variable var16bits contiene el valor hexadecimal 0xABCD, que es un número de 16 bits.
El operador de desplazamiento hacia la derecha ">>" se utiliza para desplazar el valor de 16 bits 8 bits hacia la derecha,
lo que elimina los 8 bits menos significativos y mantiene los 8 bits más significativos.
Luego, se aplica una máscara utilizando el operador "&" para mantener solo los 8 bits menos significativos.
El resultado se almacena en la variable var8bits, que es una variable de 8 bits.
Por último, se imprimen los valores originales y convertidos mediante la función printf().
*/