#ifndef PCP__H_
#define PCP__H_


#include "Kernel.h"

typedef struct {
	t_PCB* pcb;
	int IDCpu;
	char razon;
	int dispositivoIO;
	int tiempo;
} t_paquete_recibir_CPU;



typedef struct {
	int retardo;
	sem_t* semaforoCola;
	t_queue procesosBloqueados;
} t_estructuraDispositivoIO;

typedef struct
{
	t_PCB* pcb;
	int tiempo;
} t_estructuraProcesoBloqueado;

/*typedef struct {
	int idCPU;
	int estado;
	int idProceso;
} t_estructuraCPU;*/

sem_t * CPUsLibres = NULL;
sem_t * sPLP = NULL;
sem_t * sYaInicializoElMT = NULL;
sem_t * sBloqueado = NULL;
sem_t * colaExecVacia = NULL;
sem_t * semCPUDesconectadaMutex = NULL;
int laSenialDeTerminar = 0;
static t_queue* colaExec = queue_create();
pthread_t ejecutar;
pthread_t multiplexorCPUs;
pthread_t recCPU;
int retMandarAEjecutar, retRecibirCPU, retMultiplexorCPUs;
int* sinParametros = NULL;
static t_dictionary diccionarioDispositivos = dictionary_create();
t_list* CPUs = list_create();
int idUltimaCPUDesconectada;

#endif /* PCP__H_ */
