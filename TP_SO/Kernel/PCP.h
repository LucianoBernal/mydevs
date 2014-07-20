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


extern t_queue* colaReady;
extern sem_t  colaReadyMutex;
extern sem_t  vacioReady;
t_dictionary* diccionarioSemaforos;
sem_t diccionarioDispositivosMutex;
sem_t diccionarioSemaforosMutex;
sem_t CPUsLibres;
sem_t sBloqueado;
sem_t colaExecVacia;
sem_t semCPUDesconectadaMutex;
sem_t CPUsMutex;
sem_t colaExecMutex;
//sem_t semaforosMutex; //TODO
t_list* colaExec;
t_queue* colaIntermedia;
sem_t colaIntermediaMutex;
sem_t colaIntermediaVacia;
pthread_t ejecutar;
pthread_t multiplexorCPUs;
pthread_t envCPU;
int retMandarAEjecutar, retEnviarCPU, retMultiplexorCPUs;
int* sinParametros;
t_dictionary* diccionarioDispositivos;
t_list* CPUs;
int idUltimaCPUDesconectada;
sem_t mostarColasMutex;
int socketUMV;
sem_t  grado_Multiprogramacion;
char* ip_UMV;
char* puerto_UMV;
char* puerto_programa;
char* puerto_CPU;
int quantum;
int retardo;
int tamanio_stack;
int multiprogramacion;
t_queue* colaReady;
sem_t  colaReadyMutex;
sem_t  vacioReady;
sem_t mutexVG;
sem_t colaExitVacio;
sem_t colaExitMutex;
t_queue* colaExit;
int cantidadDeDispositivos;
int cantidadDeSemaforos;
t_list* valor_semaforos;
t_list* semaforos;
t_list* retardos;
t_list* idDispositivos;
extern t_log *logKernel;



#endif /* PCP__H_ */
