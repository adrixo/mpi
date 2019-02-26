#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>




long double Opera(unsigned long lNumMuestras, int iNumProcs, int iId)
{
	unsigned long l;
	unsigned long li;
	unsigned long lf;
	unsigned long lAciertos=0L;
	long double fPI=0.0;
	
	double x,y;
	double hip;
	
	li=(lNumMuestras/iNumProcs)*iId+1;
	lf=li+(lNumMuestras/iNumProcs)-1;
	
	//printf("Id: %d  -> %ld %ld\n",iId,li,lf);
	//fflush(stdout);
	
	srand(iId);
	
	for (l=li;l<=lf;l++)
	{
		x=(double)rand()/(double)RAND_MAX;
		y=(double)rand()/(double)RAND_MAX;
		
		hip=x*x+y*y;
		
		if (hip<1.0)
			lAciertos++;
		
		//printf("x: %f, y: %f, hip: %f, aciertos: %ld\n",x,y,hip,lAciertos);
	}
	
	fPI=(double)(4.0*lAciertos)/(double)(lNumMuestras/iNumProcs);
	
	//printf("Id: %d  -> %Lf\n",iId,fPI);
	//fflush(stdout);
	
	return fPI;
}

long double Opera1(unsigned long lNumMuestras, int iNumProcs, int iId)
{

	unsigned long l;
	unsigned long lAciertos=0L;
	long double fPI=0.0;
	
	long double x,y;
	long double hip;
	
	srand(iId);
	
	for (l=0;l<lNumMuestras;l++)
	{
		x=(long double)rand()/(long double)RAND_MAX;
		y=(long double)rand()/(long double)RAND_MAX;
		
		hip=x*x+y*y;
		
		if (hip<1.0)
			lAciertos++;
	}
	
	fPI=(long double)(4.0*lAciertos)/(long double)(lNumMuestras);
	
	return fPI;
}

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
			MPI_Recv(nombreproc,MPI_MAX_PROCESSOR_NAME, MPI_CHAR, origen, etiqueta, MPI_COMM_WORLD, &status);
			fprintf(stdout,"Proceso %d en %s\n", origen, nombreproc);
		}
	}
	else
	{
		MPI_Send(nombreproc, MPI_MAX_PROCESSOR_NAME, MPI_CHAR, 0, etiqueta, MPI_COMM_WORLD);
	}
}



#define MSG_RESULTADO 	50
#define MSG_DATOS		55


int main (int argc , char **argv) {
	int iId ; 
	int iNumProcs; 
	double tmpInic = 0.0; 
	double tmpFin; 
	unsigned long lNumMuestras;
	long double fParcial;
	long double fTotal;

	MPI_Init (&argc ,&argv); 						
	MPI_Comm_rank(MPI_COMM_WORLD,&iId ); 			
	MPI_Comm_size(MPI_COMM_WORLD, & iNumProcs ); 	
	
	// Para saber donde se ejecuta cada cosa
	Obten_Nombre_Procesos(iId, iNumProcs);
	
	tmpInic=MPI_Wtime(); 

	
	// El id=0 obtiene los datos 
	if (iId==0)
	{
		lNumMuestras=100L;
		lNumMuestras=lNumMuestras/iNumProcs;
	}	
	
	// Los envía a todos
	MPI_Bcast(&lNumMuestras,1,MPI_UNSIGNED_LONG, 0,MPI_COMM_WORLD);

	// Calcula
	fParcial=Opera1(lNumMuestras,iNumProcs,iId);
	
	// Recibe/Manda resultados
	MPI_Reduce(&fParcial,&fTotal,1,MPI_LONG_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);
	
	// Realiza el cálculo final
	if (iId==0)
	{
		fTotal=fTotal/iNumProcs;
	}
	
	tmpFin =MPI_Wtime(); 	
	
	if (iId==0) { 
		fprintf(stdout,"PI = %0.50Lf\n",fTotal);
		fprintf(stdout, "Numero Procesos: %d \n" , iNumProcs) ;
		fprintf(stdout, "Tiempo Procesamiento: %f \n\n" , tmpFin-tmpInic);
	}
	MPI_Finalize(); 
	return 0;
}

