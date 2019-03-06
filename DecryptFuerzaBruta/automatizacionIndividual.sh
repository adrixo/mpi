
PRIMERO='decryptFuerzaBruta_V1.c'
P_EJECUTABLE='ejecutable_V1.exe'

mpicc $PRIMERO -o $P_EJECUTABLE -lcrypt

N_PROCESOS=(2 3 4 5 6 7)

for i in ${N_PROCESOS[@]}; do
  mpirun -np ${i} $P_EJECUTABLE
done
