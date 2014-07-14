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
	sem_t* colaVacia;
	t_queue* procesosBloqueados;
	sem_t* mutexCola;
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

static sem_t * CPUsLibres = NULL;
static sem_t * sPLP = NULL;
static sem_t * sYaInicializoElMT = NULL;
static sem_t * sBloqueado = NULL;
static sem_t * colaExecVacia = NULL;
static sem_t * semCPUDesconectadaMutex = NULL;
//static int laSenialDeTerminar = 0;
static t_queue* colaExec;
static pthread_t ejecutar;
static pthread_t multiplexorCPUs;
static pthread_t envCPU;
static int retMandarAEjecutar, retEnviarCPU, retMultiplexorCPUs;
static int* sinParametros = NULL;
static t_dictionary* diccionarioDispositivos;
static t_list* CPUs;
static int idUltimaCPUDesconectada;

#endif /* PCP__H_ */
