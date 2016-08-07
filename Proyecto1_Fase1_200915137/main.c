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

struct struct_mbr{
    char mbr_byte_particion[10];//Contiene el byte donde inicia la partición.
    char mbr_byte_particion_desactivada[10];//Contiene el byte donde inicia la partición.
    char mbr_tipo_sistema_archivos[7];//Contiene el tipo de sistema de archivos.
    char mbr_tipo_particion[12];//Contiene el tipo de partición.
    char mbr_particion_activa[4];//Indica si la partición es la activa, está será la por defecto que iniciará para realizar operaciones sobre el sistema de archivos.
    char mbr_size_bytes[10];//Contiene el tamaño en bytes del sistema de archivos.
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


//--------------------------Prototipos del Programa------------------------

/*void menu();
void comandos();
void crear_disco();
int escribeInfoDirectorio(char *nombreFichero);
void eliminar_disco();
void crear_particion();
void tipo_de_tamanio();
char *ruta();*/

//-------------------------------------------------------------------------



//concatena la ruta del directorio+nombre+extension
void set_disco(char ruta_carpeta[52],char nombre[52]){
    sprintf(directorio, "%s%s%s",ruta_carpeta, nombre,extension_disco);

}

void menu()
{
   int opciones;
    do{
        printf("MENU PRINCIPAL\n\n");
        printf("1. Crear disco\n");
        printf("2. Eliminar Disco\n");
        printf("3. crear particion\n");
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
                crear_particion();
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

        // vos esque la aux de nosotros dijo que no pidiamos usar la libreria
        //dir.h para crear las carpetas,
        //librerira Dir.h

        //mkdir (ruta, 0777);    <=== esta seria la instruccion don la esta libreria


        system("mkdir /home/jonatan/hobbitelmashueco/");//<== esta linea que recibe una cadena
        //aqui que ando haciendo es enviarle a la termnal la instruccion que esta en la
        //cadena que decis vos y si funciona vos pues entonces
        // con esa la dejamos vaaa

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

void crear_particion()
{
     char ruta[52];
    char tamanio[52];
    char nom[52];

    printf("Ingrese el la ruta..\n");
    scanf("%s", &ruta);
    printf("ingrese el tamanio..\n");
    scanf("%s", &tamanio);
    printf("ingrese el nombre de la particion..\n");
    scanf("%s", &nom);

    if(escribeInfoDirectorio(ruta)==2)
    {
        FILE *f=fopen(ruta, "rb+");
        printf("\n");
        printf("**********************\n");
        printf("RUTA DE DISCO: %s \n",ruta);
        printf("**********************\n");

        //mostrando informacion del disco actual
        struct struc_disco tdisco;
        fread(&tdisco, sizeof(tdisco), 1, f);
        fclose(f);
        printf("tamaño: %s MB \n", tdisco.tamanio);
        printf("particiones: %s\n\n", tdisco.particion);

        if(atoi(tdisco.particion)==0)
            crear_particion_opciones(tdisco);
        else
            printf("\n!!!!!!!!!!!!!!!!!!!!!\nEl disco duro ya contiene 2 particiones primarias \nFormatear para crear nuevas\n!!!!!!!!!!!!!!!!!!!!!\n\n");
        //entrar_al_disco(unidad);
    }
    else
    {
        printf("El archivo con ruta %s no existe...!!",ruta);
    }

}

//creando particiones, mostrando opciones
void crear_particion_opciones(struct struct_disco tdisco)
{
    printf("\n");
    printf("**********************\n");
    printf("CREANDO PARTICION\n");
    printf("DISCO: %s \n",tdisco.nombre);
    printf("**********************\n\n");

}

void tipo_de_tamanio()
{
int opciones;
    do{


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




// inicio
int main()
{

    menu();
    //system("mkdir /home/jonatan/hobbitelmashueco/");
    return 0;
}
