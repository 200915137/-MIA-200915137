#include <stdio.h>
//atoi
#include <stdlib.h>
//directorios
#include <dirent.h>
//crear directorios
#include <sys/stat.h>
//comparacion de cadenas
#include <string.h>
//time
#include <time.h>

struct struc_disco{
    char nombre[256];
    char tamanio[9];
    char particion[9];
    char puntero[9];
};

void menu();
void comandos();
void crear_disco();

int main()
{
    char c;

   printf("Enter character: ");
   c = getchar();

   printf("Character entered: ");
   putchar(c);

    //menu();
     //comandos();
    return 0;
}

void menu()
{
   int opciones;
    FILE *f;

    do{
        char codigo[10];
        char nombre[50];
        char telefono[12];//+502 4285-5821

        char palabra[30];


        printf("MENU PRINCIPAL\n\n");
        printf("1. Crear disco\n");
        printf("2. Buscar\n");
        printf("3. Modificar\n");
        printf("4. Eliminar\n");
        printf("5. Salir\n");
        printf("Elija opcion: ");
        scanf("%d", &opciones);
        switch(opciones)
        {
            case 1:
                crear_disco();
            break;
            case 2:

            break;
            case 3:
            break;
            case 4:
            break;
            case 5:
            break;
            default:
            printf("Error opcion incorrecta\n\n");
            break;
        }
    }while(opciones != 5);

}

void crear_disco(){
    struct struc_disco tdisco;

    printf("Ingresar el nombre del disco \n");
    scanf("%s", tdisco.nombre);
    printf("Ingrese el tamanio del disco");
    int tamanio=0;
    int repetir=0;

    while()
    {

    }

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
