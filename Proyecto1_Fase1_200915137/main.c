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
    char nombre[52];
    char tamanio[9];
    char particion[9];
    char puntero[9];
};

//guarda la ruta de los discos duros del sistema
char sistema[8]="sistema/";
//bit utilizado para guardar 0's
char buffer[1];
//contador de fors
int ifor=0;

//direccion del disco actual
char directorio[58];

void menu();
void comandos();
void crear_disco();
void tipo_de_tamanio();
int main()
{


    menu();
     //comandos();
    return 0;
}

void set_disco(char nombre[50]){
    sprintf(directorio, "%s%s", sistema, nombre);
}

void menu()
{
   int opciones;
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
    int opt=0;
    printf("Ingresar el nombre del disco. \n");
    scanf("%s", tdisco.nombre);
    printf("\n");
    printf("1. En Kilobytes\n");
    printf("2. En MegaBytes\n");
    printf("3. Por default\n");
    printf("Elija si es en megas o en kilo\n");
    scanf("%d",&opt);
    printf("ingrese el tamanio del disco\n");


    int tamanio=0;
    int repetir=0;

    while(repetir==0)
    {
        if(scanf("%d",&tamanio)==0)
        {
            while(getchar() != '\n')
            {
                printf("Error...!! Asigne el tamanio...");
                scanf("%d",&tamanio);
            }
        }

        while(tamanio==0)
        {
            printf("Ingrese un tamanio mayor que Cero\n");
        }


        if(tamanio!=0){
            repetir=1;
        }

    }

    //printf("Ingrese la ruta de destino...\n");
    //scanf("%s",&directorio);
    //directorio=(char) ;
    sprintf(tdisco.tamanio, "%d", tamanio);
    sprintf(tdisco.particion, "%d", 0);
    sprintf(tdisco.puntero, "%d", (int)sizeof(tdisco));
    printf("Creando disco...\n");


    //creando el disco
    set_disco(tdisco.nombre);
    FILE *f = fopen (directorio, "w+b");
    for(ifor=0;ifor<tamanio*1024;ifor++)
        fwrite (buffer, sizeof(buffer), 1024, f);
    //guardando informacion del disco
    rewind(f);
    fwrite(&tdisco,sizeof(tdisco),1,f);
    fclose(f);
    printf("Disco %s creado exitosamente\n", tdisco.nombre);
}

void tipo_de_tamanio()
{
int opciones;
    do{
        char codigo[10];
        char nombre[50];
        char telefono[12];//+502 4285-5821

        char palabra[30];


        printf("MENU PRINCIPAL\n\n");
        printf("1. Crear disco en Kilobytes\n");
        printf("2. Crear disco en Megabytes\n");
        printf("3. Salir\n");
        printf("Elija opcion: ");
        scanf("%d", &opciones);
        switch(opciones)
        {
            case 1:

            break;
            case 2:

            break;

            default:
            printf("Se creo disco con tamanio en Megas\n\n");
            break;
        }
    }while(opciones != 2);
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
