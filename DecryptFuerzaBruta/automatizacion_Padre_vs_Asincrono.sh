
PRIMERO='decryptFuerzaBruta_Padre.c'
SEGUNDO='decryptFuerzaBruta_Padre_Asincrono.c'
P_EJECUTABLE='ejecutable_Padre.exe'
S_EJECUTABLE='ejecutable_Padre_Asincrono.exe'

mpicc $PRIMERO -o $P_EJECUTABLE -lcrypt

mpicc $SEGUNDO -o $S_EJECUTABLE -lcrypt


N_PROCESOS=(2 3 4 5 6 7)

#for i in ${N_PROCESOS[@]}; do
#  mpirun -np ${i} $P_EJECUTABLE
#done

#for i in ${N_PROCESOS[@]}; do
#  mpirun -np ${i} $S_EJECUTABLE
#done

#Ó: Esta si se calienta el ordenador daría mas equidad de resultados
for i in ${N_PROCESOS[@]}; do
  mpirun -np ${i} $P_EJECUTABLE
  mpirun -np ${i} $S_EJECUTABLE
done
