# Introducción a MPI

[MPI ó Message Passing Interface](https://es.wikipedia.org/wiki/Interfaz_de_Paso_de_Mensajes) es un estándar que define la sintaxis y la semántica de las funciones contenidas en una biblioteca de paso de mensajes diseñada para ser usada en programas que exploten la existencia de múltiples procesadores.

El paso de mensajes es una técnica empleada en programación concurrente para aportar sincronización entre procesos y permitir la exclusión mutua.

Su principal característica es que no precisa de memoria compartida, por lo que es muy importante en la programación de sistemas distribuidos.


## Index

  - [Funciones Básicas](#Architecture)
  - [Prerequisites](#Prerequisites)
  - [Problems](#Problems)
  - [Authors](#Authors)

## Ejecucion
compilacion con openMPI:

```
  mpicc MPI_hello.c -o MPI_hello
```
Ejecucion en 4 procesos en un sistema monoprocesador:
```
  mpirun -np 4 MPI_hello
```

## Funciones básicas

 - MPI_Init -> Inicialización de MPI
 - MPI_Finalize -> Termina MPI
 - MPI_Comm_size -> determina el número de procesos pertenecientes al grupo asociado a un comunicador
 - MPI_Comm_rank -> devuelve el identificador de un proceso dentro de un comunicador
 - MPI_Send -> envía un mensaje
 - MPI_Recv -> Recibe un mensaje

 - int MPI_Get_processor_name(char * nombre, int* longnombre);
 - double MPI_Wtime();

 ![Alt text](Basics/MPI_Datatype.png?raw=true "MPI_Datatype")


## Paso de mensajes
![Alt text](Basics/SEND.png?raw=true "Mensajeria")

**Bloqueantes:**
 - int MPI_Send(void * mensaje, int contador, MPI_Datatype tipo_datos, int destino, int etiq, MPI_Comm com);
 - int MPI_Recv(void * mensaje, int contador, MPI_Datatype tipo_datos, int origen, int etiqueta, MPI_Comm com, MPI_Status* status);

**NO Bloqueantes:**
 - int MPI_Isend(void* mensaje, int contador, MPI_Datatype tipo_datos, int destino, int etiq, MPI_Comm com, MPI_Request* peticion);
 - int MPI_Irecv(void* mensaje, int ocntador, MPI_Datatype tipo_datos, int origen, int etiqueta, MPI_Comm com, MPI_Request* peticion);

 ![Alt text](Basics/MPI_[B|S|R]SEND.png?raw=true "Otras")

**Otras Funciones:**
 - int MPI_Wait(MPI_Request* peticion, MPI_Status* status);
 - int MPI_Test(MPI_Request* peticion, int \*flag, MPI_Status\* status);
 - int MPI_Waitany(int contador, MPI_Request* vector_peticiones, int*indice, MPI_Status* status); //cualquiera en un vector
 - int MPI_Waitall(int contador, MPI_Request* vector_peticiones, MPI_Status* status);
 - int MPI_Probe(int origen, int etiqueta, MPI_Comm comm, MPI_Status \*status);
 - int MPI_Iprove(int origen, int etiqueta, MPI_Comm comm, int \*flag, MPI_Status \*status);

## Agrupaciones de datos
**Estructura MPI:**
```
  typedef struct {
    long double radio;
    long nmuestras_local;
  } Tipo_Datos_Entrada;
```
 - int MPI_Type_struct(int contador, int longitudes_bloque[], MPI_aint indices[], MPI_Datatype antiguos_tipos_datos, MPI_Datatype \*nuevo_tipo);
 - int MPI_Type_commit(MPI_Datatype \* tipo_datos_MPI);
 - int MPI_Type_vector(int contador, int longitud_bloque, int salto, MPI_Datatype tipo_datos_elem, MPI_Datatype* nuevo_tipo);

## Comunicacion colectiva:
![Alt text](Basics/colectiveCommunication.png?raw=true "Esquema")
 - int MPI_Bcast(void* mensaje, int contador, MPI_Datatype tipo_datos, int raiz, MPI_Comm com);
 - int MPI_Reduce(void* operando, void* resultado, int contador, MPI_Datatype tipo_datos, MPI_Op operacion, int raiz, MPI_Comm com);
 ![Alt text](Basics/MPI_Op.png?raw=true "Operaciones de reduccion")
 - int MPI_Barrier(MPI_Comm comm);

 - MPI_Scatter();
 - MPI_Gather();

## Comunicación por grupos:
 - int MPI_Comm_group(MPI_Comm com, MPI_Group* grupo); //retorna el grupo asociado a un comm det.
 - int MPI_Comm_create(MPI_Comm antiguo_com, MPI_Group nuevo_grupo, MPI_Comm * nuevoCom); //crea un nuevo comunicador a partir de otro
 - int MPI_Comm_split(MPI_Comm antiguo_com, int clave_particion, int clave_id, MPI_Comm* nuevo_com); //crea varios com simultaneamente
 - ... MPI_Group_incl()...

**TOPOLOGIA:**
 - int MPI_Cart_create(MPI_Comm antiguo_com, int numero_dimensiones, int* tam_dimensiones, int*periodicidades, int reordenamiento, MPI_Comm* com_cartesiano); //crea un nuevo comunicador de topología cartesiana.
 - int MPI_Cart_coords(MPI_Comm cart_com_ int id, int numero_dimensiones, int* coordenadas); //Retorna las coordenadas de un proc. en un com. de top. cartesiana.
 - int MPI_Cart_rank(MPI_Comm cart_com, int* coordenadas, int* id); //retorna el id de un proc en com de top cart
 - int MPI_Cart_sub(MPI_Comm cart_com, int*var_coords, MPI_Comm nuevo_com); //Divide un com rejilla en rejillas menores

## Tipos de datos derivados:

### Contiguous:
  Un tipo de datos consistente en la replicación de un tipo de dato básico en pos. de mem. contiguas
  (No aporta mucho frente a la forma tradicional)
   - int MPI_Type_contiguous(int count, MPI_Datatype oldtype, MPI_Datatype \*newtype);

### Vector:
  Permite definir tipos con elementos equiespaciados de un array.
   - int MPI_Type_vector(int count, int blocklength, int stride, MPI_Datatype oldtype, MPI_Datatype* newtype);

### Indexados:
  Permiten definir tipos con elementos arbitrarios de un array
  Elementos no equiespaciados, de modo que las distancias entre ellos son ahora un vector distancias
  cada elemento tiene un num. dist de entradas. cada entrada es también un vector
   - int MPI_Type_indexed(int count, int * array_of_blocklengths, int * array_of_displacements, MPI_Datatype oldtype, MPI_Datatype * newtype)

###Correspondencia de tipos:
  Es posible recibir datos de un tipo distinto al que se envía.








## Architecture ##

Proposed map:

![Alt text](doc/Map.png?raw=true "ClassDiagram")

The vehicle sharing service in a city consists of stops and vehicles in these. A system is developed in which several stations will form a network in a hypothetical city; each stop will be an autonomous agent.

On the other hand we have users who use the service, who want to use the vehicles. These are also independent agents, who take the desired vehicles from the stations and move between them.

### Objects and agents classes:

The main objective of the system is the handling of the object vehicle, by which the service that we raised exists; this one is sent between agents by means of messages. In the following image you can see the main classes that make up the system.

![Alt text](doc/classDiagram.png?raw=true "ClassDiagram")


in addition there is a **capsule class** that packs both the vehicle and the various information transmitted by the agents (desired stations, users who wish to reserve a type of vehicle ...)

As we will explain in the [problems section](#Problems), the monitor has a poor design and has not been included in this diagram because I don't want to give too much importance.


## Communication:

**Comunication between user/stations:**
Users move freely on the map made up of stations, therefore they only communicate individually with the station in which they are. The station asked, trying to satisfy the request of a user, may try to communicate with nearby stations to reserve a vehicle.

![Alt text](doc/communicationUserStation.png?raw=true "Comunication between agents")

**Notifications to the monitor:**
Each time an agent performs an action, it notifies the central monitor that manages information about all the agents that make up the ecosystem.

![Alt text](doc/communicationMonitor.png?raw=true "Comunication with monitor")

**Messages:**
- Monitor:
	- Only receive *USERSTATUS_NOTIFICATION* and *STATIONSTATUS_NOTIFICATION* messages from users and stations

- User:
	- **VEHICLE_REQUEST -> Station:** Capsule(null, desiredVehicle, currentStation, null)
	- **VEHICLE_RETURN -> Station:** Capsule(Vehicle, null, null, null)
	- **USERSTATUS_NOTIFICATION -> Monitor:** String("moving currentStation destinationStation vehicleType") or String("waiting|final currentStation")

- Station:
	- **VEHICLE_DELIVERY -> User:** Capsule(Vehicle, null, null, null)
	- **UNSATISFACTORY_REQUEST -> User:** Capsule(vehicle,null,alternativeStation,null) //alternativeStation and/or vehicle can be null
	- **VEHICLE_RESERVATION_REQUEST -> Station:** Capsule(null, desiredVehicle, askingStation , user requesting for a reservation)
	- **VEHICLE_RESERVED -> Station:** void
	- **VEHICLE_NOT_RESERVED -> Station:** void
	- **STATIONSTATUS_NOTIFICATION -> Monitor:** string(List of vehicles)

## Agents Behaviour:
The agents, autonomous programmes of flexible, reactive, proactive and social behaviour, behave according to the cyclic behaviour described in the folder *./VirtualBikeSharingSystem/bin/vehicleShareSystem/behaviours/*
The two main agents involved in the system have been programmed following the structure described in the following diagrams:

- **User behaviour:**

![Alt text](doc/userBehaviour.png?raw=true "User Behaviour")

- **Station behaviour:**

![Alt text](doc/stationBehaviour.png?raw=true "Station Behaviour")

As we see the redirection of messages is quite simple, a someone wanted to add a new functionality or behavior to an agent he could add a new case type, a new column in the diagram.


## Prerequisites ##
This project has been carried out through the platform for the development of JADE agents. This software is contained in this repository. The development and implementation was carried out on the Eclipse platform, this guide assumes its use.

## Using Jade on Eclipse ##

#### How to compile and run tests:

First of all you should run JADE within the main agent, on Eclipse go to "run configurations" -> "choose a java application" -> "pick jade.Boot as main class", after that insert as an argument this line:
```
   -gui Monitor:vehicleShareSystem.agents.AgentMonitor
```

Once this is done, we can instantiate the other agents creating new configurations with diferent arguments:
```
   -main false -host 127.0.0.1 **InstanceName**:vehicleShareSystem.agents.**AgentFile**
```

### Proposed instance
The current configuration allows the instantiation of 4 stations and up to 5 different users, the code can be easily modified to add stations/users or edit the agents' disposition.

**Stations:**
```
    -main false -host 127.0.0.1 StationA:vehicleShareSystem.agents.AgentStation
    -main false -host 127.0.0.1 StationB:vehicleShareSystem.agents.AgentStation
    -main false -host 127.0.0.1 StationC:vehicleShareSystem.agents.AgentStation
    -main false -host 127.0.0.1 StationD:vehicleShareSystem.agents.AgentStation
```
**Users:**
```
    -main false -host 127.0.0.1 User1:vehicleShareSystem.agents.AgentUser
    -main false -host 127.0.0.1 User2:vehicleShareSystem.agents.AgentUser
    -main false -host 127.0.0.1 User3:vehicleShareSystem.agents.AgentUser
    -main false -host 127.0.0.1 User4:vehicleShareSystem.agents.AgentUser
    -main false -host 127.0.0.1 User5:vehicleShareSystem.agents.AgentUser
```

If all the agents run correctly on the main monitor we will see an operation like this:

![Alt text](doc/monitorExample.png?raw=true "ClassDiagram")

In the same way each agent has its own internal dialogue, prompted in its own terminal:

**StationA:**

![Alt text](doc/stationExample.png?raw=true "ClassDiagram")

**User1:**

![Alt text](doc/userExample.png?raw=true "ClassDiagram")


## Problems ##

the monitor agent has a bad design. It was not conceived as something fundamental in the bicycle sharing system but was added later to facilitate the understanding of the operation of the agents.

Although it works, the code is a western spaghetti and does not have to be taken too seriously. A modification or an attempt to add some new functionality can be an arduous task, the best would be to change the system with which the information is displayed.


## Authors ##

* **Adrián Valera Román** - *Architecture designer and main programmer* - [Adrixo](https://github.com/adrixo)
* **Diego Mateos Matilla** - *Architecture designer, bug resolver and tester*

## Acknowledgments

We would like to thank our programming teacher who introduced us to the world of agents:
* Dr. Juan Manuel Corchado - [website](https://corchado.net/)




Developed with java language in eclipse.
