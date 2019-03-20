
# Asingar variables de lo que se quiera ejecutar y comparar
# Tened en cuenta que la V1 el padre no realiza accion y con 1 proceso hay espera infinita, descomentar linea anterior bucle

#Parametros a configurar:
# "Padre" "Padre_Asincrono" "V1" "V1_Asincrono"
NOMBRE1="V1_Asincrono"
NOMBRE2="Padre_Asincrono"
REPETICIONES=10
N_PROCESOS=(2 3 4 5 6 7 8 9) #cuiudado con 1 proceso si el padre no actua

#Los siguientes se deben modificar tambien los #define en los fuentes
CLAVES=4
LONGITUD=6

# 1. EJECUCION
PRIMERO="decryptFuerzaBruta_${NOMBRE1}.c"
SEGUNDO="decryptFuerzaBruta_${NOMBRE2}.c"
P_EJECUTABLE="ejecutable_${NOMBRE1}.exe"
S_EJECUTABLE="ejecutable_${NOMBRE2}.exe"

mpicc $PRIMERO -o $P_EJECUTABLE -lcrypt
mpicc $SEGUNDO -o $S_EJECUTABLE -lcrypt

#for j in $(seq 1 1 $REPETICIONES); do # Utilizar para comparar padre vs V1
#  mpirun -np 1 $S_EJECUTABLE
#done
#for i in ${N_PROCESOS[@]}; do
#  for j in $(seq 1 1 $REPETICIONES); do
#    mpirun -np ${i} $P_EJECUTABLE
#    mpirun -np ${i} $S_EJECUTABLE
#  done
#done

#2. Transformamos a formato gnplot e imprimimos
#En caso de querer cambiar el numero de claves, podemos hacerlo desde aqui
#el formato es nClaves x Longitud clave + nombre.gpi p.e ${CLAVES}x${LONGITUD},,,
DATOS1="${CLAVES}x${LONGITUD}_claves_${NOMBRE1}.dat"
DATOS2="${CLAVES}x${LONGITUD}_claves_${NOMBRE2}.dat"

./TransformaGNUPLOT.py $DATOS1
./TransformaGNUPLOT.py $DATOS2

GPI1_TIEMPO="${CLAVES}x${LONGITUD}_claves_${NOMBRE1}_Tiempo.gpi"
GPI2_TIEMPO="${CLAVES}x${LONGITUD}_claves_${NOMBRE2}_Tiempo.gpi"
GPI1_REPETICIONES="${CLAVES}x${LONGITUD}_claves_${NOMBRE1}_Repeticiones.gpi"
GPI2_REPETICIONES="${CLAVES}x${LONGITUD}_claves_${NOMBRE2}_Repeticiones.gpi"

gnuplot --persist -e "plot \"$GPI1_TIEMPO\" title \"$NOMBRE1\" with linespoints linestyle 1 , \"$GPI2_TIEMPO\" title \"$NOMBRE2\" with linespoints linestyle 7"
gnuplot --persist -e "plot \"$GPI1_REPETICIONES\" title \"$NOMBRE1\" with linespoints linestyle 1 , \"$GPI2_REPETICIONES\" title \"$NOMBRE2\" with linespoints linestyle 7"
