#ifndef PCP__H_
#define PCP__H_

#include "Kernel.h"
#include "multiplexorCPUs_interfaz.h"

typedef struct {
	t_PCB* pcb;
	int IDCpu;
	char razon;
	int dispositivoIO;
	int tiempo;
} t_paquete_recibir_CPU;

typedef struct {
	int retardo;
	sem_t colaVacia;
	t_queue* procesosBloqueados;
	sem_t mutexCola;
} t_estructuraDispositivoIO;

typedef struct {
	int valor;
	t_queue* procesosBloqueados;
	sem_t mutexCola;
} t_estructuraSemaforo;

//typedef struct
// {
// t_PCB* pcb;
// int tiempo;
// } t_estructuraProcesoBloqueado;
//
//typedef struct {
// int idCPU;
// int estado;
// int idProceso;
// } t_estructuraCPU;

t_dictionary* diccionarioSemaforos;
sem_t diccionarioDispositivosMutex;
sem_t diccionarioSemaforosMutex;
sem_t CPUsLibres;
sem_t sBloqueado;
sem_t colaExecVacia;
sem_t semCPUDesconectadaMutex;
sem_t CPUsMutex;
sem_t colaExecMutex;
sem_t semaforosMutex; //TODO
t_queue* colaExec;
pthread_t ejecutar;
pthread_t multiplexorCPUs;
pthread_t envCPU;
int retMandarAEjecutar, retEnviarCPU, retMultiplexorCPUs;
int* sinParametros;
t_dictionary* diccionarioDispositivos;
t_list* CPUs;
int idUltimaCPUDesconectada;

#endif /* PCP__H_ */
