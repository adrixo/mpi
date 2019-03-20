#!/usr/bin/python

import sys

# Dado un archivo hace una media de los valores para cada n hilos (primer elemento)
#
# esta se guardara con el formato X_[repeticiones|tiempo].dat
#   nprocesos tab [repeticiones|tiempo]
# y posteriormente se utilizara gnuplot para generar la grafica

repeticiones=[]
tiempos=[]

# 1. abrimos el archivo y lo guardamos linea a linea en un array
f = open(sys.argv[1])
next(f)
for line in f:
    ejec = line.rstrip().split(';')
    repeticiones.append([ejec[0],int(ejec[1])])
    tiempos.append([ejec[0],float(ejec[2])])

# 2. calculamos las medias
media_tiempos=[]
nProceso=-1
procesoActual=0
for i in range(len(tiempos)):
    if (nProceso==-1):
        nProceso = tiempos[i][0]
        media_tiempos.append([tiempos[i][0], tiempos[i][1], 1])

    if nProceso == tiempos[i][0]:
        media_tiempos[procesoActual][1] += tiempos[i][1]
        media_tiempos[procesoActual][2] += 1

    if nProceso != tiempos[i][0]:
        procesoActual += 1
        nProceso = tiempos[i][0]
        media_tiempos.append([tiempos[i][0], tiempos[i][1], 1])

for i in range(len(media_tiempos)):
     media_tiempos[i][1] = media_tiempos[i][1]/media_tiempos[i][2]

media_repeticiones=[]
nProceso=-1
procesoActual=0
for i in range(len(tiempos)):
    if (nProceso==-1):
        nProceso = repeticiones[i][0]
        media_repeticiones.append([repeticiones[i][0], repeticiones[i][1], 1])

    if nProceso == repeticiones[i][0]:
        media_repeticiones[procesoActual][1] += repeticiones[i][1]
        media_repeticiones[procesoActual][2] += 1

    if nProceso != repeticiones[i][0]:
        procesoActual += 1
        nProceso = repeticiones[i][0]
        media_repeticiones.append([repeticiones[i][0], repeticiones[i][1], 1])

for i in range(len(media_repeticiones)):
     media_repeticiones[i][1] = media_repeticiones[i][1]//media_repeticiones[i][2]
     media_repeticiones[i][1] = media_repeticiones[i][1]//media_tiempos[i][1]

#. guardamos para el gnuplot
nombre=sys.argv[1].split('.')[0]
f = open(nombre + "_Tiempo.gpi", "w+")
for i in range(len(media_tiempos)):
    linea = "" + media_tiempos[i][0] + "\t" + "{:.4f}".format(media_tiempos[i][1]) + "\n"
    f.write(linea)

nombre=sys.argv[1].split('.')[0]
f = open(nombre + "_Repeticiones.gpi", "w+")
for i in range(len(media_repeticiones)):
    linea = "" + media_repeticiones[i][0] + "\t" + "{:.4f}".format(media_repeticiones[i][1]) + "\n"
    f.write(linea)
