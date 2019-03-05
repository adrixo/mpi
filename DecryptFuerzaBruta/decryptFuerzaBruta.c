/*
  si no funciona al compilar:
  gcc vBasicaSecuencial.c -lcrypt; ./a.out > prueba1.txt
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define _XOPEN_SOURCE
#include <unistd.h>
#include <crypt.h>
#include <time.h>
#include "mpi.h"
#include <limits.h>

#define TAG 10

#define SALT "aa"
#define KEY_LENGTH 7 //Tenemos 20 claves encriptadas de 7 caracteres que contienen solo cifras.
#define CRYPTED_LENGTH 14 //el tamaño de una contraseña encriptada es 13+1
#define NUM_CLAVES 4 //numero de claves a generar
#define MIN_RAND 100000 //realmente sería 0..0, se utiliza 10000 para evitar eliminacion de 0 en el parseo
#define MAX_RAND 999999
/*
#define MIN_RAND 1000000 //realmente sería 0..0, se utiliza 10000 para evitar eliminacion de 0 en el parseo
#define MAX_RAND 9999999
*/

#define TAM_MENSAJE 5

//se resolverá generando numeros aleatorios de 0..0 a 9..9

void generarKeyList(char listaClaves[NUM_CLAVES][2][CRYPTED_LENGTH]);
int obtenerClaveADesencriptar(int clavesEncontradas[NUM_CLAVES]);
char * desencriptarClave(char claveEncriptada[CRYPTED_LENGTH], int *repeticiones);
void marcarDesencriptada(char listaClaves[NUM_CLAVES][2][CRYPTED_LENGTH], int clavesEncontradas[NUM_CLAVES], char claveDesencriptada[KEY_LENGTH+1], int proceso);

void printClaves(char listaClaves[NUM_CLAVES][2][CRYPTED_LENGTH], int seed);
void printMonitor(char listaClaves[NUM_CLAVES][2][CRYPTED_LENGTH], int clavesEncontradas[NUM_CLAVES], int *procAsignadoA, int iNumProcs);
void printResultados(unsigned int repeticiones, double tiempoTotal, int iNumProcs);
// char *crypt(const char *key, const char *salt);

/* Secuencial:
 *  1. Declaraciones e inicio conteo tiempos
 *  2. Generamos claves aleatorias en la listaClaves
 *  3. mientras clavesEncontradas < N_CLAVES
 *   3.1 encontramos una clave que case
 *   3.2 añadimos a encontradas y repetimos
 *  4. presentamos resultados
 */
void main(int argc , char **argv)
{
  int seed=9999;
  int i, j;
  unsigned int repeticiones = 0;
  char *claveDesencriptada;
  int iId;
  int iNumProcs;

  unsigned int mensajeNumClave;
  MPI_Status status;
  MPI_Request request;

  MPI_Init (&argc ,&argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &iId);
  MPI_Comm_size(MPI_COMM_WORLD, &iNumProcs);

  /* Podemos realizar el paso de las claves de 2 maneras:
   *  1. Pasar la clave como string de padre -> hijo
   *  2. Que todos conozcan todas las claves y el padre solo notifique al hijo
   *     cual es el n de la que tiene que resolver. En este esquema se hace esto último
   */
  srand(seed);
  char listaClaves[NUM_CLAVES][2][CRYPTED_LENGTH]; //Lista clave-claveEncriptada que maneja el proceso principal
  generarKeyList(listaClaves); //si eso lo realizamos en todo los procesos con la misma seed tendriamos la misma clave para todos y no necesitariamos el paso de mensajes

  switch(iId){

    case 0: //padre
    {
    //Declaracion variables padre
      double clock_start, clock_end, tiempoTotal;
      clock_start=MPI_Wtime();
      int clavesEncontradas[NUM_CLAVES];
            /* entendido como otra columna extra de listaClaves almacena -1 si una clave no ha sido encontrada y nProceso que la encontró en caso contrario.
             * p.e: cE[2] = 4 <-> La clave 3 fue encontrada por el proceso 4
             * En caso de no haber sido resuelta almacena un -x siendo
             * x-1 el numero de procesos tratando de desencriptar una clave
             */
      for(i=0; i<NUM_CLAVES; i++){
        clavesEncontradas[i]=-1; //lo inicializamos marcando a todas como no resuelta
      }

      int nClavesEncontradas = 0; //nClaves encontradas
      int numClaveDesencriptada; //numero clave devuelta por un proceso que la ha resuelto
      int procesoDesencriptador;
      int procAsignadoA[iNumProcs]; //Cada proceso i se asigna a una clave a desencriptar, -1 si esta inactivo.
      for(i=0; i<iNumProcs; i++){
        procAsignadoA[i]=-1; //lo inicializamos marcando a todas como no resuelta
      }

    //generacion claves
      //generarKeyList(listaClaves); //se hace fuera del switch, si eso lo realizamos en todo los procesos con la misma seed tendriamos la misma clave para todos y no necesitariamos el paso de mensajes
      printClaves(listaClaves, seed);

    //busqueda de Claves
      //Iniciamos con una tarea para todos los procesos (tener en cuenta si nprocesos > nClaves)
      for(int i=1; i<iNumProcs; i++){
        mensajeNumClave =  obtenerClaveADesencriptar(clavesEncontradas);//n clave a desencriptar
        procAsignadoA[i] = mensajeNumClave; //indicamos que el proceso i trabaja con la clave obtenida
        clavesEncontradas[ mensajeNumClave ] += -1; // por optimización reducimos 1, si entendiamos que -1 era que no se habia encontrado, podemos entender -x como que x procesos (x-1 en realida) estan con esa tarea, por optimizacion
        MPI_Send(&mensajeNumClave, 1, MPI_UNSIGNED, i, TAG, MPI_COMM_WORLD);
      }

      printMonitor(listaClaves, clavesEncontradas, procAsignadoA, iNumProcs);

      nClavesEncontradas = 0;
      while (nClavesEncontradas < NUM_CLAVES)
      {
        MPI_Recv(&mensajeNumClave, 1, MPI_UNSIGNED, MPI_ANY_SOURCE/*global*/, TAG, MPI_COMM_WORLD, &status); //bloqueante, esperamos recibir clave desencriptada

        numClaveDesencriptada = mensajeNumClave;
        procesoDesencriptador = status.MPI_SOURCE;/*proceso del que se ha recibido*/
        //manejar repeticiones

        if(numClaveDesencriptada >= 0 &&
           numClaveDesencriptada < NUM_CLAVES &&
           clavesEncontradas[numClaveDesencriptada] < 0 ){//recibimos n clave desencriptada, esta no debe haberse resuelto anteriormente

        //1. marcamos esa contraseña como desencriptada, dos opciones dependiendo de implementacion:
          //marcarDesencriptada(listaClaves, clavesEncontradas, claveDesencriptada, procesoDesencriptador); //si se envia la clave en vez de n clave
          clavesEncontradas[numClaveDesencriptada] = procesoDesencriptador; //la marcamos como resuelta //si se envia el num clave en vez de la clave
          nClavesEncontradas++;
          //faltaría manejar repeticiones

          if(nClavesEncontradas < NUM_CLAVES) //solo si quedan
        //2. Mandamos recados los procesos que estaban trabajando con la clave desencriptada, estos pasarian a desencriptar la nueva clave
            for(i=1; i<iNumProcs; i++)
            {
              if(procAsignadoA[i] == numClaveDesencriptada)
              {
                mensajeNumClave = obtenerClaveADesencriptar(clavesEncontradas);
                MPI_Send( &mensajeNumClave, 1, MPI_UNSIGNED, i, TAG, MPI_COMM_WORLD);

                procAsignadoA[i] = mensajeNumClave;
                clavesEncontradas[ mensajeNumClave ] += -1; // por optimización reducimos 1 ... vease anterior cE[] += -1
              }
            }
        } //fi

        printMonitor(listaClaves, clavesEncontradas, procAsignadoA, iNumProcs);
      } //while

    //finalizamos
      clock_end = MPI_Wtime();
      tiempoTotal = (clock_end - clock_start);// / (double) CLOCKS_PER_SEC;

      printf("\n\tFINALIZADO - RECIBIENDO REPETICIONES DE LOS PROCESOS\n");
      //Matamos a los procesos
      for(int i=1; i<iNumProcs; i++){
        mensajeNumClave = NUM_CLAVES + 1; //si el mensaje contiene una contraseña no valida (maximo + 1 por ejemplo) terminamos
        MPI_Send(&mensajeNumClave, 1, MPI_UNSIGNED, i, TAG, MPI_COMM_WORLD);
      }

      MPI_Barrier(MPI_COMM_WORLD);

      unsigned int totalRepeticiones=0;
      //Recogemos las repeticiones
      for(i=0; i<iNumProcs-1; i++){
        MPI_Recv(&mensajeNumClave, 1, MPI_UNSIGNED, MPI_ANY_SOURCE/*global*/, TAG, MPI_COMM_WORLD, &status);
        printf("\tRepeticiones hijo %d: %d - %f%% (sobre UINT_MAX)\n", status.MPI_SOURCE, mensajeNumClave, (float) mensajeNumClave / UINT_MAX);
        totalRepeticiones += mensajeNumClave;
      }
      printf("\tRepeticiones Total: %d - %f%% (sobre UINT_MAX)\n", totalRepeticiones, (float) totalRepeticiones / INT_MAX);

      printMonitor(listaClaves, clavesEncontradas, procAsignadoA, iNumProcs);
      printResultados(totalRepeticiones, tiempoTotal, iNumProcs);


      break;
    }

    /* Hijo:
    *
     * bucle infinito
     *  1. recibimos un mensaje del padre con el num clave a desencriptar
     *  2. comenzamos a desencriptar la clave
     *    2.1 Si desencriptarClave() finaliza con "cancel" es que ya fue desencriptada por otro proceso y tenemos un mensaje esperando con la siguiente
     *    2.2 Si desencriptarClave() devuelve la clave desencriptada notificamos al padre de que ya terminamos
     */
    default: //hijos https://lsi.ugr.es/jmantas/ppr/ayuda/mpi_ayuda.php?ayuda=MPI_Iprobe manejo de interrupciones
    {
      int nClaveADesencriptar;
      srand(iId*100); //iniciamos el seed de cada hijo rand por ejemplo el id*100
      repeticiones=0; //recogemos las repeticiones

      while(1) { //por seguridad he puesto un límite, pero seria bucle infinito
        MPI_Recv( &mensajeNumClave, 1, MPI_UNSIGNED, 0, TAG, MPI_COMM_WORLD, &status);

        if(mensajeNumClave ==  NUM_CLAVES + 1)
          break; //si el mensaje contiene una contraseña no valida (maximo + 1 por ejemplo) terminamos

        nClaveADesencriptar = mensajeNumClave;
        claveDesencriptada = desencriptarClave( listaClaves[ mensajeNumClave ][1], &repeticiones);

        //Si la clave ha sido desencriptada enviamos, en caso contrario (ha sido resuelta por otro) volvemos al recv a recibir la nueva a desencriptar
        if(strcmp(claveDesencriptada, "cancel") == 0){
          continue;
        } else {
          mensajeNumClave = nClaveADesencriptar;
          MPI_Send(&mensajeNumClave, 1, MPI_UNSIGNED, 0, TAG, MPI_COMM_WORLD);
        }
      }

      MPI_Barrier(MPI_COMM_WORLD);
      //Una vez acabado mandamos las repeticiones al padre
      mensajeNumClave = repeticiones;
      MPI_Send(&mensajeNumClave, 1, MPI_UNSIGNED, 0, TAG, MPI_COMM_WORLD);

      break;
    } //default
  } //case

  MPI_Finalize();
  return;
}

/*
 * Llena una matriz string 20x2 aleatoriamente: clave / clave encriptada
 */
void generarKeyList(char listaClaves[NUM_CLAVES][2][CRYPTED_LENGTH]){
  int i;
  int randNum;

  for(i=0; i<NUM_CLAVES; i++){
    randNum =  rand()%MAX_RAND;
    //para evitar que se sustituya 0X por X
    if(randNum > MIN_RAND){
      sprintf(listaClaves[i][0], "%d", randNum);
    }
    else {
      sprintf(listaClaves[i][0], "%d", randNum+MIN_RAND);
      listaClaves[i][0][0] = '0';
    }

    strcpy(listaClaves[i][1], crypt(listaClaves[i][0], SALT));
  }

}

// Imprime las claves
void printClaves(char listaClaves[NUM_CLAVES][2][CRYPTED_LENGTH], int seed){
  int i;
    printf("\n--Claves generadas con semilla %d--\n\tRango: %d - %d\n", seed, 0, MAX_RAND);
  for(i=0; i<NUM_CLAVES; i++){
    printf("%02d) %s %s\n", i, listaClaves[i][0], listaClaves[i][1]);
  }
}

void printMonitor(char listaClaves[NUM_CLAVES][2][CRYPTED_LENGTH], int clavesEncontradas[NUM_CLAVES], int *procAsignadoA, int iNumProcs){
  int i;
  printf("\n---------------------\n");
  for(i=0; i<NUM_CLAVES; i++){
    printf("%02d) %s %s -> (%d)\n", i, listaClaves[i][0], listaClaves[i][1], clavesEncontradas[i]);
  }
  printf("\n-------Procesos------\n");
  for(i=0; i<iNumProcs; i++){
    printf("Proceso %d resolviendo clave %d\n", i, procAsignadoA[i]);
  }
}

void printResultados(unsigned int repeticiones, double tiempoTotal, int iNumProcs){
  int tamanoClave=0;
  for(int i = MAX_RAND; i>0; i /= 10) tamanoClave++;

  printf("\nCASO:\n - Claves  : %d \n - Tamanio : %d \n - procesos: %d + padre\n\nRESULTADO:\n - Repeticiones: %d \n - Tiempo: %f \n\n", NUM_CLAVES, tamanoClave, iNumProcs-1, repeticiones, tiempoTotal);

  FILE *fd;
  char * filename = malloc((32)*sizeof(char));
  char * contenido = malloc((1024)*sizeof(char));
  char * datos = malloc((512)*sizeof(char));
  sprintf(filename, "%d_claves.txt\0", NUM_CLAVES);
  char cabecera[128] = "tamano; numeroProcesos; repeticiones; tiempoTotal;";
  sprintf(datos,      "%d; %d; %d; %f;",tamanoClave, iNumProcs-1, repeticiones, tiempoTotal);


  fd = fopen(filename, "r");
  if(fd == NULL){ //si no existe, añadimos la cabecera
    sprintf(contenido, "%s\n%s\n\0", cabecera, datos);
  } else {
    fclose(fd);
    sprintf(contenido, "%s\n\0", datos);
  }

  fd = fopen(filename, "a");
  fprintf(fd, "%s", contenido);
  fclose(fd);
}

/*
 * Devuelve el n de una clave que no ha sido encontrada todavía
 * Elige la que menos procesos tenga asignada esto es
 * la que mayor numero negativo tenga
 * (-1 son 0 procesos asignados, -3 son 2 procesos asignados)
 */
int obtenerClaveADesencriptar(int clavesEncontradas[NUM_CLAVES]){
  int i;
  int claveConMenorNumProcesos=0;

  for(i=0; i<NUM_CLAVES; i++){
    if( clavesEncontradas[i] < 0){ //de las negativas
      if(clavesEncontradas[i] >= clavesEncontradas[claveConMenorNumProcesos]) //la mayor
        claveConMenorNumProcesos = i;
    }
  }

  return claveConMenorNumProcesos;
}


/*
 * Dada una clave encriptada, mediante fuerza bruta va encriptando posibles claves y comparando
 * La semilla de rand se tiene que seleccionar previamente.
 *
 * mediante statusItnerrupcion y test podemos comprobar si ha llegado una interrupcion
 *
 * Devuelve la clave que al encriptarse coincide.
 */
char * desencriptarClave(char claveEncriptada[CRYPTED_LENGTH], int *repeticiones){
  char * claveDesencriptada;
  claveDesencriptada = malloc((KEY_LENGTH+1)*sizeof(char));
  int randNum;

  int flag;
  MPI_Status statusInterrupcion;

  while(1){
    (*repeticiones)++;

//1. probamos si ha llegado un nuevo codigo a desencriptar con int MPI_Iprobe(int source, int tag, MPI_Comm comm, int *flag, MPI_Status *status)
    MPI_Iprobe(MPI_ANY_SOURCE, TAG, MPI_COMM_WORLD, &flag, &statusInterrupcion);
    if(flag) //Si no me equivoco leyendo la documentacion flag = true si hay mensajes a ser leidos
      return "cancel";

//2. probamos un codigo aleatorio
    randNum =  rand()%MAX_RAND;
    //para evitar que se sustituya 0X por X, podiamos usar %8d pero entonces no podriamos cambiar el tam contraseñas
    if(randNum >= MIN_RAND){
      sprintf(claveDesencriptada, "%d\0", randNum);
    }
    else {
      sprintf(claveDesencriptada, "%d\0", randNum+MIN_RAND);
      claveDesencriptada[0] = '0';
    }

    if( strcmp(claveEncriptada, crypt(claveDesencriptada, SALT)) == 0 ){
      return claveDesencriptada;
    }
  }

  return claveDesencriptada;
}

/*
 * Dada una claveDesencriptada, busca la clave en la listaClaves que coincida y añade el proceso que la descubrió a clavesEncontradas
 */
void marcarDesencriptada(char listaClaves[NUM_CLAVES][2][CRYPTED_LENGTH], int clavesEncontradas[NUM_CLAVES],char claveDesencriptada[KEY_LENGTH+1], int proceso){
  int i;

  for(i=0; i<NUM_CLAVES; i++){
    if(strcmp(listaClaves[i][0], claveDesencriptada) == 0){
      clavesEncontradas[i]=proceso;
      return;
    }
  }
}
