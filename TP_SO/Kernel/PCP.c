#include "PCP.h"
#include "PCPinterface.h"

void* pcp_main(void* sinParametro) {
	sinParametros = NULL;
	colaExec = list_create();
	diccionarioDispositivos = dictionary_create();
	printf("diccionario de variables vale: %x\n", (u_int)variables_globales);
	sem_init(&diccionarioDispositivosMutex, 0, 1);
	CPUs = list_create();
	sem_init(&CPUsLibres, 0, 0); //FIXME CPUS LIBRES VA 0, PUSE 3 PARA PROBAR
	sem_init(&sBloqueado, 0, 0);
	sem_init(&colaExecVacia, 0, 0);
	sem_init(&semCPUDesconectadaMutex, 0, 1);
	sem_init(&CPUsMutex, 0, 1);
	sem_init(&colaExecMutex, 0, 1);

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
				(void*) estructuraDispositivo);
		if (retIO) {
			log_error(logKernel,
					"Error al crear hilo bloquear y devolver a Ready de dispositivo,Error %d",
					retIO);
			exit(EXIT_FAILURE);
		}
		log_info(logKernel,
				"Hilo bloquear y devolver a Ready de dispositivo %s exitoso",
				idDispositivo);
		i++;
	}
	crearHilosPrincipales();
	return EXIT_SUCCESS;
}

void crearHilosPrincipales() {
	retMultiplexorCPUs = pthread_create(&multiplexorCPUs, NULL, atencionCPUs,
			(void*) sinParametros);
	if (retMultiplexorCPUs) {
		log_error(logKernel, "Error al crear hilo atencionCPUs,con error:",
				retMultiplexorCPUs);
		exit(EXIT_FAILURE);
	}
	log_info(logKernel, "Exito al crear hilo atencionCPUs");
	retMandarAEjecutar = pthread_create(&ejecutar, NULL, mandarAEjecutar,
			(void*) sinParametros);
	if (retMandarAEjecutar) {
		log_error(logKernel,
				"Error al crear hilo mandar a Ejecutar, con error:",
				retMandarAEjecutar);
		exit(EXIT_FAILURE);
	}
	log_info(logKernel, "Exito al crear hilo mandar a Ejecutar");
	pthread_join(multiplexorCPUs, NULL );
	pthread_join(ejecutar, NULL );

}

void* mandarAEjecutar(void* j) {
	while (1) {
		sem_wait(&vacioReady);
		sem_wait(&CPUsLibres);
		sem_wait(&colaReadyMutex);
		t_PCB* procesoAEjecutar = queue_pop(colaReady);
		log_info(logKernel, "Se saco programa de la cola Ready, con PID:%d",
				procesoAEjecutar->program_id);
		sem_post(&colaReadyMutex);
		enviarCPU(procesoAEjecutar);
	}
}

void enviarCPU(t_PCB* pcbAEjecutar) {
		sem_wait(&CPUsMutex);
		int IDCpuLibre = encontrarPrimeraCpuLibre(CPUs); //FIXME PARA PRUEBA
		//int IDCpuLibre =1;
		sem_post(&CPUsMutex);
		sem_wait(&colaExecMutex);
		list_add(colaExec, pcbAEjecutar);
		log_info(logKernel,
				"Agregando programa en cola de ejecucion,con PID:%d",
				pcbAEjecutar->program_id);
		sem_post(&colaExecMutex);
		mostrar_todas_Las_Listas();
		t_paquete* paquete = serializarPCB(pcbAEjecutar);
		sem_wait(&CPUsMutex);
		int i = posicionEnLaLista(CPUs, IDCpuLibre); //FIXME PARA PRUEBA
		t_estructuraCPU* estructura = malloc(sizeof(t_estructuraCPU));
		estructura->idCPU = IDCpuLibre;
		estructura->estado = 1;
		estructura->idProceso = (pcbAEjecutar->program_id);
		list_replace_and_destroy_element(CPUs, i, estructura, //  FIXME PARA
				(void*) cpu_destroy);						//	FIXME PRUEBA
		sem_post(&CPUsMutex);
		log_info(logKernel,
				"Enviando Programa a CPU disponible, con PID:%d y CPU:%d",
				pcbAEjecutar->program_id, IDCpuLibre);
		enviarConRazon(IDCpuLibre, logKernel, HANDSHAKE_CPU_KERNEL,
				serializar2(crear_nodoVar(paquete->msj, paquete->tamano), 0));
		}


void* bloquearYDevolverAReady(void * param) {
	t_estructuraDispositivoIO* estructura /*= malloc(
	 sizeof(t_estructuraDispositivoIO))*/;
	estructura = (t_estructuraDispositivoIO *) param;
	while (1) {
		sem_wait(&(estructura->colaVacia));
		sem_wait(&(estructura->mutexCola));
		t_estructuraProcesoBloqueado* estructuraBloqueada = queue_pop(
				estructura->procesosBloqueados);
		sem_post(&(estructura->mutexCola));
		usleep((estructura->retardo) * (estructuraBloqueada->tiempo) * 1000);
		sem_wait(&colaReadyMutex);
		queue_push(colaReady, estructuraBloqueada->pcb);
		sem_post(&colaReadyMutex);
		sem_post(&vacioReady);
		mostrar_todas_Las_Listas();
	}
	queue_destroy(estructura->procesosBloqueados);
	free(estructura);
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
	seLiberoUnaCPU(idCPU);
	sem_wait(&colaReadyMutex);
	log_debug(logKernel, "Estoy por encolar el pcb en ready");
	queue_push(colaReady, pcb);
	sem_post(&colaReadyMutex);
	mostrar_todas_Las_Listas();
	sem_post(&vacioReady);


}

void moverAColaExit(t_PCB* pcb, int idCPU) {
	sem_wait(&colaExitMutex);
	queue_push(colaExit, pcb);
	sem_post(&colaExitMutex);
	log_info(logKernel, "Se movio programa a cola exit, PID:%d",
			pcb->program_id);
	mostrar_todas_Las_Listas();
	sem_post(&colaExitVacio);
}

void moverAColaExityLiberarCPU(t_PCB* pcb, int idCPU) {
	seLiberoUnaCPU(idCPU);
	moverAColaExit(pcb, idCPU);

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
	mostrar_todas_Las_Listas();
	sem_post(&(estructura->colaVacia));
	seLiberoUnaCPU(idCPU);
}

void seLiberoUnaCPU(int idCPU) {
	sem_wait(&CPUsMutex);
	int i = posicionEnLaLista(CPUs, idCPU);
	t_estructuraCPU* CPU = list_get(CPUs, i);
	sem_post(&CPUsMutex);
	int pidASacar = CPU->idProceso;
	bool victimaPCB_exec(t_PCB* self){
		return self->program_id==pidASacar;
	}
	sem_wait(&colaExecMutex);
	list_remove_and_destroy_by_condition(colaExec, (void*)victimaPCB_exec,(void*)free);
	sem_post(&colaExecMutex);
	t_estructuraCPU* estructura = malloc(sizeof(t_estructuraCPU));
	estructura->idCPU = idCPU;
	estructura->estado = 0;
	estructura->idProceso = -1;
	sem_wait(&CPUsMutex);
	int pos = posicionEnLaLista(CPUs, idCPU);
	list_replace_and_destroy_element(CPUs, pos, estructura,
			(void*) cpu_destroy);
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
	if (!estaLibreID(idCPU)) {
		int idPrograma = buscarIDPrograma(idCPU);
	printf("idPrograma:%d \n", idPrograma);
		int sd = obtener_sd_Programa(idPrograma);
		printf("SD:%d\n", sd);
		//notificar_Programa(sd, "La CPU se desconectó, programa abortado");
		bool esElpcb(t_PCB* pcbAcomparar) {
			return pcbAcomparar->program_id == idPrograma;
		}
		sem_wait(&colaExecMutex);
		t_PCB* pcb = list_remove_by_condition(colaExec, (void*) esElpcb);
		sem_post(&colaExecMutex);
		moverAColaExit(pcb, idCPU);
		}else {
			sem_wait(&CPUsLibres);
		}
	sem_wait(&CPUsMutex);
	list_remove_and_destroy_by_condition(CPUs, (void*) tieneID,(void*)free);
	sem_post(&CPUsMutex);
}

void seDesconectoCPUSigusr(int idCPU, t_PCB* pcb) {
	sem_wait(&CPUsMutex);
	t_estructuraCPU* CPU=list_remove_by_condition(CPUs, (void*) tieneID);
	sem_post(&CPUsMutex);
	int pidASacar=CPU->idProceso;
	bool victimaPCB_exec(t_PCB* self){
			return self->program_id==pidASacar;
		}
	sem_wait(&colaExecMutex);
	list_remove_and_destroy_by_condition(colaExec, (void*)victimaPCB_exec,(void*)free);
	sem_post(&colaExecMutex);
	sem_wait(&colaReadyMutex);
	queue_push(colaReady, pcb);
	sem_post(&colaReadyMutex);
	mostrar_todas_Las_Listas();
	sem_post(&vacioReady);
	free(CPU);

}

int posicionEnLaLista(t_list* lista, int idCpu) {
	int i = 0;
	while ((*(t_estructuraCPU*) list_get(lista, i)).idCPU != idCpu) {
		i++;
	}
	return i;
}
int posicionEnLaListaExec(t_list* lista, int pid) {
	int i = 0;
	while ((*(t_PCB*) list_get(lista, i)).program_id != pid) {
		i++;
	}
	return i;
}

int estaLibreID(int idCPU) {
	sem_wait(&CPUsMutex);
	int i = posicionEnLaLista(CPUs, idCPU);
	t_estructuraCPU* estructura = list_get(CPUs, i);
	sem_post(&CPUsMutex);
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
		mostrarColaDePCBsBloqueados(estructura->procesosBloqueados);
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
		mostrarColaDePCBs(semaforo->procesosBloqueados);
		sem_post(&(semaforo->mutexCola));
		b++;
	}
	sem_post(&diccionarioSemaforosMutex);
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
	sem_post(&CPUsMutex);
	return CPU->idProceso;
}
