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

//tamanio 28 bytes
typedef struct partition
{
    char part_status; /*Indica si la particion esta activa o no*/
    char part_type; /*Indica el tipo de particion, primaria(P) o extendida (E)*/
    char part_fit[2]; /*Tipo de ajuste de la particion { BF(best) FF(First) WF(worst)}*/
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

//banderas para cada inicio de comando;
int bandera_mkdisk=0;
int bandera_rmdisk=0;
int bandera_fdisk=0;
int bandera_mount=0;
int bandera_umount=0;
int bandera_name=0;
int bandera_path=0;
int bandera_unit=0;
int bandera_exec=0;
int bandera_comentario=0;
int contador_asignature=1;


//-----------------------------------------------------------------------


//concatena la ruta del directorio+nombre

void set_disco(char ruta_carpeta[52],char nombre[52])
{
    sprintf(directorio, "%s%s",ruta_carpeta, nombre);

}



void archivo_masivo(char ruta[]) {


    char caracter=' ';
    char comando_ejecutar[200] = " ";
    int bandera = 0;
    int contador = 0;
    vaciar_vector(comando_ejecutar);
    FILE * f;
    f = fopen(ruta, "r");
    if (f == NULL)
        printf("\nError al abrir el archivo... \n\n");
    else
    {
        while (feof(f) == 0) {

            caracter = fgetc(f); //se lee caracter por caracter el archivo

            if(caracter!='\n')
            {

                comando_ejecutar[contador] = caracter; //se agrega a un arreglo de chars
                contador++;

            }
            else
            {
                if (caracter == '\n') {
                    int temporal = 0;
                    while (comando_ejecutar[temporal] != ' ') {
                        if (comando_ejecutar[temporal] == '\\') {
                            contador = temporal - 1;
                            bandera= 1;
                        }
                        temporal++;
                    }
                    if (bandera == 1) {
                        bandera = 0;
                    } else {
                        if (comando_ejecutar[0] != '\n') {
                            validar(comando_ejecutar);
                            vaciar_vector(comando_ejecutar);
                            contador = 0;
                            bandera = 0;
                        }
                    }
                }

            }

        }
    }

    fclose(f);
}


int es_fichero(char  *ruta)
{
    //recoge la informacion
    struct stat datos;

    //verificar si no viene vacio
    if(ruta== NULL)
        return 0;

    if (lstat (ruta, &datos) == -1)
    {
        //printf ("No existe \n");
        return 0;
    }
    else
    {
        /* Se comprueba si es un directorio */
        if (S_ISREG(datos.st_mode)){
            return 1;
        }
        /* Es otra cosa */
        else
            return 0;

    }
}

int es_directorio(char  *directorio)
{
    //recoge la informacion
    struct stat datos;

    //verificar si no viene vacio
    if(directorio == NULL)
        return 0;

    if (lstat (directorio, &datos) == -1)
    {
        //printf ("no existe\n");
        return 0;
    }
    else
    {
        /* Se comprueba si es un directorio */
        if (S_ISDIR(datos.st_mode)){
            //printf ("es un directorio\n");
            return 1;
        }
        /* Es otra cosa */
        else
            return 0;

    }
}

void eliminar_disco(char ruta[100])
{

    if(es_fichero(ruta))
    {
        //esta manera de eliminar es usando el la libreria Dir.h
        //unlink (ruta);
        char path[100]="rm ";
        strcat(path,ruta);
        system(path);
    }
}

void crear_disco_nuevo(char size[], char name[], char unit, char path[])
{
    struct_mbr infodisco;

    //asignar datos en la variable infodisco de tipo mbr
    sprintf(infodisco.nombre,name);
    if(unit=='K'){
        infodisco.mbr_tamanio = (int)atoi(size)*1024;
    }
    else
    {
        infodisco.mbr_tamanio = (int)atoi(size)*1024*1024;
    }

    infodisco.mbr_disk_signature=contador_asignature;
    infodisco.mbr_partition_1.part_size=0;
    infodisco.mbr_partition_2.part_size=0;
    infodisco.mbr_partition_3.part_size=0;
    infodisco.mbr_partition_4.part_size=0;
    contador_asignature+=2;

    printf("Creando disco...\n");

    //Si el numero que retorna es igual a 1 indica que es un directorio
    if(es_directorio(path))
    {
        //Metodo que concatena dos vectores y lo asigna a una variable global
        set_disco(path,infodisco.nombre);
        printf("El directorio existe %s ....\n", path);
    }
    else
    {
        char directorio_nuevo[100];
        sprintf(directorio_nuevo,"%s%s","mkdir ",path);
        system(directorio_nuevo);
        //mkdir(directorio_nuevo,777);
        printf("Se creo %s en %s..",name, path);

    }



    //Metodo que concatena dos vectores y lo asigna a una variable global
    set_disco(path,infodisco.nombre);


    FILE *f = fopen (directorio, "w+b");

    int tamanio=atoi(size);

    //tamanio en bytes
    if(unit=='k' || unit=='K')
    {
        int x;
        for(x=0;x<tamanio;x++)
            fwrite (buffer, sizeof(buffer), 1024, f);
    }
    else
    {
        //tamanio en megabytes
        int x;
        for(x=0;x<tamanio*1024;x++)
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

int crear_particion_(char size[], char unit, char path[], char name[], char fit[], char type)
{
    if(es_fichero(path))
    {
        struct_mbr tmp;
        FILE * f = fopen(path, "rb+");

        rewind(f);
        fread(&tmp, sizeof (tmp), 1, f);
        fclose(f);

        struct_partition nueva;

        nueva.part_status = 'f';
        nueva.part_type = type;
        sprintf(nueva.part_fit,fit);
        //nueva.part_start = (int) sizeof (tmp);
        //int a;


        if(unit=='B')
        {
              nueva.part_size = (int)atoi(size);
        }
        else if(unit=='K')
        {
              nueva.part_size=(int)atof(size)*1024;
        }
         else
        {
            nueva.part_size=(int)atof(size)*1024*1024;

        }


        int size_;
        if (tmp.mbr_partition_1.part_size == 0)
        {


            tmp.mbr_partition_1.part_status = nueva.part_status;
            tmp.mbr_partition_1.part_type = nueva.part_type;
            sprintf(tmp.mbr_partition_1.part_fit,nueva.part_fit);
            tmp.mbr_partition_1.part_start = (int)sizeof(tmp);
            tmp.mbr_partition_1.part_size = nueva.part_size;
            sprintf(tmp.mbr_partition_1.part_name, name);

            f = fopen(path, "rb+");
            rewind(f);
            fwrite(&tmp, sizeof (tmp), 1, f);
            fclose(f);

        }

        else if (tmp.mbr_partition_2.part_size == 0)
        {
            int tam=tmp.mbr_partition_1.part_size;
            size_=sizeof(tmp)+tam;
            tmp.mbr_partition_2.part_status = nueva.part_status;
            tmp.mbr_partition_2.part_type = nueva.part_type;
            sprintf(tmp.mbr_partition_2.part_fit,nueva.part_fit);
            tmp.mbr_partition_2.part_start = size_;
            tmp.mbr_partition_2.part_size = nueva.part_size;
            sprintf(tmp.mbr_partition_2.part_name, name);

            f = fopen(path, "rb+");
            rewind(f);
            fwrite(&tmp, sizeof (tmp), 1, f);
            fclose(f);

        }
        else if (tmp.mbr_partition_3.part_size == 0) {

            int tam=tmp.mbr_partition_2.part_size;
            size_=sizeof(tmp)+tam;
            tmp.mbr_partition_3.part_status = nueva.part_status;
            tmp.mbr_partition_3.part_type = nueva.part_type;
            sprintf(tmp.mbr_partition_3.part_fit,nueva.part_fit);
            tmp.mbr_partition_3.part_start = size_;
            tmp.mbr_partition_3.part_size = nueva.part_size;
            sprintf(tmp.mbr_partition_3.part_name, name);

            f = fopen(path, "rb+");
            rewind(f);
            fwrite(&tmp, sizeof (tmp), 1, f);
            fclose(f);

        }
        else
        {
            int tam=tmp.mbr_partition_3.part_size;
            size_=sizeof(tmp)+tam;
            tmp.mbr_partition_4.part_status = nueva.part_status;
            tmp.mbr_partition_4.part_type = nueva.part_type;
            sprintf(tmp.mbr_partition_4.part_fit,nueva.part_fit);
            tmp.mbr_partition_4.part_start = size_;
            tmp.mbr_partition_4.part_size = nueva.part_size;
            sprintf(tmp.mbr_partition_4.part_name, name);

            f = fopen(path, "rb+");
            rewind(f);
            fwrite(&tmp, sizeof (tmp), 1, f);
            fclose(f);

        }


        struct_mbr dat_mbr;
        f = fopen(path, "rb+");
        rewind(f);
        fread(&dat_mbr, (int)sizeof (dat_mbr), 1, f);
        fclose(f);

        timer_t t;
        t=time(NULL);
        struct tm *tm;
        char fechayhora[100];
        tm=localtime(&t);
        strftime(fechayhora, 100, "%d/%m/%Y %H:%M:%S", tm);

        printf("\n************************************\n");
        printf("DATOS DISCO\n");
        printf("TAMANIO: %d BYTES\n", dat_mbr.mbr_tamanio);
        printf("NOMBRE : %s\n", dat_mbr.nombre);
        printf("FECHA Y HORA: %s\n",fechayhora);
        printf("No. ASIGNATURE: %d\n", dat_mbr.mbr_disk_signature);
        printf("************************************");
        if(dat_mbr.mbr_partition_1.part_size>0){
            printf("\nDATOS PARTICION No. 1\n");
            printf("************************************\n");
            printf("\STATUS: %c\n", dat_mbr.mbr_partition_1.part_status);
            printf("\TYPE: %c\n", dat_mbr.mbr_partition_1.part_type);
            printf("\FIT: %s\n", dat_mbr.mbr_partition_1.part_fit);
            printf("\START: %d\n", dat_mbr.mbr_partition_1.part_start);
            printf("\SIZE: %d BYTES\n", dat_mbr.mbr_partition_1.part_size);
            printf("\NAME: %s\n", dat_mbr.mbr_partition_1.part_name);
        }
        if(dat_mbr.mbr_partition_2.part_size>0){
            printf("\nDATOS PARTICION No. 2\n");
            printf("************************************\n");
            printf("\STATUS: %c\n", dat_mbr.mbr_partition_2.part_status);
            printf("\TYPE: %c\n", dat_mbr.mbr_partition_2.part_type);
            printf("\FIT: %s\n", dat_mbr.mbr_partition_2.part_fit);
            printf("\START: %d\n", dat_mbr.mbr_partition_2.part_start);
            printf("\SIZE: %d BYTES\n", dat_mbr.mbr_partition_2.part_size);
            printf("\NAME: %s\n", dat_mbr.mbr_partition_2.part_name);
        }
        if(dat_mbr.mbr_partition_3.part_size>0){
            printf("\nDATOS PARTICION No. 3\n");
            printf("************************************\n");
            printf("\STATUS: %c\n", dat_mbr.mbr_partition_3.part_status);
            printf("\TYPE: %c\n", dat_mbr.mbr_partition_3.part_type);
            printf("\FIT: %s\n", dat_mbr.mbr_partition_3.part_fit);
            printf("\START: %d\n", dat_mbr.mbr_partition_3.part_start);
            printf("\SIZE: %d BYTES\n", dat_mbr.mbr_partition_3.part_size);
            printf("\NAME: %s\n", dat_mbr.mbr_partition_3.part_name);
        }
        if(dat_mbr.mbr_partition_4.part_size>0){
            printf("\nDATOS PARTICION No. 4\n");
            printf("************************************\n");
            printf("\STATUS: %c\n", dat_mbr.mbr_partition_4.part_status);
            printf("\TYPE: %c\n", dat_mbr.mbr_partition_4.part_type);
            printf("\FIT: %s\n", dat_mbr.mbr_partition_4.part_fit);
            printf("\START: %d\n", dat_mbr.mbr_partition_4.part_start);
            printf("\SIZE: %d BYTES\n", dat_mbr.mbr_partition_4.part_size);
            printf("\NAME: %s\n", dat_mbr.mbr_partition_4.part_name);
        }

    return 1;
    }
    else
    {
        return 0;
    }


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
    char path [100]=" ";
    char name[100]=" ";
    char bandera=1;
    //Esta es una bandera para que termine de validad los comandos
    char bandera_fin=1;
    char delete_[4]; // full/fast
    char add[12]; //
    char fit[2]; //WF BF/FF/WF
    char type; //P     P E L






    int t=strlen(comando_entrada);

    while(bandera_fin != 0)
    {
        switch (estado_actual) {
        //recorrer el vector para comparar cada posicion
        case 0:
            //validando la palabra reservada para pasar a otro estado
            if((comando_entrada[columna]=='m' || comando_entrada[columna]=='M') &&
                    (comando_entrada[columna+1]=='k' || comando_entrada[columna+1]=='K') &&
                    (comando_entrada[columna+2]=='d' || comando_entrada[columna+2]=='D') &&
                    (comando_entrada[columna+3]=='i' || comando_entrada[columna+3]=='I') &&
                    (comando_entrada[columna+4]=='s' || comando_entrada[columna+4]=='S') &&
                    (comando_entrada[columna+5]=='k' || comando_entrada[columna+5]=='K') )
            {
                //el memset limpia la cadena
                vaciar_vector(size);;
                vaciar_vector(path);;
                vaciar_vector(name);
                unit='\0';
                vaciar_vector(fit);
                type='\0';
                vaciar_vector(delete_);
                vaciar_vector(add);

                bandera_mkdisk=1;
                estado_actual=1;
                columna = columna + 6;
                bandera=1;

            }

            else if ((comando_entrada[columna] == 'e' || comando_entrada[columna] == 'E') &&
                     (comando_entrada[columna + 1] == 'x' || comando_entrada[columna + 1] == 'X') &&
                     (comando_entrada[columna + 2] == 'e' || comando_entrada[columna + 2] == 'E') &&
                     (comando_entrada[columna + 3] == 'c' || comando_entrada[columna + 3] == 'C'))

            {
                estado_actual=2;
                columna=columna+4;
                bandera_exec=1;

            }
            else if((comando_entrada[columna]=='r' || comando_entrada[columna]=='R') &&
                    (comando_entrada[columna+1]=='m' || comando_entrada[columna+1]=='M') &&
                    (comando_entrada[columna+2]=='d' || comando_entrada[columna+2]=='D' )&&
                    (comando_entrada[columna+3]=='i' || comando_entrada[columna+3]=='I') &&
                    (comando_entrada[columna+4]=='s' || comando_entrada[columna+4]=='S') &&
                    (comando_entrada[columna+5]=='k' || comando_entrada[columna+5]=='K') )
            {
                //el memset limpia la cadena
                vaciar_vector(size);;
                vaciar_vector(path);;
                vaciar_vector(name);
                unit='\0';
                vaciar_vector(fit);
                type='\0';
                vaciar_vector(delete_);
                vaciar_vector(add);

                bandera_rmdisk=1;
                estado_actual=32;
                columna=columna+6;


            }

            else if((comando_entrada[columna]=='f' || comando_entrada[columna]=='F') &&
                    (comando_entrada[columna+1]=='d' || comando_entrada[columna+1]=='D') &&
                    (comando_entrada[columna+2]=='i' || comando_entrada[columna+2]=='I') &&
                    (comando_entrada[columna+3]=='s' || comando_entrada[columna+3]=='S') &&
                    (comando_entrada[columna+4]=='k' || comando_entrada[columna+4]=='K') )
            {
                //el memset limpia la cadena
                vaciar_vector(size);;
                vaciar_vector(path);;
                vaciar_vector(name);
                unit='\0';
                vaciar_vector(fit);
                type='\0';
                vaciar_vector(delete_);
                vaciar_vector(add);

                bandera_fdisk=1;
                estado_actual=12;
                columna=columna+6;


            }

            else if((comando_entrada[columna]=='m' || comando_entrada[columna]=='M') &&
                    (comando_entrada[columna+1]=='o' || comando_entrada[columna+1]=='O') &&
                    (comando_entrada[columna+2]=='u' || comando_entrada[columna+2]=='U') &&
                    (comando_entrada[columna+3]=='n' || comando_entrada[columna+3]=='N') &&
                    (comando_entrada[columna+4]=='t' || comando_entrada[columna+4]=='T') )
            {

                bandera_mount=1;
                columna=columna+6;


            }

            else if((comando_entrada[columna]=='u' || comando_entrada[columna]=='U') &&
                    (comando_entrada[columna+1]=='m' || comando_entrada[columna+1]=='M') &&
                    (comando_entrada[columna+2]=='o' || comando_entrada[columna+2]=='O') &&
                    (comando_entrada[columna+3]=='u' || comando_entrada[columna+3]=='U') &&
                    (comando_entrada[columna+4]=='n' || comando_entrada[columna+4]=='N') &&
                    (comando_entrada[columna+5]=='t' || comando_entrada[columna+5]=='T') )
            {

                bandera_umount=1;
                columna=columna+7;
            }



            else if((comando_entrada[columna]=='c' || comando_entrada[columna]=='C') &&
                    (comando_entrada[columna+1]=='c' || comando_entrada[columna+1]=='C'))
            {

                columna +=2;
                columna +=1;
                output = 1;
                bandera_fin=1;
            }

            else if(comando_entrada[columna]=='#')
            {
                estado_actual=60;
                columna++;
                bandera_comentario=1;

            }
            else
                estado_actual=100; //Error
                //printf("Comando invalido...!! Vuelva intentarlo..?\n");

            break;

        case 1:

            if(comando_entrada[columna]==' ' || comando_entrada[columna]=='\\' ||comando_entrada[columna]=='\n')
            {
                //cuando encuentra espacio, barra inclinada o un salto de linea se queda en el mismo estado
                estado_actual = 1;
                columna++;

            }
            else if(comando_entrada[columna] == '-' &&
                        (comando_entrada[columna+1]=='s' || comando_entrada[columna+1]=='S') &&
                        (comando_entrada[columna+2]=='i' || comando_entrada[columna+2]=='I') &&
                        (comando_entrada[columna+3]=='z' || comando_entrada[columna+3]=='Z') &&
                        (comando_entrada[columna+4]=='e' || comando_entrada[columna+4]=='E') &&
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
                    else
                    if(comando_entrada[columna] =='\n'){
                       break;
                    }

                    else
                    {
                        printf("Error...!! Ingrese solo numeros.....!!!\n\n");
                        estado_actual=100;
                        break;
                    }

                }

            }

            else if (comando_entrada[columna] == '+' &&
                     (comando_entrada[columna+1]=='u' || comando_entrada[columna+1]=='U') &&
                     (comando_entrada[columna+2]=='n' || comando_entrada[columna+2]=='N') &&
                     (comando_entrada[columna+3]=='i' || comando_entrada[columna+3]=='I') &&
                     (comando_entrada[columna+4]=='t' || comando_entrada[columna+4]=='T') &&
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
                        printf("Error...!! Ingrese M ó K en mayuscula o minuscula .....!!!\n\n");
                        estado_actual=100;
                        unit= '\0';
                        break;


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

                if (comando_entrada[columna] == '"')
                {

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
                                estado_actual=100;
                                printf("Error...!! Escriba el nombre con extension (.dsk)...\n");
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
                    estado_actual = 100; //Error
                    printf("Error...!! Debe agregar las comillas dobles el inicio y al final...!!!");
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
                    printf("¡Importante...! Debe ingresar el tamanio del disco.\n");
                    bandera_fin = 0;
                }
                if (unit == '\0') {
                    printf("¡Importante...! Si no ingreso el valor de +unit:: se toma por defaul M.\n");
                    unit = 'M';
                }

                if (path[0] == '\0') {
                    printf("Importante...! escribe el nombre en comillas dobles.\n");
                    bandera_fin= 0;
                }

                if (name[0] == '\0') {
                    printf("Importante...! escribe el nombre y la extension en comiilas dobles.\n");
                    bandera_fin = 0;
                }

                if(bandera_fin==1){

                    if (bandera_mkdisk== 1)
                    {
                        /*printf("size: %s\n", size);
                        printf("MB o B 2: %c\n", unit);
                        printf("path: %s\n", path);
                        printf("name: %s\n", name);*/

                        crear_disco_nuevo(size, name, unit, path);
                        bandera_mkdisk=0;



                    }

                    bandera_fin=0;

                }




            }

            break;

        case 2:

            if(comando_entrada[columna]==' ' || comando_entrada[columna]=='\\' ||comando_entrada[columna]=='\n')
            {
                //cuando encuentra espacio, barra inclinada o un salto de linea se queda en el mismo estado
                estado_actual = 2;
                columna++;

            }

            else
            {
                int contador = 0;
                while (comando_entrada[columna] != ' ') {
                    if (comando_entrada[columna] == '.' && comando_entrada[columna + 1] == 's' && comando_entrada[columna+ 2] == 'h') {
                        path[contador] = '.';
                        path[contador + 1] = 's';
                        path[contador + 2] = 'h';
                        contador = contador + 2;
                        break;
                    }
                    path[contador] = comando_entrada[columna];
                    contador++;
                    columna++;

                }


                if (path[contador] == 'h' && path[contador - 1] == 's' && path[contador - 2] == '.')
                {

                    if (path[0] != "\0" ) {
                        char * aux = &path;
                        archivo_masivo(aux);
                        bandera_fin= 0;
                    }
                    else
                    {
                        printf("Error...\n");
                        bandera_fin= 0;
                    }
                } else {
                    printf("No se puede abrir....\n");
                    bandera_fin= 0;
                }


            }



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

        case 12:

            if(comando_entrada[columna]==' ' || comando_entrada[columna]=='\\' ||comando_entrada[columna]=='\n')
            {
                //cuando encuentra espacio, barra inclinada o un salto de linea se queda en el mismo estado
                estado_actual = 12;
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

                estado_actual =12;
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
                    else
                    if(comando_entrada[columna] =='\n'){
                       break;
                    }

                    else
                    {
                        printf("Error...!! Ingrese solo numeros.....!!!\n\n");
                        estado_actual=100;
                        break;
                    }

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
                estado_actual=12;
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

            else if (comando_entrada[columna] == '+' &&
                     comando_entrada[columna+1]=='u' || comando_entrada[columna+1]=='U' &&
                     comando_entrada[columna+2]=='n' || comando_entrada[columna+2]=='N' &&
                     comando_entrada[columna+3]=='i' || comando_entrada[columna+3]=='I' &&
                     comando_entrada[columna+4]=='t' || comando_entrada[columna+4]=='T' &&
                     comando_entrada[columna+5]==':' &&
                     comando_entrada[columna+6]==':')
            {
                estado_actual=12;
                columna=columna+7;

                //reconoce el valor de unit

                while (comando_entrada[columna] != ' ')
                {

                    if (comando_entrada[columna]== 'K' || comando_entrada[columna]== 'M' || comando_entrada[columna] == 'k' || comando_entrada[columna] == 'm' || comando_entrada[columna] == 'b' || comando_entrada[columna] == 'B') {
                        unit = comando_entrada[columna];
                        columna++;
                    }
                    else if(comando_entrada[columna]=='\n')
                    {
                        break;
                    }
                    else
                    {
                        printf("Error...!! Ingrese M ó K ó B  en mayuscula o minuscula .....!!!\n\n");
                        estado_actual=100;
                        unit= '\0';
                        break;

                    }

                }

            }

            else if(comando_entrada[columna] == '+' &&
                    comando_entrada[columna+1]=='t' || comando_entrada[columna+1]=='T' &&
                    comando_entrada[columna+2]=='y' || comando_entrada[columna+2]=='Y' &&
                    comando_entrada[columna+3]=='p' || comando_entrada[columna+3]=='P' &&
                    comando_entrada[columna+4]=='e' || comando_entrada[columna+4]=='E' &&
                    comando_entrada[columna+5]==':' &&
                    comando_entrada[columna+6]==':')
            {
                estado_actual =12;
                columna=columna+7;

                while (comando_entrada[columna] != ' ')
                {

                    if (comando_entrada[columna]== 'p' || comando_entrada[columna]== 'P' || comando_entrada[columna] == 'l' || comando_entrada[columna] == 'L' || comando_entrada[columna] == 'e' || comando_entrada[columna] == 'E') {
                        type = comando_entrada[columna];
                        columna++;
                    }
                    else if(comando_entrada[columna]=='\n')
                    {
                        break;
                    }
                    else
                    {
                        printf("Error...!! Ingrese P ó L ó E en mayuscula o minuscula .....!!!\n\n");
                        estado_actual=100;
                        break;

                    }

                }


            }

            else if(comando_entrada[columna] == '+' &&
                    comando_entrada[columna+1]=='f' || comando_entrada[columna+1]=='F' &&
                    comando_entrada[columna+2]=='i' || comando_entrada[columna+2]=='I' &&
                    comando_entrada[columna+3]=='t' || comando_entrada[columna+3]=='T' &&
                    comando_entrada[columna+4]==':' &&
                    comando_entrada[columna+5]==':')

            {
                estado_actual=12;
                columna=columna+6;

                int contador = 0;

                while (comando_entrada[columna]!= '\0') {
                    if (comando_entrada[columna] == '-' || comando_entrada[columna]== ' ') {
                        if (((fit[0] == 'B' || fit[0] == 'b') && (fit[1] == 'F' || fit[1] == 'f')) || ((fit[0] == 'F' || fit[0] == 'f') && (fit[1] == 'F' || fit[1] == 'f')) || ((fit[0] == 'W' || fit[0] == 'w') && (fit[1] == 'F' || fit[1] == 'f'))) {
                            break;
                        } else {
                            printf("Comando invalido, los tipos de ajustes deben ser: Mejor Ajuste (BF) o Primer Ajuste (FF) o Pero Ajuste (WF).\n");
                            fit[0] = '\0';
                            break;
                        }
                    }
                    fit[contador] = comando_entrada[columna];
                    contador++;
                    columna++;
                }



            }

            else if(comando_entrada[columna] == '+' &&
                    comando_entrada[columna+1]=='d' || comando_entrada[columna+1]=='D' &&
                    comando_entrada[columna+2]=='e' || comando_entrada[columna+2]=='E' &&
                    comando_entrada[columna+3]=='l' || comando_entrada[columna+3]=='L' &&
                    comando_entrada[columna+4]=='e' || comando_entrada[columna+4]=='E' &&
                    comando_entrada[columna+5]=='t' || comando_entrada[columna+5]=='T' &&
                    comando_entrada[columna+6]=='e' || comando_entrada[columna+6]=='E' &&
                    comando_entrada[columna+7]==':' &&
                    comando_entrada[columna+8]==':')

            {
                estado_actual=12;
                columna=columna+9;
                bandera_name=1;
                bandera_path=1;
                int contador = 0;

                while (comando_entrada[columna] != ' ') {
                    if (comando_entrada[columna]== '-' || comando_entrada[columna] == ' ') {

                        if (((delete_[0] == 'F' || delete_[0] == 'f') &&
                             (delete_[1] == 'U' || delete_[1] == 'u') &&
                             (delete_[2] == 'L' || delete_[2] == 'l') &&
                             (delete_[3] == 'L' || delete_[3] == 'l')) ||
                            ((delete_[0] == 'F' || delete_[0] == 'f') &&
                             (delete_[1] == 'A' || delete_[1] == 'a') &&
                             (delete_[2] == 'S' || delete_[2] == 's') &&
                             (delete_[3] == 'T' || delete_[3] == 't')))
                        {
                            break;

                        }
                        else
                        {
                            printf("Comando invalido,  full o fast.\n");
                            delete_[0] = '\0';
                            break;
                        }
                    }
                    if(comando_entrada[columna]=='\n')
                        break;

                    if(comando_entrada[columna]!='\n')
                    {
                        delete_[contador] = comando_entrada[columna];
                        contador++;
                        columna++;
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

                estado_actual = 12;
                columna=columna + 7;

                if (comando_entrada[columna]== '"') {

                    columna++;
                    int contador = 0;
                    while (comando_entrada[columna]!= '-') {
                        if (comando_entrada[columna] == '"') {
                            columna++;
                            break;
                        }
                        name[contador] = comando_entrada[columna];
                        contador++;
                        columna++;
                    }
                } else {
                    estado_actual = 100; //Error
                }


            }

            else
            {
                if (size[0] == ' ') {
                    printf("¡Importante...! Debe ingresar el tamanio del particion.\n");
                    bandera_fin = 0;
                }

                if (path[0] == '\0') {
                    printf("Importante...! escribe el nombre en comillas dobles.\n");
                    bandera_fin= 0;
                }

                if (name[0] == '\0') {
                    printf("¡Importante...!  escribe el nombre entre las comiilas dobles.\n");
                    bandera_fin = 0;
                }

                if (unit == '\0') {
                    printf("¡Importante...! Si no ingreso el valor de +unit:: se toma por defaul k.\n");
                    unit = 'K';
                }


                if(type =='\0'){
                    printf("¡Importante...! Si no ingreso el valor de +type:: se toma por defaul P.\n");
                    type='P';
                }



                if(delete_==' ' )
                {
                    bandera_fin=0;

                }
                else
                {
                    if(bandera_name==1 && bandera_path==1){
                        printf("tiene lo necesario para usar delete\n");
                    }
                }
                if(add=='\0')
                {

                }
                if(fit[0]=='\0' || fit[0]==' ')
                {
                    printf("¡Importante...! no se escribio el comando +fit se ingreso por defecto el valor WF.\n");
                    fit[0]='W';
                    fit[1]='F';
                }

                if(bandera_fin==1){
                    if (bandera_fdisk== 1)
                    {
                        /*printf("size: %s\n", size);
                        printf("unit: %c\n", unit);
                        printf("path: %s\n", path);
                        printf("name: %s\n", name);
                        printf("type: %c\n", type);
                        printf("delete: %s\n", delete_);
                        printf("fit: %s\n", fit);*/

                        if(crear_particion_(size, unit, path, name, fit, type))
                        {
                            bandera_fdisk=0;
                            bandera_fin=0;
                        }
                        else
                        {
                            bandera_fdisk=0;
                            bandera_fin=0;
                        }

                    }

                }


            }



            break;

        case 32:
            if(comando_entrada[columna]==' ' || comando_entrada[columna]=='\\' ||comando_entrada[columna]=='\n')
            {
                //cuando encuentra espacio, barra inclinada o un salto de linea se queda en el mismo estado
                estado_actual = 32;
                columna++;

            }

            else if(comando_entrada[columna] == '-' &&
                    comando_entrada[columna+1]=='p' || comando_entrada[columna+1]=='P' &&
                    comando_entrada[columna+2]=='a' || comando_entrada[columna+2]=='A' &&
                    comando_entrada[columna+3]=='t' || comando_entrada[columna+3]=='T' &&
                    comando_entrada[columna+4]=='h' || comando_entrada[columna+4]=='H' &&
                    comando_entrada[columna+5]==':' &&
                    comando_entrada[columna+6]==':')
            {
                estado_actual=32;
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
                if (path[0] == '\0') {
                    printf("Importante...! escriba -path:: seguido de la ruta o directorio .\n");
                    bandera_fin= 0;
                }

                if(bandera_fin==1)
                {
                    if(bandera_rmdisk==1)
                    {
                        char c;
                        printf("Quieres eliminar el archivo s/n.....\n");
                        scanf("%c",&c);


                        if(c=='s')
                        {
                            eliminar_disco(path);
                            printf("Eliminado exitosamente......\n");
                            bandera_rmdisk=0;
                            bandera_fin= 0;
                        }
                        else
                        {
                            printf("Cancelando ===>  :( ...\n");
                            bandera_rmdisk=0;
                            bandera_fin= 0;
                        }

                    }

                }


            }

            break;

        case 60:


            if (comando_entrada[columna] == 'a' ||comando_entrada[columna] == 'b' ||comando_entrada[columna] == 'c' ||comando_entrada[columna] == 'd' ||comando_entrada[columna] == 'e'
                    ||comando_entrada[columna] == 'f' ||comando_entrada[columna] == 'g' ||comando_entrada[columna] == 'h' ||comando_entrada[columna] == 'i' ||comando_entrada[columna] == 'j'
                    ||comando_entrada[columna] == 'k' ||comando_entrada[columna] == 'l' ||comando_entrada[columna] == 'm' ||comando_entrada[columna] == 'n' ||comando_entrada[columna] == 'o'
                    ||comando_entrada[columna] == 'p' ||comando_entrada[columna] == 'q' ||comando_entrada[columna] == 'r' ||comando_entrada[columna] == 's' ||comando_entrada[columna] == 't'
                    ||comando_entrada[columna] == 'u' ||comando_entrada[columna] == 'v' ||comando_entrada[columna] == 'w' ||comando_entrada[columna] == 'x' ||comando_entrada[columna] == 'y'
                    ||comando_entrada[columna] == 'z' ||comando_entrada[columna] == 'A' ||comando_entrada[columna] == 'B' ||comando_entrada[columna] == 'C' ||comando_entrada[columna] == 'D'
                    ||comando_entrada[columna] == 'E' ||comando_entrada[columna] == 'F' ||comando_entrada[columna] == 'G' ||comando_entrada[columna] == 'H' ||comando_entrada[columna] == 'I'
                    ||comando_entrada[columna] == 'J' ||comando_entrada[columna] == 'K' ||comando_entrada[columna] == 'L' ||comando_entrada[columna] == 'M' ||comando_entrada[columna] == 'N'
                    ||comando_entrada[columna] == 'O' ||comando_entrada[columna] == 'P' ||comando_entrada[columna] == 'Q' ||comando_entrada[columna] == 'R' ||comando_entrada[columna] == 'S'
                    ||comando_entrada[columna] == 'T' ||comando_entrada[columna] == 'U' ||comando_entrada[columna] == 'V' ||comando_entrada[columna] == 'W' ||comando_entrada[columna] == 'X'
                    ||comando_entrada[columna] == 'Y' ||comando_entrada[columna] == 'Z' ||comando_entrada[columna] == '0' ||comando_entrada[columna] == '1' ||comando_entrada[columna] == '2'
                    ||comando_entrada[columna] == '3' ||comando_entrada[columna] == '4' ||comando_entrada[columna] == '5' ||comando_entrada[columna] == '6' ||comando_entrada[columna] == '7'
                    ||comando_entrada[columna] == '8' ||comando_entrada[columna] == '9' ||comando_entrada[columna] == '_' ||comando_entrada[columna] == '°' ||comando_entrada[columna] == '|'
                    ||comando_entrada[columna] == '¬' ||comando_entrada[columna] == '!' ||comando_entrada[columna] == '"' ||comando_entrada[columna] == '#' ||comando_entrada[columna] == '$'
                    ||comando_entrada[columna] == '%' ||comando_entrada[columna] == '&' ||comando_entrada[columna] == '/' ||comando_entrada[columna] == '(' ||comando_entrada[columna] == ')'
                    ||comando_entrada[columna] == '?' ||comando_entrada[columna] == '\\' ||comando_entrada[columna] == '¡' ||comando_entrada[columna] == '¿' ||comando_entrada[columna] == '´'
                    ||comando_entrada[columna] == '+' ||comando_entrada[columna] == '*' ||comando_entrada[columna] == '~' ||comando_entrada[columna] == '{' ||comando_entrada[columna] == '}'
                    ||comando_entrada[columna] == '[' ||comando_entrada[columna] == ']' ||comando_entrada[columna] == '_' ||comando_entrada[columna] == '-' ||comando_entrada[columna] == ':'
                    ||comando_entrada[columna] == '.' ||comando_entrada[columna] == ',' ||comando_entrada[columna] == ';' ||comando_entrada[columna] == ' ' ||comando_entrada[columna] == 'á'
                    ||comando_entrada[columna] == 'é' ||comando_entrada[columna] == 'í' ||comando_entrada[columna] == 'ó' ||comando_entrada[columna] == 'ú' ||comando_entrada[columna] == 'Á'
                    ||comando_entrada[columna] == 'É' ||comando_entrada[columna] == 'Í' ||comando_entrada[columna] == 'Ó' ||comando_entrada[columna] == 'Ú')
            {

                estado_actual = 60;
                columna++;

            }

            else if (comando_entrada[columna] == '\n')
            {
                printf("Escribe un comando valido...\n");
                bandera_fin = 0;
            }



            break;

        case 100:


            bandera_fin=0;

            break;

        default:
            printf("Nose encontro el comando de entrada...!!");
            bandera_fin=0;
            break;
        }

    }//fin del while que recorre todos los caracteres
}

//Metodo para vaciar vectores
void vaciar_vector(char vector[])
{
    int x;
    for (x = 0; x < strlen(vector); x++)
        vector[x] = ' ';
}




// inicio principal
int main()
{

    char comando_entrada[100];

    printf("  ****************************************************************************\n");
    printf("  ********************* MANEJO E IMPLEMENTACION DE ARCHIVO *******************\n");
    printf("  ****************************************************************************\n");
    printf("  ****************************************************************************\n");
    printf("  ********************Para salir ingrese el comando cc ***********************\n");
    printf("  ****************************************************************************\n\n");



//archivo_masivo("/home/jonatan/calificacion.sh");

    while(output !=1)
    {

        printf("::>");
        fgets(comando_entrada, 100, stdin);
        //scanf("%s", &comando_entrada);
        strcat(comando_entrada, " ");
        validar(comando_entrada);


    }

    printf("\n\nSaliendo del sistema.....!!\n");


    return 0;
}


