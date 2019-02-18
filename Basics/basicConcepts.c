#include <mpi.h>


/*
 * Funciones básicas:
 *
  MPI_Init -> Inicialización de MPI
  MPI_Finalize -> Termina MPI
  MPI_Comm_size -> determina el número de procesos pertenecientes al grupo asociado a un comunicador
  MPI_Comm_rank -> devuelve el identificador de un proceso dentro de un comunicador
  MPI_Send -> envía un mensaje
  MPI_Recv -> Recibe un mensaje
  *
  */

/* compilacion con openMPI:
 * mpicc MPI_hello.c -o MPI_hello
 *
 * Ejecucion en 4 procesos en un sistema monoprocesador:
 * mpirun -np 4 MPI_hello
 *
 */

/*
 * Otras funciones:
 *
 int MPI_Get_processor_name(char * nombre, int* longnombre);
 double MPI_Wtime();

//Bloqueantes
 int MPI_Send(void * mensaje, int contador, MPI_Datatype tipo_datos, int destino, int etiq, MPI_Comm com);
 int MPI_Recv(void * mensaje, int contador, MPI_Datatype tipo_datos, int origen, int etiqueta, MPI_Comm com, MPI_Status* status);
//MPI_Datatype = MPI_INT

//NO Bloqueantes
 int MPI_Isend(void* mensaje, int contador, MPI_Datatype tipo_datos, int destino, int etiq, MPI_Comm com, MPI_Request* peticion);
 int MPI_Irecv(void* mensaje, int ocntador, MPI_Datatype tipo_datos, int origen, int etiqueta, MPI_Comm com, MPI_Request* peticion);

 //Esperar a que se complete un envio o una recepción y comprobar:
 int MPI_Wait(MPI_Request* peticion, MPI_Status* status);
 int MPI_Test(MPI_Request* peticion, int *flag, MPI_Status* status);
 int MPI_Waitany(int contador, MPI_Request* vector_peticiones, int*indice, MPI_Status* status); //cualquiera en un vector
 int MPI_Waitall(int contador, MPI_Request* vector_peticiones, MPI_Status* status);
 //comprobar si hay mensajes pendientes de ser recibidos:
 int MPI_Probe(int origen, int etiqueta, MPI_Comm comm, MPI_Status *status);
 int MPI_Iprove(int origen, int etiqueta, MPI_Comm comm, int *flag, MPI_Status *status);

 *
 */


/*
Agrupacion de datos de MPI

typedef struct {
  long double radio;
  long nmuestras_local;
} Tipo_Datos_Entrada;

  int MPI_Type_struct(int contador, int longitudes_bloque[], MPI_aint indices[], MPI_Datatype antiguos_tipos_datos, MPI_Datatype *nuevo_tipo);
  int MPI_Type_commit(MPI_Datatype* tipo_datos_MPI);
  int MPI_Type_vector(int contador, int longitud_bloque, int salto, MPI_Datatype tipo_datos_elem, MPI_Datatype* nuevo_tipo);
*/

/*
 * Comunicacion colectiva:
 * aquellas que se aplican al mismo tiempo a todos los procesos pert. a un comunicador
 *
 int MPI_Bcast(void* mensaje, int contador, MPI_Datatype tipo_datos, int raiz, MPI_Comm com);

 //Reduccion: forman un subconjunto de las op. de com. colectiva
 int MPI_Reduce(void* operando, void* resultado, int contador, MPI_Datatype tipo_datos, MPI_Op operacion, int raiz, MPI_Comm com);

 int MPI_Barrier(MPI_Comm comm);

 MPI_Scatter();
 MPI_Gather();
 *
 */

 /*
  * Comunicación por grupos:
  *
  int MPI_Comm_group(MPI_Comm com, MPI_Group* grupo); //retorna el grupo asociado a un comm det.
  int MPI_Comm_create(MPI_Comm antiguo_com, MPI_Group nuevo_grupo, MPI_Comm * nuevoCom); //crea un nuevo comunicador a partir de otro
  int MPI_Comm_split(MPI_Comm antiguo_com, int clave_particion, int clave_id, MPI_Comm* nuevo_com); //crea varios com simultaneamente
  ... MPI_Group_incl()...

  //TOPOLOGIA
  int MPI_Cart_create(MPI_Comm antiguo_com, int numero_dimensiones, int* tam_dimensiones, int*periodicidades, int reordenamiento, MPI_Comm* com_cartesiano); //crea un nuevo comunicador de topología cartesiana.
  int MPI_Cart_coords(MPI_Comm cart_com_ int id, int numero_dimensiones, int* coordenadas); //Retorna las coordenadas de un proc. en un com. de top. cartesiana.
  int MPI_Cart_rank(MPI_Comm cart_com, int* coordenadas, int* id); //retorna el id de un proc en com de top cart
  int MPI_Cart_sub(MPI_Comm cart_com, int*var_coords, MPI_Comm nuevo_com); //Divide un com rejilla en rejillas menores
  *
  */

/*
 * Tipos de datos derivados:
 *
 Contiguous:
  Un tipo de datos consistente en la replicación de un tipo de dato básico en pos. de mem. contiguas
  (No aporta mucho frente a la forma tradicional)
  int MPI_Type_contiguous(int count, MPI_Datatype oldtype, MPI_Datatype *newtype);
  
 Vector:
  Permite definir tipos con elementos equiespaciados de un array.
  int MPI_Type_vector(int count, int blocklength, int stride, MPI_Datatype oldtype, MPI_Datatype* newtype);

 Indexados:
  Permiten definir tipos con elementos arbitrarios de un array
  Elementos no equiespaciados, de modo que las distancias entre ellos son ahora un vector distancias
  cada elemento tiene un num. dist de entradas. cada entrada es también un vector
  int MPI_Type_indexed(int count, int * array_of_blocklengths, int * array_of_displacements, MPI_Datatype oldtype, MPI_Datatype * newtype)

 Correspondencia de tipos:
  es posible recibir datos de un tipo distinto al que se envía.
 *
 */
main(int argc, char** argv){
  int myrank, nprocs;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  MPI_Finalize(MPI_COMM_WORLD, &myrank);

  MPI_Finalize();
}
