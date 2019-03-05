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

#define SALT "aa"
#define KEY_LENGTH 7 //Tenemos 20 claves encriptadas de 7 caracteres que contienen solo cifras.
#define CRYPTED_LENGTH 14 //el tamaño de una contraseña encriptada es 13+1
#define PASSWORDS 4 //numero de claves a generar
#define MIN_RAND 10000 //realmente sería 0..0, se utiliza 10000 para evitar eliminacion de 0 en el parseo
#define MAX_RAND 99999
/*
#define MIN_RAND 1000000 //realmente sería 0..0, se utiliza 10000 para evitar eliminacion de 0 en el parseo
#define MAX_RAND 9999999
*/

#define TAM_MENSAJE 5

//se resolverá generando numeros aleatorios de 0..0 a 9..9

void generarKeyList(char keyList[PASSWORDS][2][CRYPTED_LENGTH]);
int obtenerClaveADesencriptar(int clavesEncontradas[PASSWORDS]);
char * desencriptarClave(char claveEncriptada[CRYPTED_LENGTH], int *repeticiones);
void marcarDesencriptada(char keyList[PASSWORDS][2][CRYPTED_LENGTH], int clavesEncontradas[PASSWORDS], char claveDesencriptada[KEY_LENGTH+1], int proceso);

void printClaves(char keyList[PASSWORDS][2][CRYPTED_LENGTH], int seed);
void printMonitor(char keyList[PASSWORDS][2][CRYPTED_LENGTH], int clavesEncontradas[PASSWORDS]);
void printResultados(int repeticiones, double tiempoTotal);
// char *crypt(const char *key, const char *salt);

/* Secuencial:
 *  1. Declaraciones e inicio conteo tiempos
 *  2. Generamos claves aleatorias en la keyList
 *  3. mientras clavesEncontradas < N_CLAVES
 *   3.1 encontramos una clave que case
 *   3.2 añadimos a encontradas y repetimos
 *  4. presentamos resultados
 */
void main(int argc , char **argv)
{
  int seed=9999;
  int i, j, repeticiones = 0;
  char *claveDesencriptada;
  int iId;
  int iNumProcs;

  int mensajeNumClave[TAM_MENSAJE]; //supongamos por ejemplo que en el primer campo esta el n clave y en el segundo el n repeticiones
  MPI_Datatype tipo_mensajeNumClave;
  MPI_Status status;
  MPI_Request request;

  MPI_Init (&argc ,&argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &iId);
  MPI_Comm_size(MPI_COMM_WORLD, &iNumProcs);

  MPI_Type_vector(TAM_MENSAJE, 1, 1, MPI_INT, &tipo_mensajeNumClave);
  MPI_Type_commit(&tipo_mensajeNumClave);

  /* Podemos realizar el paso de las claves de 2 maneras:
   *  1. Pasar la clave como string de padre -> hijo
   *  2. Que todos conozcan todas las claves y el padre solo notifique al hijo
   *     cual es el n de la que tiene que resolver. En este esquema se hace esto último
   */
  srand(seed);
  char keyList[PASSWORDS][2][CRYPTED_LENGTH]; //Lista clave-claveEncriptada que maneja el proceso principal
  generarKeyList(keyList); //si eso lo realizamos en todo los procesos con la misma seed tendriamos la misma clave para todos y no necesitariamos el paso de mensajes

  switch(iId){

    case 0: //padre
    {
    //Declaracion variables padre
      double clock_start, clock_end, tiempoTotal;
      clock_start=MPI_Wtime();
      int clavesEncontradas[PASSWORDS];
            /* entendido como otra columna extra de keyList almacena -1 si una clave no ha sido encontrada y nProceso que la encontró en caso contrario.
             * p.e: cE[2] = 4 <-> La clave 3 fue encontrada por el proceso 4
             * En caso de no haber sido resuelta almacena un -x siendo
             * x-1 el numero de procesos tratando de desencriptar una clave
             */
      for(i=0; i<PASSWORDS; i++){
        clavesEncontradas[i]=-1; //lo inicializamos marcando a todas como no resuelta
      }

      int nClavesEncontradas = 0; //nClaves encontradas
      int procAsignadoA[iNumProcs]; //Cada proceso i se asigna a una clave a desencriptar, -1 si esta inactivo.
      int numClaveDesencriptada; //numero clave devuelta por un proceso que la ha resuelto

    //generacion claves
      //generarKeyList(keyList); //se hace fuera del switch, si eso lo realizamos en todo los procesos con la misma seed tendriamos la misma clave para todos y no necesitariamos el paso de mensajes
      printClaves(keyList, seed);

    //busqueda de Claves
      //Iniciamos con una tarea para todos los procesos (tener en cuenta si nprocesos > nClaves)
      for(int i=1; i<iNumProcs; i++){
        mensajeNumClave[0] = obtenerClaveADesencriptar(clavesEncontradas);//n clave a desencriptar
        procAsignadoA[i] = mensajeNumClave[0]; //indicamos que el proceso i trabaja con la clave obtenida
        clavesEncontradas[ mensajeNumClave[0] ] += -1; // por optimización reducimos 1, si entendiamos que -1 era que no se habia encontrado, podemos entender -x como que x procesos (x-1 en realida) estan con esa tarea, por optimizacion
        MPI_Send(mensajeNumClave, 1, tipo_mensajeNumClave, i, MPI_ANY_TAG, MPI_COMM_WORLD); //no bloqueante, clave n (i-j) a proceso i
      }

      nClavesEncontradas = 0;
      while (nClavesEncontradas < PASSWORDS)
      {
        MPI_Recv(mensajeNumClave, 1, tipo_mensajeNumClave, MPI_ANY_SOURCE/*global*/, MPI_ANY_TAG, MPI_COMM_WORLD, &status); //bloqueante, esperamos recibir clave desencriptada

        numClaveDesencriptada = mensajeNumClave[0];
        procesoDesencriptador = status.MPI_SOURCE;/*proceso del que se ha recibido*/
        //manejar repeticiones

        if(numClaveDesencriptada >= 0 && numClaveDesencriptada < PASSWORDS){//recibimos n clave desencriptada, no se si habria que comprobar si ya ha sido resuelta previamente

        //1. marcamos esa contraseña como desencriptada, dos opciones dependiendo de implementacion:
          //marcarDesencriptada(keyList, clavesEncontradas, claveDesencriptada, procesoDesencriptador); //si se envia la clave en vez de n clave
          clavesEncontradas[numClaveDesencriptada] = procesoDesencriptador; //la marcamos como resuelta //si se envia el num clave en vez de la clave
          nClavesEncontradas++;
          //faltaría manejar repeticiones

        //2. Mandamos recados los procesos que estaban trabajando con la clave desencriptada, estos pasarian a desencriptar la nueva clave
          for(i=1; i<iNumProcs; i++)
          {
            if(procAsignadoA[i] == numClaveDesencriptada)
            {
              mensajeNumClave[0] = obtenerClaveADesencriptar(clavesEncontradas);
              MPI_Isend( mensajeNumClave, 1, tipo_mensajeNumClave, i, MPI_ANY_TAG, MPI_COMM_WORLD, &request); //no bloqueante, clave por descifrar a i

              procAsignadoA[i] = mensajeNumClave[0];
              clavesEncontradas[ mensajeNumClave[0] ] += -1; // por optimización reducimos 1 ... vease anterior cE[] += -1
            }
          }
        } //fi

        printMonitor(keyList, clavesEncontradas);
      } //while

    //finalizamos
      clock_end = MPI_Wtime();
      tiempoTotal = (clock_end - clock_start);// / (double) CLOCKS_PER_SEC;
      printResultados(repeticiones, tiempoTotal);
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

      while(repeticiones<(MAX_RAND*2)) { //por seguridad he puesto un límite, pero seria bucle infinito
        MPI_Recv( mensajeNumClave, 1, tipo_mensajeNumClave, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        nClaveADesencriptar = mensajeNumClave[0];
        claveDesencriptada = desencriptarClave( keyList[ mensajeNumClave[0] ][1], &repeticiones);

        //Si la clave ha sido desencriptada enviamos, en caso contrario (ha sido resuelta por otro) volvemos al recv a recibir la nueva a desencriptar
        if(strcmp(claveDesencriptada, "cancel") == 0){
          continue;
        } else {
          mensajeNumClave[1] = repeticiones;
          MPI_Send(mensajeNumClave, 1, tipo_mensajeNumClave, 0, MPI_ANY_TAG, MPI_COMM_WORLD);
        }

      }
      break;
    } //default
  } //case

  MPI_Finalize();
  return;
}

/*
 * Llena una matriz string 20x2 aleatoriamente: clave / clave encriptada
 */
void generarKeyList(char keyList[PASSWORDS][2][CRYPTED_LENGTH]){
  int i;
  int randNum;

  for(i=0; i<PASSWORDS; i++){
    randNum =  rand()%MAX_RAND;
    //para evitar que se sustituya 0X por X
    if(randNum > MIN_RAND){
      sprintf(keyList[i][0], "%d", randNum);
    }
    else {
      sprintf(keyList[i][0], "%d", randNum+MIN_RAND);
      keyList[i][0][0] = '0';
    }

    strcpy(keyList[i][1], crypt(keyList[i][0], SALT));
  }

}

// Imprime las claves
void printClaves(char keyList[PASSWORDS][2][CRYPTED_LENGTH], int seed){
  int i;
    printf("\n--Claves generadas con semilla %d--\n\tRango: %d - %d\n", seed, 0, MAX_RAND);
  for(i=0; i<PASSWORDS; i++){
    printf("%02d) %s %s\n", i, keyList[i][0], keyList[i][1]);
  }
}

void printMonitor(char keyList[PASSWORDS][2][CRYPTED_LENGTH], int clavesEncontradas[PASSWORDS]){
  int i;
  printf("\n---------------------\n");
  for(i=0; i<PASSWORDS; i++){
    printf("%02d) %s %s -> (%d)\n", i, keyList[i][0], keyList[i][1], clavesEncontradas[i]);
  }
}

void printResultados(int repeticiones, double tiempoTotal){
  printf("\nTerminado\n \tRepetciones: %d \n \tTiempo: %f \n\n", repeticiones, tiempoTotal);
}

/*
 * Devuelve el n de una clave que no ha sido encontrada todavía
 * Elige la que menos procesos tenga asignada esto es
 * la que mayor numero negativo tenga
 * (-1 son 0 procesos asignados, -3 son 2 procesos asignados)
 */
int obtenerClaveADesencriptar(int clavesEncontradas[PASSWORDS]){
  int i;
  int claveConMenorNumProcesos=0;

  for(i=0; i<PASSWORDS; i++){
    if( clavesEncontradas[i] < 0){ //de las negativas
      if(clavesEncontradas[i] > clavesEncontradas[claveConMenorNumProcesos]) //la mayor
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
    MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &statusInterrupcion);
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
      //printf("He encontrado la clave: %s que se encriptaba como: %s\n",claveDesencriptada, claveEncriptada);
      return claveDesencriptada;
    }
  }

  return claveDesencriptada;
}

/*
 * Dada una claveDesencriptada, busca la clave en la keyList que coincida y añade el proceso que la descubrió a clavesEncontradas
 */
void marcarDesencriptada(char keyList[PASSWORDS][2][CRYPTED_LENGTH], int clavesEncontradas[PASSWORDS],char claveDesencriptada[KEY_LENGTH+1], int proceso){
  int i;

  for(i=0; i<PASSWORDS; i++){
    if(strcmp(keyList[i][0], claveDesencriptada) == 0){
      clavesEncontradas[i]=proceso;
      return;
    }
  }
}
