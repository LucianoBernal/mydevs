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

extern sem_t victimasMutex;
extern t_list* victimas;
extern t_queue* colaReady;
extern sem_t  colaReadyMutex;
extern sem_t  vacioReady;
extern t_dictionary* diccionarioSemaforos;
sem_t diccionarioDispositivosMutex;
extern sem_t diccionarioSemaforosMutex;
sem_t CPUsLibres;
sem_t sBloqueado;
sem_t colaExecVacia;
sem_t semCPUDesconectadaMutex;
sem_t CPUsMutex;
sem_t colaExecMutex;
//sem_t semaforosMutex; //TODO
t_list* colaExec;
pthread_t ejecutar;
pthread_t multiplexorCPUs;
int retMandarAEjecutar, retMultiplexorCPUs;
int* sinParametros;
t_dictionary* diccionarioDispositivos;
t_list* CPUs;
extern int idUltimaCPUDesconectada;
extern int quantum;
extern int retardo;
extern sem_t colaExitVacio;
extern sem_t colaExitMutex;
extern t_queue* colaExit;
extern int cantidadDeDispositivos;
extern int cantidadDeSemaforos;
extern t_list* valor_semaforos;
extern t_list* semaforos;
extern t_list* retardos;
extern t_list* idDispositivos;
extern t_log *logKernel;




#endif /* PCP__H_ */
