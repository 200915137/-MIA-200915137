#include <stdio.h>
#include <stdlib.h>

struct struc_disco{
    char nombre[256];
    char tamanio[9];
    char particion[9];
    char puntero[9];
};

void comandos();
void crear_disco();

int main()
{
     comandos();
    return 0;
}

void crear_disco(){
printf("hola mundo");

}

void comandos(){
char comando[20] = "";
    int salida = 1;
    while(salida != 0){
        printf("Ingrese el comando: ");
        fflush(stdin);
        scanf("%s", &comando);
        printf("Comando escrito: %s\n", comando);
        if(comando[0] == 's' && comando[1] == 'a' && comando[2] == 'l' && comando[3] == 'i' && comando[4] == 'r'){
            salida = 0;
        }
    }
}
