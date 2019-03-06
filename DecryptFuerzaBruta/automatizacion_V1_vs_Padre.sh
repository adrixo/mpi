
PRIMERO='decryptFuerzaBruta_V1.c'
SEGUNDO='decryptFuerzaBruta_Padre.c'
P_EJECUTABLE='ejecutable_V1.exe'
S_EJECUTABLE='ejecutable_Padre.exe'

mpicc $PRIMERO -o $P_EJECUTABLE -lcrypt

mpicc $SEGUNDO -o $S_EJECUTABLE -lcrypt


N_PROCESOS=(2 3 4 5 6 7)
N_PROCESOS_PADRE=(2 3 4 5 6 7)

for i in ${N_PROCESOS[@]}; do
  mpirun -np ${i} $P_EJECUTABLE
done

for i in ${N_PROCESOS_PADRE[@]}; do
  mpirun -np ${i} $S_EJECUTABLE
done

#Ó
#for i in ${N_PROCESOS[@]}; do
#  mpirun -np ${i} $P_EJECUTABLE
#  mpirun -np ${i} $S_EJECUTABLE
#done
