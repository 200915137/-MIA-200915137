/*
 * Universidad de San Carlos de Guatemala
 * Facultad de Ingenieria
 * Escuela de Ciencias y Sistemas
 * Manejo e implementacion de archivos
 * Seccion A-
 * Laboratorio
 * Proyecto Unico
 * Fase 1
 */

//---------------------Area de librerias--------------------------

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

//-------------------------------------------------------------------------

//---------------------Area de estructuras--------------------------

struct struc_disco{
    char nombre[52];
    char tamanio[9];
    char particion[9];
    char puntero[9]; //direccion al mbr
};

//---------------------Area de variables globales--------------------------

//bit utilizado para guardar 0's
char buffer[1];
//contador de fors
int ifor=0;
//direccion del disco actual
char directorio[58];
//extension de Disco ".dsk"
char extension_disco[4]=".dsk";

//-------------------------------------------------------------------------


//--------------------------------Prototipos-------------------------------

void menu();
void comandos();
void crear_disco();
int escribeInfoDirectorio(char *nombreFichero);
void eliminar_disco();
void particionar_disco();
void tipo_de_tamanio();

//-------------------------------------------------------------------------

int main()
{

    menu();
     //comandos();
    return 0;
}

void set_disco(char ruta_carpeta[52],char nombre[52]){
    sprintf(directorio, "%s%s%s",ruta_carpeta, nombre,extension_disco);

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
        printf("2. Eliminar Disco\n");
        printf("3. particionar Disco\n");
        printf("4. Desmontar\n");
        printf("5. Salir\n");
        printf("Elija opcion: ");
        scanf("%d", &opciones);
        switch(opciones)
        {
            case 1:
                crear_disco();
            break;
            case 2:
                eliminar_disco();
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
    char ruta[52];
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

    printf("Ingrese la ruta de destino...\n");
    scanf("%s",&ruta);
    sprintf(tdisco.tamanio, "%d", tamanio);
    sprintf(tdisco.particion, "%d", 0);
    sprintf(tdisco.puntero, "%d", (int)sizeof(tdisco));
    printf("Creando disco...\n");

    //Si el numero que retorna es igual a 1 indica que es un directorio la ruta
    if(escribeInfoDirectorio(ruta)==1)
    {
        printf("se encontro el directorio %s\n", ruta);
    }

    //Si el numero que retorna es igual a 0 indica que es un fichero
    if(escribeInfoDirectorio(ruta)==0)
    {
        /*
         * Se crea un directorio DIRECTORIO con permisos 777 y se comprueba.
         * 4+2+1=7 es decir puedo leer,escribir y ejecutar
         */
        mkdir (ruta, 0777);
        //escribeInfoDirectorio(ruta);
    }

    //creando el disco
    set_disco(ruta,tdisco.nombre);
    FILE *f = fopen (directorio, "w+b");

    if(opt==1)
    {
        for(ifor=0;ifor<tamanio;ifor++)
            fwrite (buffer, sizeof(buffer), 1024, f);
    }

    if(opt==2 || opt==3)
    {
        for(ifor=0;ifor<tamanio*1024;ifor++)
            fwrite (buffer, sizeof(buffer), 1024, f);
    }


    //guardando informacion del disco
    rewind(f);
    fwrite(&tdisco,sizeof(tdisco),1,f);
    fclose(f);
    printf("Disco %s fue creado exitosamente ", tdisco.nombre);
    printf("en %s\n\n", directorio);
}


int escribeInfoDirectorio(char *nombreFichero)
{

    /*
     * Estructura para recoger la información del fichero.
     */
    struct stat datosFichero;

    /*
     * Comprobación del parámetro de entrada
     */
    if (nombreFichero == NULL)
        return;

    /*
     * Empezamos a escribir el resultado de la comprobación.
     */
    printf ("El fichero %s --> ", nombreFichero);

    /*
     * Se comprueba si existe el fichero.
     * ¡Atención!. En esta llamada a lstat() se rellena datosFichero,
     * y por ello se puede usar datosFichero->st_mode en los siguientes if.
     */
    if (lstat (nombreFichero, &datosFichero) == -1)
    {
        printf ("no existe\n");
        return 0;
    }
    else
    {
        /* Se comprueba si es un fichero normal o un link */
        if (S_ISREG(datosFichero.st_mode)){
            printf ("es un fichero\n");
            return 2;
        }else{

            /* Se comprueba si es un directorio */
            if (S_ISDIR(datosFichero.st_mode)){
                printf ("es un directorio\n");
                return 1;
            }

            /* Es otra cosa */
            else
                printf ("Sepa que es...!!\n");
        }

    }
}

void eliminar_disco()
{
    char ruta[52];
    printf("Ingrese la ruta del Disco a eliminar...\n");
    scanf("%s", &ruta);
    if(escribeInfoDirectorio(ruta)==2)
    {
        unlink (ruta);
    }
    else
    {
        printf("No es un fichero la ruta ingresada..!!");
    }

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
