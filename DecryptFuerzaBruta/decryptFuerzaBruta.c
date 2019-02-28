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
 
  MPI_Init (&argc ,&argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &iId);
  MPI_Comm_size(MPI_COMM_WORLD, &iNumProcs);


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
            /* entendido como otra columna de keyList almacena -1 si una clave no ha sido encontrada y nProceso que la encontró en caso contrario. 
             * p.e: cE[2] = 4 <-> La clave 3 fue encontrada por el proceso 4 
             * Podría tambien ponerse -x siendo x el numero de procesos tratando de desencriptar una clave
             */

      for(i=0; i<PASSWORDS; i++){
        clavesEncontradas[i]=-1;
      }

      int nClavesEncontradas = 0; //nClaves encontradas
      int ultimaClave=0;
      int procAsignadoA[iNumProcs]; //Cada proceso i se asigna a una clave a desencriptar, -1 si esta inactivo.

    //generacion claves
      //generarKeyList(keyList); //si eso lo realizamos en todo los procesos con la misma seed tendriamos la misma clave para todos y no necesitariamos el paso de mensajes
      printClaves(keyList, seed);

//!! A partir de aqui esta pseudocodigo, creo que falta una forma de indicar que n clave esta descifrando y los tipos de datos de comunicacion 
    //busqueda de Claves
      //Inciiamos con una tarea para todos los procesos (habria que ver mas tarde si nprocesos > nClaves, p. ej: uso de j para ciclos)
      j = 0;
      for(int i=1; i<iNumProcs; i++){
        if(i>=PASSWORDS)
          j = PASSWORDS; //si i es mayor que el n procesos empezariamos a asignar claves otra vez desde la n 0
        MPI_Send(clave n (i-j) a proceso i); //no bloqueante
        procAsignadoA[i] = nClave;
      }

      nClavesEncontradas = 0;
      do {
        MPI_Recv(esperamos recibir claveDesencriptada) //bloqueante

        if(recibimos claveDesencriptada){//no se si habria que comprobar si ya ha sido resuelta previamente
        //1. desactivamos los procesos que esten con esa tarea
          for(i=1; i<iNumProcs; i++){
            if(procAsignadoA[i]==nClaveDesencriptada){
              MPI_ISend(interrupcion proceso i); //no bloqueante
              procAsignadoA[i] = -1; //lo desasignamos
            }
          }

        //2. marcamos esa contraseña como desencriptada
          marcarDesencriptada(keyList, clavesEncontradas, claveDesencriptada, //proceso que la desencripto);
          nClavesEncontradas++;

        //3. Mandamos nuevos recados a los procesos que quedaron inactivos
          for(i=1; i<iNumProcs; i++){
            if(procAsignadoA[i]==-1){
              clave por descifrar = obtenerClaveADesencriptar(clavesEncontradas);
              MPI_ISend( clave por descifrar a i ); //no bloqueante
              procAsignadoA[i] = clave por descifrar;
              clavesEncontradas[clave por descifrar] += -1;
            }
          }
        }
        printMonitor(keyList, clavesEncontradas);

      } while (nClavesEncontradas < PASSWORDS);

    //finalizamos
      clock_end = MPI_Wtime();
      tiempoTotal = (clock_end - clock_start);// / (double) CLOCKS_PER_SEC;
      printResultados(repeticiones, tiempoTotal);
      break;
    }

    default: //hijos
    {
      srand(iId*100); //aqui iniciamos el seed rand por ejemplo
      do {
        repeticiones++;
        MPI_Recv(clave, 0);
        claveDesencriptada = desencriptarClave(clave, &repeticiones);
        MPI_ISend(clave encontrada,0);
      } while(repeticiones<(MAX_RAND*2)); //manejar interrupciones y ver cuando acaba proceso main !!!!!!!!
      break;
    }
  }

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
 * Para evitar demasiados menajes podia ser mejorado evitando 
 * que fuese tan secuencial.
 */
int obtenerClaveADesencriptar(int clavesEncontradas[PASSWORDS]){
  int i;

//Mejora, primero los que no estan siendo resueltos por ninguno
  for(i=0; i<PASSWORDS; i++){
    if(clavesEncontradas[i] == -1){
      return i;
    }
  }

  for(i=0; i<PASSWORDS; i++){
    if(clavesEncontradas[i] < -1){
      return i;
    }
  }

  return -1;
}

/*
 * Dada una clave encriptada, mediante fuerza bruta va encriptando posibles claves y comparando
 * La semilla de rand se tiene que seleccionar previamente.
 *
 * Devuelve la clave que al encriptarse coincide.
 */
char * desencriptarClave(char claveEncriptada[CRYPTED_LENGTH], int *repeticiones){
  char * claveDesencriptada;
  claveDesencriptada = malloc((KEY_LENGTH+1)*sizeof(char));
  int randNum;

  while(1){
    (*repeticiones)++;

    randNum =  rand()%MAX_RAND;
    //para evitar que se sustituya 0X por X
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
