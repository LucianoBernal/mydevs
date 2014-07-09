#ifndef PCP__H_
#define PCP__H_

#include <semaphore.h>
#include <pthread.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>

typedef struct {
	t_PCB* pcb;
	int IDCpu;
	char razon;
	int dispositivoIO;
	int tiempo;
} t_paquete_recibir_CPU;

typedef struct {
	t_PCB* pcb;
	int IDCpu;
} t_paquete_enviar_CPU;


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

typedef struct {
	int idCPU;
	int estado;
} t_estructuraCPU;

#endif /* PCP__H_ */
