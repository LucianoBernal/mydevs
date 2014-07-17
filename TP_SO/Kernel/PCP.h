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

/*typedef struct
 {
 t_PCB* pcb;
 int tiempo;
 } t_estructuraProcesoBloqueado;*/

/*typedef struct {
 int idCPU;
 int estado;
 int idProceso;
 } t_estructuraCPU;*/

sem_t CPUsLibres;
//static sem_t sPLP;
sem_t sBloqueado;
sem_t colaExecVacia;
sem_t semCPUDesconectadaMutex;
sem_t CPUsMutex; //TODO
sem_t colaExecMutex; //TODO
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
