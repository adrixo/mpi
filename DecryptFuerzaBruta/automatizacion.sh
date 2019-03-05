
mpicc decryptFuerzaBruta.c -o ejecutable.exe -lcrypt

N_PROCESOS=(2 3 4 5 7 9)

for i in ${N_PROCESOS[@]}; do
  mpirun -np ${i} ejecutable.exe
done
