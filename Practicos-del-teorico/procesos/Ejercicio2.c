//Escriba un programa que sea capaz de generar 10 procesos hijos los cuales 
//deben imprimir su PID. Usar la herramienta pstree y top para analizar los resultados.
#include <stdio.h>
#include <unistd.h>

int var = 1;
int main(void) {
    pid_t pidC;
    
    printf( "Arranca el proceso del codigo cuyo PID = %d \n",getpid());
    for(int i = 0;i<10;i++){
    pidC = fork(); //Se crean 2 procesos, el padre y el hijo
                    //el padre tiene asignado un PID diferente de cero y el hijo tiene asigando el cero
    if(pidC==0){
        break;
    }

    }
    printf( "Proceso con PID = %d ,pidC = %d ejecutandose \n",getpid(), pidC );

    if(pidC>0){
        var = 2; //el padre setea el valor 2 a la variable var
    }else{
        if(pidC==0){
            var = 3; //el hijo asigna el valor 3 a la variable var    
        }else{
            var = 4; //error al crear el hijo o el padre
        }
    }
    //esta porcion de codigo lo ejecutan todos los procesos, tanto el padre como el hijo
    while(1){
        sleep(2);// lo ralentiza al proceso porque sino imprime muy rapido
        printf("Proceso con PID = %d ,var = %d ejecutandose \n",getpid(), var); //cuando se ejecute el hijo va a imprimirse el valor 3 y cuando lo hace el padre se imprime 2
    }
    
}


