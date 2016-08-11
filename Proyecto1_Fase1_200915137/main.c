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

//------------------------Area de librerias-----------------------------

#include <stdio.h>
//atoi
#include <stdlib.h>
//directorios
#include <dirent.h>
//crear directorios y validarlos
#include <sys/stat.h>
//comparacion de cadenas
#include <string.h>
//time
#include <time.h>

//-----------------------------------------------------------------------


//------------------------Area de estructuras----------------------------

#pragma pack(push, 1)

//tamanio 27 bytes
typedef struct partition
{
    char part_status; /*Indica si la particion esta activa o no*/
    char part_type; /*Indica el tipo de particion, primaria(P) o extendida (E)*/
    char part_fit; /*Tipo de ajuste de la particion { BF(best) FF(First) WF(worst)}*/
    int part_start; /*Indica en que byte del disco inicia la particion*/
    int part_size; /*Contiene el total de la particion en byte*/
    char part_name[16]; /*Nombre de la partcion*/
}struct_partition;


//Tamaio 148 bytes
typedef struct mbr //
{
    char nombre[24]; /*nombre del disco*/
    int mbr_tamanio; /*Tamanio total del disco en byte*/
    timer_t mbr_fecha_creacion; /*fecha y hora de creacion del disco*/
    int mbr_disk_signature; /*Numero random, que identificara de forma unica a cada disco*/
    struct_partition mbr_partition_1; /*Estructura con informacion de la particion 1*/
    struct_partition mbr_partition_2; /*Estructura con informacion de la particion 2*/
    struct_partition mbr_partition_3; /*Estructura con informacion de la particion 3*/
    struct_partition mbr_partition_4; /*Estructura con informacion de la particion 4*/
}struct_mbr;


//Tamanio 30 bytes
typedef struct ebr{
    char ebr_part_status; /*Indica si la particion esta activa o no.*/
    char ebr_part_fit[2]; /*Tipo de ajuste de la particion. Tendra los valores BF, FF, WF*/
    int ebr_part_star; /*Indica en que byte inicia la particion*/
    int ebr_part_size; /*Contiene el tamano total de la particion en byte*/
    int ebr_part_next; /*Byte en el que esta el proximo EBR. -1 si no hay siguiente*/
    char ebr_part_name[16]; /*Nombre de la particion*/
}struct_ebr;

#pragma pack(pop)

//-----------------------------------------------------------------------


//--------------------Area de variables globales-------------------------

//bit utilizado para guardar 0's
char buffer[1];
//contador de fors
int ifor=0;
//direccion del disco actual
char directorio[58];
//Numero de particiones primarias
int numero_particiones_primarias;
//Numero de particiones logicas
int numero_particiones_logicas;
//Esta variable es a la que se le va asignar cada comando que se va analizar
//char comando_entrada[100];
//Sale del ciclo para terminar la ejecucion
int output=0;


int bandera_size=0;
int bandera_unit=0;
int bandera_path=0;
int bandera_name=0;

//-----------------------------------------------------------------------


//concatena la ruta del directorio+nombre

void set_disco(char ruta_carpeta[52],char nombre[52])
{
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
                ver();
            break;
            case 5:
            break;
            default:
            printf("Error opcion incorrecta\n\n");
            break;
        }
    }while(opciones != 5);

}

//metodo par crear discos
void crear_disco(){

    struct_mbr infodisco;
    char ruta[52];
    int opt=0;
    //datos que se se ingresa
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

    //asignar datos en la variable infodisco de tipo mbr
    infodisco.mbr_tamanio = tamanio;
    infodisco.mbr_disk_signature=0;
    infodisco.mbr_partition_1.part_size=0;
    infodisco.mbr_partition_2.part_size=0;
    infodisco.mbr_partition_3.part_size=0;
    infodisco.mbr_partition_4.part_size=0;

    printf("Creando disco...\n");

    //Si el numero que retorna es igual a 1 indica que es un directorio
    if(escribeInfoDirectorio(ruta)==1)
    {
        printf("se encontro el directorio %s\n", ruta);
    }

    //Si el numero que retorna es igual a 1 indica que es un fichero
    if(escribeInfoDirectorio(ruta)==0)
    {
        /*
         * Se crea un directorio DIRECTORIO con permisos 777 y se comprueba.
         * 4+2+1=7 es decir puedo leer,escribir y ejecutar
         */


        //mkdir (ruta, 0777);    <=== esta seria la instruccion don la esta libreria


        system("mkdir /home/jonatan/hobbitelmashueco/");

    }

    //Metodo que concatena dos vectores y lo asigna a una variable global
    set_disco(ruta,infodisco.nombre);

    FILE *f = fopen (directorio, "w+b");

    if(opt==1)
    {
        //tamanio en bytes
        for(ifor=0;ifor<tamanio;ifor++)
            fwrite (buffer, sizeof(buffer), 1024, f);
    }

    if(opt==2 || opt==3)
    {
        //tamanio en megabytes
        for(ifor=0;ifor<tamanio*1024;ifor++)
            fwrite (buffer, sizeof(buffer), 1024, f);
    }


    //guardando informacion del disco
    //rewind indica el inicio de un archivo en este caso es un disco
    rewind(f);
    //otra manera de indicar el inicio de un archivo
    //fseek(f,0,SEEK_SET);
    //escribe un bloque de tipo struct_mbr de 148 bytes
    fwrite(&infodisco,sizeof(infodisco),1,f);
    fclose(f);
    printf("Disco %s fue creado exitosamente ", infodisco.nombre);
    printf("en %s\n\n", directorio);
}

//Metodo para validar una ruta o fichero
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
        return 0;

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
        //esta manera de eliminar es usando el la libreria Dir.h
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



    printf("ingrese el tamanio..\n");
    scanf("%s", &tamanio);
    /*printf("Ingrese el la ruta..\n");
    scanf("%s", &ruta);
    printf("ingrese el nombre de la particion..\n");
    scanf("%s", &nom);
    */

    if(escribeInfoDirectorio(ruta)==2)
    {

        printf("\n");
        printf("**********************\n");
        printf("RUTA DE DISCO: %s \n",ruta);
        printf("**********************\n");



        struct_mbr infodisco;
        FILE *f=fopen(ruta, "rb+");
        //inicio del archivo
        rewind(f);
        //leer el primer bloque del archivo
        fread(&infodisco, sizeof(infodisco), 1, f);
        fclose(f);

        //mostrar informacion del bloque de tipo struct_mbr
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
    //datos que ya esta escritos en el disco
    printf("tamaño: %d MB \n", m1.mbr_tamanio);
    printf("particiones: %d\n\n", 0);
    printf("Dato %d \n",m1.mbr_partition_1.part_size);
    printf("Dato %d \n",m1.mbr_partition_2.part_size);
    printf("Dato %d \n",m1.mbr_partition_3.part_size);
    printf("Dato %d \n",m1.mbr_partition_4.part_size);
    fclose(f);


}

void ver()
{

    FILE *f;
    f=fopen("/home/jonatan/lol.dsk","rb+");
    struct_mbr l;
    rewind(f);
    fread(&l,sizeof(l),1, f);
    /*tamanio_disco=l.mbr_tamanio*1024*1024;//tamanio en bytes
    printf("\ntamanio disco %d bytes\n", tamanio_disco);
    printf("nombre de disco %s\n", l.nombre);
    printf("tamanio particion %d\n", l.mbr_partition_1.part_size);
    printf("nombre particion %s\n", l.mbr_partition_1.part_name);
    printf("inicio %d\n", l.mbr_partition_1.part_start);
    printf("tipo %c\n", l.mbr_partition_1.part_type);*/
    fclose(f);

    //tamanio_disco_libre=tamanio_disco-sizeof(l);

    //tamanio de particion 1
    f=fopen("/home/jonatan/lol.dsk","rb+");
    struct_mbr tem;
    fseek(f,0, SEEK_SET);
    if(l.mbr_partition_1.part_size==0){

        numero_particiones_primarias+=1;
        tem.mbr_tamanio=l.mbr_tamanio;
        tem.mbr_fecha_creacion=l.mbr_fecha_creacion;
        sprintf(tem.nombre,l.nombre);
        sprintf(tem.mbr_partition_1.part_fit,"WF");
        sprintf(tem.mbr_partition_1.part_name,"particion 1");
        tem.mbr_partition_1.part_size=12*1024*1024;
        tem.mbr_partition_1.part_start=sizeof(tem);
        tem.mbr_partition_1.part_status="f"; //v de verdadero
        tem.mbr_partition_1.part_type='p';
        fwrite(&tem, sizeof(tem),1,f );
    }
    fclose(f);


    f=fopen("/home/jonatan/lol.dsk","rb+");
    rewind(f);
    fread(&l,0,1, f);
    /*tamanio_disco=l.mbr_tamanio*1024*1024;//tamanio en bytes
    printf("\ntamanio disco %d bytes\n", tamanio_disco);
    printf("nombre de disco %s\n", l.nombre);
    printf("tamanio particion %d\n", l.mbr_partition_1.part_size);
    printf("nombre particion %s\n", l.mbr_partition_1.part_name);
    printf("inicio %d\n", l.mbr_partition_1.part_start);
    printf("tipo %c\n", l.mbr_partition_1.part_type);*/
    fclose(f);

    //tamanio de particion 2
    f=fopen("/home/jonatan/lol.dsk","rb+");
    fseek(f,0, SEEK_SET);
    if(l.mbr_partition_2.part_size==0){
        numero_particiones_primarias+=1;
        tem.mbr_tamanio=l.mbr_tamanio;
        tem.mbr_fecha_creacion=l.mbr_fecha_creacion;
        sprintf(tem.nombre,l.nombre);
        tem.mbr_partition_1=l.mbr_partition_1;
        tem.mbr_partition_2.part_fit='l';
        sprintf(tem.mbr_partition_2.part_name,"particion 2");
        tem.mbr_partition_2.part_size=12*1024*1024;
        tem.mbr_partition_2.part_start=sizeof(tem)+12*1024*1024;
        tem.mbr_partition_2.part_status="f"; //v si esta activo f si esta desactivo
        tem.mbr_partition_2.part_type='p';
        fwrite(&tem, sizeof(tem),1,f );
    }

    fclose(f);




    f=fopen("/home/jonatan/lol.dsk","rb+");
    rewind(f);
    fread(&l,0,1, f);
    struct tm *tm;
    char fecha[50];
    char hora[50];
    l.mbr_fecha_creacion=time(NULL);
    tm=localtime(&(l.mbr_fecha_creacion));
    //strftime(fecha, 50, "%d/%m/%Y", tm);
    //strftime(hora, 50, "%H:%M:%S", tm);
    printf("\n\n");
    printf("\nTAMANIO DISCO: %d bytes\n", l.mbr_tamanio);
    printf("NOMBRE DISCO %s\n", l.nombre);
    printf("\n\n");
    printf("TAMANIO PARTICION_1 %d byte\n", l.mbr_partition_1.part_size);
    printf("NOMBRE PARTICION_1 %s\n", l.mbr_partition_1.part_name);
    printf("INICIO PARTICION_1 %d\n", l.mbr_partition_1.part_start);
    printf("TIPO PARTICION_1 %c\n", l.mbr_partition_1.part_type);
    printf("STATUS PARTICION_1 %c\n", l.mbr_partition_1.part_status);
    printf("AJUSTE PARTICION_1 %c\n", l.mbr_partition_1.part_fit);
    printf("\n\n");
    printf("TAMANIO PARTICION_2 %d byte\n", l.mbr_partition_2.part_size);
    printf("NOMBRE PARTICION_2 %s\n", l.mbr_partition_2.part_name);
    printf("INICIO PARTICION_2 %d\n", l.mbr_partition_2.part_start);
    printf("TIPO PARTICION_2 %c\n", l.mbr_partition_2.part_type);
    printf("STATUS PARTICION_2 %c\n", l.mbr_partition_2.part_status);
    printf("AJUSTE PARTICION_2 %c\n", l.mbr_partition_2.part_fit);
    //printf ("Fecha creacion: %s\n", fecha);
    //printf ("Hora de creacion: %s\n", hora);
    fclose(f);

}


void montar_disco()
{

}


void desmontar_disco()
{

}


void validar(char comando_entrada[])
{

    int columna=0;
    int estado_actual=0;
    char size[100]=" ";
    char unit=' ';
    char path [100];
    char name[100];
    char bandera=1;
    //Esta es una bandera para que termine de validad los comandos
    char bandera_fin=1;


    int t=strlen(comando_entrada);

    while(bandera_fin != 0)
    {
        switch (estado_actual) {
        //recorrer el vector para comparar cada posicion
        case 0:
            //validando la palabra reservada para pasar a otro estado
            if(comando_entrada[columna]=='m' || comando_entrada[columna]=='M' &&
                    comando_entrada[columna+1]=='k' || comando_entrada[columna+1]=='K' &&
                    comando_entrada[columna+2]=='d' || comando_entrada[columna+2]=='D' &&
                    comando_entrada[columna+3]=='i' || comando_entrada[columna+3]=='I' &&
                    comando_entrada[columna+4]=='s' || comando_entrada[columna+4]=='S' &&
                    comando_entrada[columna+5]=='k' || comando_entrada[columna+5]=='K' )
            {

                memset( size, '\0', strlen(size) );
                memset( path, '\0', strlen(path) );
                memset( name, '\0', strlen(name) );
                unit='\0';

                estado_actual=1;
                columna = columna + 6;
                bandera=1;

            }

            else if(comando_entrada[columna]=='c' || comando_entrada[columna]=='C' &&
                    comando_entrada[columna+1]=='c' || comando_entrada[columna+1]=='C')
            {

                columna +=2;
                columna +=1;
                output = 1;
                bandera_fin=1;
            }

            break;

        case 1:

            if(comando_entrada[columna]==' ' || comando_entrada[columna]=='\\' ||comando_entrada[columna]=='\n')
            {
                //cuando encuentra espacio, barra inclinada o un salto de linea se queda en el mismo estado
                estado_actual = 1;
                columna++;

            }
            else if(comando_entrada[columna] == '-' &&
                        comando_entrada[columna+1]=='s' || comando_entrada[columna+1]=='S' &&
                        comando_entrada[columna+2]=='i' || comando_entrada[columna+2]=='I' &&
                        comando_entrada[columna+3]=='z' || comando_entrada[columna+3]=='Z' &&
                        comando_entrada[columna+4]=='e' || comando_entrada[columna+4]=='E' &&
                        comando_entrada[columna+5]==':' &&
                        comando_entrada[columna+6]==':')
            {

                estado_actual =1;
                columna = columna + 7;

                //reconocer el valor de size
                int i = 0;
                while(comando_entrada[columna] !=' ')
                {
                    if(comando_entrada[columna]=='0' || comando_entrada[columna]=='1'||comando_entrada[columna]=='2' || comando_entrada[columna]=='3' || comando_entrada[columna]=='4'|| comando_entrada[columna]=='5' | comando_entrada[columna]=='6' || comando_entrada[columna]=='7' || comando_entrada[columna]=='8' || comando_entrada[columna]=='9')
                    {
                        size[i] = comando_entrada[columna];
                        i++;
                        columna++;
                    }
                    if(comando_entrada[columna] =='\n'){
                       break;
                    }

                }


            }

            else if (comando_entrada[columna] == '+' &&
                     comando_entrada[columna+1]=='u' || comando_entrada[columna+1]=='U' &&
                     comando_entrada[columna+2]=='n' || comando_entrada[columna+2]=='N' &&
                     comando_entrada[columna+3]=='i' || comando_entrada[columna+3]=='N' &&
                     comando_entrada[columna+4]=='t' || comando_entrada[columna+4]=='T' &&
                     comando_entrada[columna+5]==':' &&
                     comando_entrada[columna+6]==':')
            {
                estado_actual=1;
                columna=columna+7;

                //reconoce el valor de unit

                while (comando_entrada[columna] != ' ')
                {

                    if (comando_entrada[columna]== 'K' || comando_entrada[columna]== 'M' || comando_entrada[columna] == 'k' || comando_entrada[columna] == 'm') {
                        unit = comando_entrada[columna];
                        columna++;
                    }
                    else if(comando_entrada[columna]=='\n')
                    {
                        break;
                    }
                    else
                    {
                        printf("Comando invalido, eliga M (Megabytes ) o  k(Kilobytes).\n");
                        unit= '\0';

                    }



                }

            }

            else if(comando_entrada[columna] == '-' &&
                                comando_entrada[columna+1]=='n' || comando_entrada[columna+1]=='N' &&
                                comando_entrada[columna+2]=='a' || comando_entrada[columna+2]=='A' &&
                                comando_entrada[columna+3]=='m' || comando_entrada[columna+3]=='M' &&
                                comando_entrada[columna+4]=='e' || comando_entrada[columna+4]=='E' &&
                                comando_entrada[columna+5]==':' &&
                                comando_entrada[columna+6]==':')
                        {
                            estado_actual = 1;
                            columna=columna + 7;

                            if (comando_entrada[columna] == '"') {

                                columna++;
                                int i = 0;

                                while (comando_entrada[columna]!= '-') {

                                    if (comando_entrada[columna] == '"') {
                                        columna++;

                                        if (name[i - 4] == '.' &&
                                                (name[i- 3] == 'D' || name[i- 3] == 'd') &&
                                                (name[i - 2] == 'S' || name[i - 2] == 's') &&
                                                (name[i - 1] == 'K' || name[i - 1] == 'k')) {
                                            break;

                                        } else {
                                            printf("Comando invalido debe de escribir el nombre del disco con extension .dsk.\n");
                                            name[0] = '\0';
                                            break;
                                        }

                                    }

                                    name[i] = comando_entrada[columna];
                                    i++;
                                    columna++;
                                }

                            } else {
                                //si no viene una comilla en el principio del nombre disco
                                estado_actual = 100; //Estado de error
                            }


                        }

            else if(comando_entrada[columna] == '-' &&
                                comando_entrada[columna+1]=='p' || comando_entrada[columna+1]=='P' &&
                                comando_entrada[columna+2]=='a' || comando_entrada[columna+2]=='A' &&
                                comando_entrada[columna+3]=='t' || comando_entrada[columna+3]=='T' &&
                                comando_entrada[columna+4]=='h' || comando_entrada[columna+4]=='H' &&
                                comando_entrada[columna+5]==':' &&
                                comando_entrada[columna+6]==':')
                        {
                            estado_actual=1;
                            columna=columna+7;
                            if (comando_entrada[columna] == '"') {
                                columna++;
                                int contador = 0;
                                while (comando_entrada[columna] != '-') {
                                    if (comando_entrada[columna] == '"') {
                                        columna++;
                                        break;
                                    }
                                    path[contador] = comando_entrada[columna];
                                    contador++;
                                    columna++;
                                }
                            } else {
                                estado_actual = 100; //Estado de error
                            }

                        }


            else
            {
                if (size[0] == '\0') {
                    printf("Comando invalido debe de escribir el tamaño del disco.\n");
                    bandera_fin = 0;
                }
                if (unit == '\0') {
                    printf("No se especifico la unidad de tamaño, el disco se creara en Megabytes.\n");
                    unit = 'M';
                }
                if (unit == '\0') {
                    bandera_fin=0;
                }
                if (path[0] == '\0') {
                    printf("Comando invalido debe de escribir el path de almacenamiento del disco.\n");
                    bandera_fin= 0;
                }

                if (name[0] == '\0') {
                    printf("Comando invalido debe de escribir el nombre del disco.\n");
                    bandera_fin = 0;
                }
                if (bandera == 1) {
                    printf("size: %s\n", size);
                    printf("MB o B 2: %c\n", unit);
                    printf("path: %s\n", path);
                    printf("name: %s\n", name);
                    bandera_fin=0;
                }





            }

            break;

        case 2:


            break;

        case 3:

            break;

        case 4:

            break;

        case 5:

            break;

        case 6:

            break;

        case 7:

            break;

        case 8:

            break;

        case 100:

            break;

        default:
            printf("Nose encontro el comando de entrada...!!");
            break;
        }

    }//fin del while que recorre todos los caracteres
}


//Metodo limpiar vectores
void vaciar_vector(char vector[]) {
    int i = 0;
    for (i = 0; i < strlen(vector); i++) {
        vector[i] = ' ';
    }
}

/*int esnumero(char num)
{

    if(num=='0' || num=='1'||num=='2' || num=='3' || num=='4'|| num=='5' | num=='6' || num=='7' || num=='8' || num=='9')
    {
        return 1;
    }
    return 0;
}*/


// inicio
int main()
{


/*struct numero num;
struct caracteres carar;
struct otro o;

    FILE *f = fopen ("/home/jonatan/disco.dsk", "w+b");

    rewind(f);
    //for(ifor=0;ifor<10;ifor++)
        fwrite (&num, sizeof(num), 1, f);
        //120+360+120=600bytes


    //for(ifor=0;ifor<10;ifor++)
        //fwrite (&carar, sizeof(carar), 1, f);

    //for(ifor=0;ifor<10;ifor++)
        fwrite (&o, sizeof(o), 1, f);

    //guardando informacion del disco

    //fwrite(&tdisco,sizeof(tdisco),1,f);

    fclose(f);



    f = fopen ("/home/jonatan/disco.dsk", "w+b");
    fseek(f,sizeof(num), SEEK_SET);
    //sprintf(num,num,"jose")
    printf("ingrese una frase\n");
    char nombre[12];
    scanf("%s",&nombre);
    sprintf(o.cadena,nombre);
    fwrite(&o,sizeof(o),1,f);
    fclose(f);

    f = fopen ("/home/jonatan/disco.dsk", "rb+");
    fseek(f,sizeof(num), SEEK_SET);
    struct otro n;
    fread(&n, sizeof(n), 1,f);
    printf("mi dato %s\n", n.cadena);
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

    //menu();
    //system("mkdir /home/jonatan/hobbitelmashueco/");


    char comando_entrada[100];

    while(output !=1)
    {
    printf("  ****************************************************************************\n");
    printf("  ********************* MANEJO E IMPLEMENTACION DE ARCHIVO *******************\n");
    printf("  ****************************************************************************\n");
    printf("  ********************Para salir ingrese el comando cc ***********************\n");
    printf("  ****************************************************************************\n\n");
        printf("::>");
        fgets(comando_entrada, 100, stdin);
        //scanf("%s", &comando_entrada);
        strcat(comando_entrada, " ");
        validar(comando_entrada);

    }

    printf("\n\nSaliendo del sistema.....!!\n");


    return 0;
}
