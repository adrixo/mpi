// Ejemplo 1: MPI_Send , MPI_Recv. Nombre de hosts

# define TAMCADENA 100
# include "mpi.h"
# include <stdio.h>

void Obten_Nombre_Procesos(int id, int numprocs)
{
	char nombreproc [MPI_MAX_PROCESSOR_NAME];	
	int lnombreproc; 	
	int etiqueta=50;
	MPI_Status status;
	int origen;

	MPI_Get_processor_name(nombreproc,& lnombreproc);
	
	if (id==0)
	{
		fprintf(stdout,"Proceso %d en %s\n", id, nombreproc);
		
		for (origen=1; origen<numprocs;origen++)
		{
	//		MPI_Recv(nombreproc,MPI_MAX_PROCESSOR_NAME, MPI_CHAR, origen, etiqueta, MPI_COMM_WORLD, &status);
			MPI_Recv(nombreproc,MPI_MAX_PROCESSOR_NAME, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			fprintf(stdout,"Proceso %d en %s\n", status.MPI_SOURCE, nombreproc);
		}
	}
	else
	{
		MPI_Send(nombreproc, MPI_MAX_PROCESSOR_NAME, MPI_CHAR, 0, etiqueta, MPI_COMM_WORLD);
	}
}

int main (int argc , char **argv) {
	int id ; 
	int numprocs; 
	
	double tmpinic = 0.0; 
	double tmpfin; 
	

	MPI_Init (&argc ,&argv); 						
	MPI_Comm_rank(MPI_COMM_WORLD,&id ); 			
	MPI_Comm_size(MPI_COMM_WORLD, & numprocs ); 	
	
	tmpinic=MPI_Wtime(); 
	
	Obten_Nombre_Procesos(id, numprocs);
	
	tmpfin =MPI_Wtime(); 	

	if (id==0) { 
		fprintf(stdout, "Numero Procesos: %d \n" , numprocs) ;
		fprintf(stdout, "Tiempo Procesamiento: %f \n\n" , tmpfin-tmpinic);
	}

	MPI_Finalize(); 
	return 0 ;
}
s
