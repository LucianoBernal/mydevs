#include "PCP.h"
#include "PCPinterface.h"

void* pcp_main(void* sinParametro) {
	sinParametros = NULL;
	colaExec = queue_create();
	valor_semaforos = list_create();
	semaforos = list_create();
	retardos = list_create();
	idDispositivos = list_create();
	diccionarioDispositivos = dictionary_create();
	CPUs = list_create();
	sem_init(&CPUsLibres, 0, 0);
	sem_init(&sBloqueado, 0, 0);
	sem_init(&colaExecVacia, 0, 0);
	sem_init(&semCPUDesconectadaMutex, 0, 1);
	sem_init(&CPUsMutex, 0, 1);
	sem_init(&colaExecMutex, 0, 0);

	//crearHilosDeEntradSalida
	int i = 0;
	while (i < cantidadDeDispositivos) {
		int *a = list_get(retardos, i);
		int retardo = *a;
		t_queue* colaDispositivo = queue_create();
		sem_t semaforo;
		sem_t mutex;
		sem_init(&semaforo, 0, 0);
		sem_init(&mutex, 0, 1);
		t_estructuraDispositivoIO* estructuraDispositivo = malloc(
				sizeof(t_estructuraDispositivoIO));
		estructuraDispositivo->retardo = retardo;
		estructuraDispositivo->procesosBloqueados = colaDispositivo;
		estructuraDispositivo->colaVacia = semaforo;
		estructuraDispositivo->mutexCola = mutex;
		char * idDispositivo = list_get(idDispositivos, i);
		dictionary_put(diccionarioDispositivos, idDispositivo,
				estructuraDispositivo);
		pthread_t dispositivo;
		int retIO = pthread_create(&dispositivo, NULL, bloquearYDevolverAReady,
				(void*) &estructuraDispositivo);
		if (retIO) {
			fprintf(stderr, "Error - pthread_create() return code: %d\n",
					retIO);
			exit(EXIT_FAILURE);
		}
		i++;
	}
	crearHilosPrincipales();
	return EXIT_SUCCESS;
}

void crearHilosPrincipales() {
//	retMultiplexorCPUs = pthread_create(&multiplexorCPUs, NULL, atencionCPUs,
//			(void*) sinParametros);
//	if (retMultiplexorCPUs) {
//		fprintf(stderr, "Error - pthread_create() return code: %d\n",
//				retMultiplexorCPUs);
//		exit(EXIT_FAILURE);
//	}
	retMandarAEjecutar = pthread_create(&ejecutar, NULL, mandarAEjecutar,
			(void*) sinParametros);
	if (retMandarAEjecutar) {
		fprintf(stderr, "Error - pthread_create() return code: %d\n",
				retMandarAEjecutar);
		exit(EXIT_FAILURE);
	}
	retEnviarCPU = pthread_create(&envCPU, NULL, enviarCPU,
			(void*) sinParametros);
	if (retEnviarCPU) {
		fprintf(stderr, "Error - pthread_create() return code: %d\n",
				retEnviarCPU);
		exit(EXIT_FAILURE);
	}
	pthread_join(multiplexorCPUs, NULL );
	pthread_join(ejecutar, NULL );
	pthread_join(envCPU, NULL );

}

void* mandarAEjecutar(void* j) {
	while (1) {
		sem_wait(&vacioReady);
		sem_wait(&colaReadyMutex);
		t_PCB* procesoAEjecutar = queue_pop(colaReady);
		sem_wait(&colaExecMutex);
		queue_push(colaExec, procesoAEjecutar);
		sem_post(&colaExecMutex);
		int * sinParametro = NULL;
		enviarCPU(sinParametro);
		free(sinParametro);
		sem_post(&colaReadyMutex);
		sem_post(&colaExecVacia);
	}
}

void* enviarCPU(void* sinParametro) {
	while (1) {
		sem_wait(&CPUsLibres);
		sem_wait(&CPUsMutex);
		int IDCpuLibre = encontrarPrimeraCpuLibreYOcuparla(CPUs);
		sem_post(&CPUsMutex);
		sem_wait(&colaExecMutex);
		t_PCB* paquete = queue_pop(colaExec);
		sem_post(&colaExecMutex);
		printf("%d", IDCpuLibre);
		printf("%d", paquete->indice_de_Codigo);
		//lo hago para sacar el warning por variable sin usar,
		//TODO serializar(paquete);
		int i = posicionEnLaLista(CPUs, IDCpuLibre);
		t_estructuraCPU* estructura = malloc(sizeof(t_estructuraCPU));
		estructura->idCPU = IDCpuLibre;
		estructura->estado = 0;
		estructura->idProceso = (paquete->program_id);
		sem_wait(&CPUsMutex);
		list_replace_and_destroy_element(CPUs, i, estructura,
				(void*) cpu_destroy);
		sem_post(&CPUsMutex);
		//TODO mandar el paquete serializado a esa id (sd)
	}
}

void* bloquearYDevolverAReady(void * param) {
	t_estructuraDispositivoIO* estructura = malloc(
			sizeof(t_estructuraDispositivoIO));
	estructura = (t_estructuraDispositivoIO *) param;
	sem_wait(&(estructura->colaVacia));
	sem_wait(&(estructura->mutexCola));
	t_estructuraProcesoBloqueado* estructuraBloqueada = queue_pop(
			estructura->procesosBloqueados);
	sem_post(&(estructura->mutexCola));
	usleep(estructura->retardo);
	usleep((estructuraBloqueada->tiempo) / 1000); //TODO ver el tema de las unidades
	sem_wait(&colaReadyMutex);
	queue_push(colaReady, estructuraBloqueada->pcb);
	sem_post(&colaReadyMutex);
	return NULL ;
}

int encontrarPrimeraCpuLibreYOcuparla(t_list* lista) {
	t_estructuraCPU* estructura = list_find(lista, (void*) estaLibre);
	estructura->estado = 1;
	return (estructura->idCPU);
}

bool estaLibre(t_estructuraCPU* estructura) {
	return (estructura->estado == 0);
}

void nuevaCPU(int idCPU) {
	t_estructuraCPU* estructura = malloc(sizeof(t_estructuraCPU));
	estructura->idCPU = idCPU;
	estructura->estado = 0;
	estructura->idProceso = -1;
	sem_wait(&CPUsMutex);
	list_add(CPUs, estructura);
	sem_post(&CPUsMutex);
	sem_post(&CPUsLibres);
}

void programaSalioPorQuantum(t_PCB* pcb, int idCPU) {
	sem_wait(&colaReadyMutex);
	queue_push(colaReady, pcb);
	sem_post(&colaReadyMutex);
	seLiberoUnaCPU(idCPU);
}

void moverAColaExit(t_PCB* pcb, int idCPU) {
	sem_wait(&colaExitMutex);
	queue_push(colaExit, pcb);
	sem_post(&colaExitMutex);
	sem_post(&colaExitVacio);
	seLiberoUnaCPU(idCPU);
}

void programaSalioPorBloqueo(t_PCB* pcb, int tiempo, char* dispositivo,
		int idCPU) {
	t_estructuraDispositivoIO* estructura = dictionary_get(
			diccionarioDispositivos, dispositivo);
	t_estructuraProcesoBloqueado* procesoBloqueado = malloc(
			sizeof(t_estructuraProcesoBloqueado));
	procesoBloqueado->pcb = pcb;
	procesoBloqueado->tiempo = tiempo;
	sem_wait(&(estructura->mutexCola));
	queue_push(estructura->procesosBloqueados, procesoBloqueado);
	sem_post(&(estructura->mutexCola));
	sem_post(&(estructura->colaVacia));
	seLiberoUnaCPU(idCPU);
}

void seLiberoUnaCPU(int idCPU) {
	int i = posicionEnLaLista(CPUs, idCPU);
	t_estructuraCPU* estructura = malloc(sizeof(t_estructuraCPU));
	estructura->idCPU = idCPU;
	estructura->estado = 0;
	estructura->idProceso = -1;
	sem_wait(&CPUsMutex);
	list_replace_and_destroy_element(CPUs, i, estructura, (void*) cpu_destroy);
	sem_post(&CPUsMutex);
	sem_post(&CPUsLibres);
}

void cpu_destroy(t_estructuraCPU* estructura) {
	free(estructura);
}
bool tieneID(t_estructuraCPU* estructura) {
	return (estructura->idCPU == idUltimaCPUDesconectada);
}

void seDesconectoCPU(int idCPU) { //TODO
	if (estaLibreID(idCPU)) {
		sem_wait(&CPUsMutex);
		list_remove_by_condition(CPUs, (void*) tieneID);
		sem_post(&CPUsMutex);
	} else {
		int idPrograma = buscarIDPrograma(idCPU);
		int sd = obtener_sd_Programa(idPrograma);
		notificar_Programa(sd,"La CPU se desconectó, programa abortado");
		sem_wait(&colaExitMutex);
		//queue_push(colaExit, paquete_CPU.pcb); lo comento para que no me tire el error de que no encuenra pquetecpu
		sem_post(&colaExitMutex);
		sem_wait(&CPUsMutex);
		list_remove_by_condition(CPUs, (void*) tieneID);
		sem_wait(&CPUsMutex);
	}
}

void seDesconectoCPUSigusr(int idCPU, t_PCB* pcb) {
	sem_wait(&CPUsMutex);
	list_remove_by_condition(CPUs, (void*) tieneID);
	sem_post(&CPUsMutex);
	sem_wait(&colaReadyMutex);
	queue_push(colaReady, pcb);
	sem_post(&colaReadyMutex);

}

int posicionEnLaLista(t_list* lista, int idCpu) {
	int i = 0;
	while ((*(t_estructuraCPU*) list_get(lista, i)).idCPU != idCpu) {
		i++;
	}
	return i;
}

int estaLibreID(int idCPU) {
	sem_wait(&CPUsMutex);
	int i = posicionEnLaLista(CPUs, idCPU);
	t_estructuraCPU* estructura = list_get(CPUs, i);
	sem_wait(&CPUsMutex);
	return (estructura->estado == 0);

}

void mostrarColaDeProcesosListos() {
	printf("El estado de la Cola Ready es el siguiente:\n");
	sem_wait(&colaReadyMutex);
	mostrarColaDePCBs2(colaReady);
	sem_post(&colaReadyMutex);
}
void mostrarColaDeProcesosFinalizados() {
	printf("El estado de la Cola Exit es el siguiente:\n");
	sem_wait(&colaExitMutex);
	mostrarColaDePCBs2(colaExit);
	sem_post(&colaExitMutex);
}

void mostrarColaDeProcesosEnEjecucion() {
	printf("El estado de la Cola Exec es el siguiente:\n");
	sem_wait(&colaExecMutex);
	mostrarColaDePCBs2(colaExec);
	sem_post(&colaExecMutex);
}

void mostrarColaDeProcesosBloqueados() {
	printf("El estado de la Cola de Bloqueados es el siguiente:\n");
	int a = 0;
	int b = 0;
	while (a < cantidadDeDispositivos) {
		char* id = list_get(idDispositivos, a);
		printf("Procesos bloqueados para el dispositivo %s es el siguiente: \n",
				id);

		t_estructuraDispositivoIO* estructura = dictionary_get(
				diccionarioDispositivos, id);
		sem_wait(&(estructura->mutexCola));
		mostrarColaDePCBsBloqueados(estructura->procesosBloqueados);
		sem_post(&(estructura->mutexCola));
		a++;
	}
	while (b < cantidadDeSemaforos) {
		char* sem = list_get(semaforos, b);
		printf("Procesos bloqueados para el semaforo %s es el siguiente: \n",
				sem);
		//t_estructuraSemaforo* semaforo = dictionary_get(semaforos,sem);
		//sem_wait(&(semaforo->mutexCola));
		//mostrarColaDePCBsBloqueadosSem(semaforo->procesosBloqueados); //TODO no me toma t_estructuraSemaforo (esta en syscalls)
		///sem_post(&(semaforo->mutexCola));
		b++;

	}
}

void mostrarColaDePCBsBloqueados(t_queue* procesosBloqueados) {
	list_iterate(procesosBloqueados->elements,
			(void*) (void*) imprimirNodosPCBsBloqueados);
}

void mostrarColaDePCBsBloqueadosSem(t_queue* procesosBloqueados) {
	list_iterate(procesosBloqueados->elements,
			(void*) (void*) imprimirNodosPCBsBloqueadosSem);
}
void imprimirNodosPCBsBloqueados(t_estructuraProcesoBloqueado* procesoBloqueado) {
	printf("Program id:%i \n", procesoBloqueado->pcb->program_id);
}

void imprimirNodosPCBsBloqueadosSem(int* idProceso) {
	printf("Program id:%i \n", *idProceso);
}
void mostrarColaDePCBs2(t_queue* cola) {
	list_iterate(cola->elements, (void*) (void*) imprimirNodosPCBs2);
}

void imprimirNodosPCBs2(t_PCB* pcb) {
	printf("Program id:%i \n", pcb->program_id);
}

int buscarIDPrograma(int idCPU) {
	sem_wait(&CPUsMutex);
	int posicion = posicionEnLaLista(CPUs, idCPU);
	t_estructuraCPU* CPU = list_get(CPUs, posicion);
	sem_wait(&CPUsMutex);
	return CPU->idProceso;
}
