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

typedef struct struc_disco{
    char nombre[52];
    char tamanio[9];
    char particion[9];
    char puntero[9]; //direccion al mbr
}info_disco;

#pragma pack(push, 1)

typedef struct partition//28
{
    char part_status; /*Indica si la particion esta activa o no*/
    char part_type; /*Indica el tipo de particion, primaria(P) o extendida (E)*/
    char part_fit; /*Tipo de ajuste de la particion { BF(best) FF(First) WF(worst)}*/
    int part_start; /*Indica en que byte del disco inicia la particion*/
    int part_size; /*Contiene el total de la particion en byte*/
    char part_name[16]; /*Nombre de la partcion*/
}struct_partition;



typedef struct mbr //136
{
    char nombre[52];
    int mbr_tamanio; /*Tamanio total del disco en byte*/
    timer_t mbr_fecha_creacion; /*fecha y hora de creacion del disco*/
    int mbr_disk_signature; /*Numero random, que identificara de forma unica a cada disco*/
    struct_partition mbr_partition_1; /*Estructura con informacion de la particion 1*/
    struct_partition mbr_partition_2; /*Estructura con informacion de la particion 2*/
    struct_partition mbr_partition_3; /*Estructura con informacion de la particion 3*/
    struct_partition mbr_partition_4; /*Estructura con informacion de la particion 4*/
}struct_mbr;

#pragma pack(pop)

struct struct_mbr1{
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

//-------------------------------------------------------------------------




//concatena la ruta del directorio+nombre+extension
void set_disco(char ruta_carpeta[52],char nombre[52]){
    sprintf(directorio, "%s%s",ruta_carpeta, nombre);

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
    //info_disco tdisco;
    struct_mbr infodisco;
    char ruta[52];
    int opt=0;
    printf("Ingresar el nombre del disco. \n");
    scanf("%s", infodisco.nombre);
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

    infodisco.mbr_tamanio = tamanio;
    infodisco.mbr_disk_signature=0;
    infodisco.mbr_partition_1.part_size=0;
    infodisco.mbr_partition_2.part_size=0;
    infodisco.mbr_partition_3.part_size=0;
    infodisco.mbr_partition_4.part_size=0;

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


        //mkdir (ruta, 0777);    <=== esta seria la instruccion don la esta libreria


        system("mkdir /home/jonatan/hobbitelmashueco/");

    }

    //creando el disco
    set_disco(ruta,infodisco.nombre);
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
    //fseek(f,0,SEEK_SET);
    fwrite(&infodisco,sizeof(infodisco),1,f);
    fclose(f);
    printf("Disco %s fue creado exitosamente ", infodisco.nombre);
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

    char ruta[52]="/home/jonatan/lol.dsk";
    char tamanio[52];
    char nom[52];

    /*
    printf("Ingrese el la ruta..\n");
    scanf("%s", &ruta);
    printf("ingrese el tamanio..\n");
    scanf("%s", &tamanio);
    printf("ingrese el nombre de la particion..\n");
    scanf("%s", &nom);
    */

    if(escribeInfoDirectorio(ruta)==2)
    {

        printf("\n");
        printf("**********************\n");
        printf("RUTA DE DISCO: %s \n",ruta);
        printf("**********************\n");

        //mostrando informacion del disco actual

        struct_mbr infodisco;
        FILE *f=fopen(ruta, "rb+");
        rewind(f);
        fread(&infodisco, sizeof(infodisco), 1, f);
        fclose(f);

        printf("tamaño: %d MB \n", infodisco.mbr_tamanio);
        printf("particiones: %d\n\n", 0);
        printf("Dato %d \n",infodisco.mbr_partition_1.part_size);
        printf("Dato %d \n",infodisco.mbr_partition_2.part_size);
        printf("Dato %d \n",infodisco.mbr_partition_3.part_size);
        printf("Dato %d \n",infodisco.mbr_partition_4.part_size);


        //if(infodisco.mbr_partition_1.part_size==0)
            crear_particion_opciones(infodisco,tamanio, ruta);
        //else
            //printf("\n!!!!!!!!!!!!!!!!!!!!!\nEl disco duro ya contiene 4 particiones primarias \nFormatear para crear nuevas\n!!!!!!!!!!!!!!!!!!!!!\n\n");
        //entrar_al_disco(unidad);
    }
    else
    {
        printf("El archivo con ruta %s no existe...!!",ruta);
    }

}

//creando particiones, mostrando opciones
void crear_particion_opciones(struct_mbr infodisco,char tamanio_partition[52], char ruta[52])
{
    printf("\n");
    printf("**********************\n");
    printf("CREANDO PARTICION\n");
    printf("DISCO: %s \n",infodisco.nombre);
    printf("**********************\n\n");

    printf("********antes*********** \n");
    timer_t t;
    t=time(NULL);
    FILE *f;
    f=fopen(ruta, "rb+"); //lee numeros binarios
    fseek(f,0, SEEK_SET);
    struct_mbr m1;
    fread(&m1, sizeof(m1), 1, f);
    printf("tamaño: %d MB \n", m1.mbr_tamanio);
    printf("particiones: %d\n\n", 0);
    printf("Dato %d \n",m1.mbr_partition_1.part_size);
    printf("Dato %d \n",m1.mbr_partition_2.part_size);
    printf("Dato %d \n",m1.mbr_partition_3.part_size);
    printf("Dato %d \n",m1.mbr_partition_4.part_size);
    fclose(f);



    f=fopen(ruta, "rb+"); //lee numeros binarios
    rewind(f);
    struct_mbr mbr_;
    mbr_.mbr_disk_signature=99;
    mbr_.mbr_tamanio=infodisco.mbr_tamanio;
    mbr_.mbr_fecha_creacion=t;
    mbr_.mbr_partition_1.part_size=111;
    mbr_.mbr_partition_2.part_size=222;
    mbr_.mbr_partition_3.part_size=333;
    mbr_.mbr_partition_4.part_size=666;
    fwrite(&mbr_,sizeof(mbr_),1,f);
    fclose(f);

    printf("**********Despues********* \n");
    f=fopen(ruta, "rb+"); //lee numeros binarios
    fseek(f,0, SEEK_SET);
    struct_mbr m;
    fread(&m, sizeof(m), 1, f);
    printf("tamaño: %d MB \n", m.mbr_tamanio);
    printf("particiones: %d\n\n", 0);
    printf("Dato %d \n",m.mbr_partition_1.part_size);
    printf("Dato %d \n",m.mbr_partition_2.part_size);
    printf("Dato %d \n",m.mbr_partition_3.part_size);
    printf("Dato %d \n",m.mbr_partition_4.part_size);
    fclose(f);
    int a;


}

/*
struct numero
{
    int num1;
    int num2;
    int num3;
};

struct caracteres
{
    char cadena[12];
    char cadena2[12];
    char cadena3[12];
};

struct otro
{
    char cadena[12];
};
*/



// inicio
int main()
{

    /*struct numero num; 12 byte
    struct caracteres carar; 36 byte
    struct otro o; 12 byte
    struct struc_disco tdisco;


    FILE *f = fopen ("/home/jonatan/disco.dsk", "w+b");


    for(ifor=0;ifor<10;ifor++)
        fwrite (&num, sizeof(num), 1, f);
        120+360+120=600bytes


    for(ifor=0;ifor<10;ifor++)
        fwrite (&carar, sizeof(carar), 1, f);

    for(ifor=0;ifor<10;ifor++)
        fwrite (&o, sizeof(o), 1, f);

    //guardando informacion del disco
    rewind(f);
    fwrite(&tdisco,sizeof(tdisco),1,f);
    fclose(f);

    STRUC1 12
    STRUC2 24
   ==> STRUC3 36

    f = fopen ("/home/jonatan/disco.dsk", "rb+");
    fseek(f,3*sizeof(num), SEEK_SET);
    //sprintf(num,num,"jose")
    num.num1=1;
    num.num2=2;
    num.num3=3;
    fwrite(&num,sizeof(num),1,f);
    fclose(f);

    f = fopen ("/home/jonatan/disco.dsk", "rb+");
    fseek(f,3*sizeof(num), SEEK_SET);
    struct numero n;
    fread(&n, sizeof(n), 1,f);
    printf("mi dato %d %d %d\n", n.num1, n.num2, n.num3);
    fclose(f);*/



    /*time_t t;
      struct tm *tm;
      char fechayhora[100];

      t=time(NULL);
      tm=localtime(&t);

      strftime(fechayhora, 100, "%d/%m/%Y %H:%M:%S", tm);
      printf ("Hoy es: %s\n", fechayhora);


    struct_partition p;
    p.part_fit='p';
    sprintf(p.part_name,"nombre");
    p.part_size=100;
    p.part_start=1005;
    p.part_status='l';
    p.part_type='l';

    FILE *ff=fopen("/home/jonatan/entrada.txt", "w+b");

    fwrite(&p,sizeof(p),1,ff);
    fclose(ff);

    struct_mbr m;
    m.mbr_disk_signature=12;
    m.mbr_tamanio=125;
    m.mbr_fecha_creacion=t;
    m.mbr_partition_1=p;
    m.mbr_partition_3=p;
    m.mbr_partition_3=p;
    m.mbr_partition_3=p;



    printf("mbr %d\n", sizeof(m));
    printf("partition %d\n", sizeof(p));
    printf("timer %d\n", sizeof(t));
    printf("tdisco %d\n", sizeof(info_disco));*/

    menu();
    //system("mkdir /home/jonatan/hobbitelmashueco/");
    return 0;
}
