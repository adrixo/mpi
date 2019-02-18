#include <mpi.h>

/*
 * Proceso para crear un nuevo comunicador enfocado a comunicación colectiva.
 */
main(int argc, char** argv){

  MPI_Group MPI_GROUP_WORLD;
  MPI_GROUP grupo_primera_fila;
  MPI_Comm com_primera_fila;
  int tam_fila;
  int îds_procesos;

  int proc, q=10;

  MPI_Init(&argc, &argv);

  ids_procesos=(int *) malloc(q*sizeof(int));
  for(proc = 0; proc<q; proc++)
    ids_procesos[proc]=proc;

  MPI_Comm_group(MPI_COMM_WORLD, &MPI_GROUP_WORLD);

  //creamos un nuevo grupo
  MPI_Group_incl(MPI_GROUP_WORLD, q, ids_procesos, &grupo_primera_fila);
  //crea el nuevo comunicador
  MPI_Comm_create(MPI_COMM_WORLD, grupo_primera_fila, &com_primera_fila);

  MPI_Finalize();
}
