#include "PCP.h"
#include "PCPinterface.h"

void* pcp_main(void* sinParametro) {
	sinParametros = NULL;
	colaExec = list_create();
	colaIntermedia = queue_create();
	valor_semaforos = list_create();
	semaforos = list_create();
	retardos = list_create();
	idDispositivos = list_create();
	diccionarioDispositivos = dictionary_create();
	sem_init(&diccionarioDispositivosMutex, 0, 1);
	CPUs = list_create();
	sem_init(&colaIntermediaVacia, 0, 0);
	sem_init(&colaIntermediaMutex, 0, 1);
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
		sem_wait(&diccionarioDispositivosMutex);
		dictionary_put(diccionarioDispositivos, idDispositivo,
				estructuraDispositivo);
		sem_post(&diccionarioDispositivosMutex);
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
	retMultiplexorCPUs = pthread_create(&multiplexorCPUs, NULL, atencionCPUs,
			(void*) sinParametros);
	if (retMultiplexorCPUs) {
		fprintf(stderr, "Error - pthread_create() return code: %d\n",
				retMultiplexorCPUs);
		exit(EXIT_FAILURE);
	}
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
		sem_post(&colaReadyMutex);
		sem_wait(&colaIntermediaMutex);
		queue_push(colaIntermedia, procesoAEjecutar);
		sem_post(&colaIntermediaMutex);
		sem_post(&colaIntermediaVacia);
	}
}

void* enviarCPU(void* sinParametro) {
	while (1) {
		sem_wait(&colaIntermediaVacia);
		sem_wait(&CPUsLibres);
		sem_wait(&CPUsMutex);
		int IDCpuLibre = encontrarPrimeraCpuLibre(CPUs);
		sem_post(&CPUsMutex);
		sem_wait(&colaIntermediaMutex);
		t_PCB* pcbAEjecutar = queue_pop(colaIntermedia);
		sem_post(&colaIntermediaMutex);
		sem_wait(&colaExecMutex);
		list_add(colaExec, pcbAEjecutar);
		sem_post(&colaExecMutex);
		t_paquete* paquete = serializarPCB(pcbAEjecutar);
		int i = posicionEnLaLista(CPUs, IDCpuLibre);
		t_estructuraCPU* estructura = malloc(sizeof(t_estructuraCPU));
		estructura->idCPU = IDCpuLibre;
		estructura->estado = 1;
		estructura->idProceso = (pcbAEjecutar->program_id);
		sem_wait(&CPUsMutex);
		list_replace_and_destroy_element(CPUs, i, estructura,
				(void*) cpu_destroy);
		sem_post(&CPUsMutex);
		send(IDCpuLibre, &paquete, sizeof(paquete), 0);
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
	sleep((estructura->retardo)*1000);
	sleep(estructuraBloqueada->tiempo);
	//sleep(estructuraBloqueada->tiempo);
	sem_wait(&colaReadyMutex);
	queue_push(colaReady, estructuraBloqueada->pcb);
	sem_post(&colaReadyMutex);
	return NULL ;
}

int encontrarPrimeraCpuLibre(t_list* lista) {
	t_estructuraCPU* estructura = list_find(lista, (void*) estaLibre);
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

	sem_wait(&diccionarioDispositivosMutex);
	t_estructuraDispositivoIO* estructura = dictionary_get(
			diccionarioDispositivos, dispositivo);
	sem_post(&diccionarioDispositivosMutex);
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
	t_estructuraCPU* CPU = list_get(CPUs, i);
	int pidASacar = CPU->idProceso;
	sem_wait(&colaExecMutex);
	int posicion = posicionEnLaListaExec(colaExec, pidASacar);
	list_remove(colaExec, posicion);
	sem_post(&colaExecMutex);
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
		notificar_Programa(sd, "La CPU se desconectó, programa abortado");
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
	sem_wait(&colaExecMutex);
	int posicion = posicionEnLaListaExec(colaExec, pcb->program_id);
	list_remove(colaExec, posicion);
	sem_post(&colaExecMutex);
	sem_wait(&colaReadyMutex);
	queue_push(colaReady, pcb);
	sem_post(&colaReadyMutex);

}

int posicionEnLaLista(t_list* lista, int idCpu) {
	int i = 0;
	while ((*(t_estructuraCPU*) list_get(lista, i))->idCPU != idCpu) {
		i++;
	}
	return i;
}
int posicionEnLaListaExec(t_list* lista, int pid) {
	int i = 0;
	while ((*(t_PCB*) list_get(lista, i))->program_id != pid) {
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
	mostrarColaDePCBs(colaReady);
	sem_post(&colaReadyMutex);
}
void mostrarColaDeProcesosFinalizados() {
	printf("El estado de la Cola Exit es el siguiente:\n");
	sem_wait(&colaExitMutex);
	mostrarColaDePCBs(colaExit);
	sem_post(&colaExitMutex);
}

void mostrarColaDeProcesosEnEjecucion() {
	printf("El estado de la Cola Exec es el siguiente:\n");
	sem_wait(&colaExecMutex);
	list_iterate(colaExec, (void*) (void*) imprimirNodosPCBs);
	sem_post(&colaExecMutex);
}

void mostrarColaDeProcesosBloqueados() {
	printf("El estado de la Cola de Bloqueados es el siguiente:\n");
	int a = 0;
	int b = 0;
	sem_wait(&diccionarioDispositivosMutex);
	while (a < cantidadDeDispositivos) {
		char* id = list_get(idDispositivos, a);
		printf("Procesos bloqueados para el dispositivo %s es el siguiente: \n",
				id);

		t_estructuraDispositivoIO* estructura = dictionary_get(
				diccionarioDispositivos, id);
		sem_wait(&(estructura->mutexCola));
		mostrarColaDePCBs(estructura->procesosBloqueados);
		sem_post(&(estructura->mutexCola));
		a++;
	}
	sem_post(&diccionarioDispositivosMutex);
	sem_wait(&diccionarioSemaforosMutex);
	while (b < cantidadDeSemaforos) {
		char* sem = list_get(semaforos, b);
		printf("Procesos bloqueados para el semaforo %s es el siguiente: \n",
				sem);
		t_estructuraSemaforo* semaforo = dictionary_get(diccionarioSemaforos,
				sem);
		sem_wait(&(semaforo->mutexCola));
		mostrarColaDePCBsBloqueados(semaforo->procesosBloqueados);
		sem_post(&(semaforo->mutexCola));
		b++;
	}
	sem_wait(&diccionarioSemaforosMutex);
}

void mostrarColaDePCBsBloqueados(t_queue* procesosBloqueados) {
	list_iterate(procesosBloqueados->elements,
			(void*) (void*) imprimirNodosPCBsBloqueados);
}

void imprimirNodosPCBsBloqueados(t_estructuraProcesoBloqueado* procesoBloqueado) {
	printf("Program id:%i \n", procesoBloqueado->pcb->program_id);
}

int buscarIDPrograma(int idCPU) {
	sem_wait(&CPUsMutex);
	int posicion = posicionEnLaLista(CPUs, idCPU);
	t_estructuraCPU* CPU = list_get(CPUs, posicion);
	sem_wait(&CPUsMutex);
	return CPU->idProceso;
}

