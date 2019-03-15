
#Parametros a configurar:
NOMBRE1="Padre"
REPETICIONES=1
N_PROCESOS=(1 2 3 4 5 6 7 8 16 32 64 128 256)
CLAVES=20 #Se deben modificar tambien los #define en los fuentes
LONGITUD=6

# 1. EJECUCION
PRIMERO="decryptFuerzaBruta_${NOMBRE1}.c"
P_EJECUTABLE="ejecutable_${NOMBRE1}.exe"

mpicc $PRIMERO -o $P_EJECUTABLE -lcrypt

#mpirun -np 1 $S_EJECUTABLE
for i in ${N_PROCESOS[@]}; do
  for j in $(seq 1 1 $REPETICIONES); do
    mpirun -np ${i} -host localhost $P_EJECUTABLE
  done
done

DATOS1="${CLAVES}x${LONGITUD}_claves_${NOMBRE1}.dat"

./TransformaGNUPLOT.py $DATOS1

GPI1_TIEMPO="${CLAVES}x${LONGITUD}_claves_${NOMBRE1}_Tiempo.gpi"
GPI1_REPETICIONES="${CLAVES}x${LONGITUD}_claves_${NOMBRE1}_Repeticiones.gpi"

gnuplot --persist -e "plot \"$GPI1_TIEMPO\" title \"$NOMBRE1\" with linespoints linestyle 1"
gnuplot --persist -e "plot \"$GPI1_REPETICIONES\" title \"$NOMBRE1\" with linespoints linestyle 1"
