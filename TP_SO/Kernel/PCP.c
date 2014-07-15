#include "PCP.h"
#include "Kernel.h"

void* pcp_main(void* sinParametro) {
	colaExec = queue_create();
	diccionarioDispositivos = dictionary_create();
	CPUs = list_create();
	sem_init(CPUsLibres, 0, 0);
	sem_init(sPLP, 0, 0);
	sem_init(sYaInicializoElMT, 0, 0);
	sem_init(sBloqueado, 0, 0);
	sem_init(colaExecVacia, 0, 0);
	sem_init(semCPUDesconectadaMutex, 0, 1);
	//crearHilosDeEntradSalida
	int i = 0;
	while (i < cantidadDeDispositivos) {
		int retardo = buscarRetardo(idhio[i]);
		t_queue* colaDispositivo = queue_create();
		sem_t* semaforo = NULL;
		sem_t* mutex = NULL;
		sem_init(semaforo, 0, 0);
		sem_init(mutex, 0, 1);
		t_estructuraDispositivoIO* estructuraDispositivo = malloc(
				sizeof(t_estructuraDispositivoIO));
		estructuraDispositivo->retardo = retardo;
		estructuraDispositivo->procesosBloqueados = colaDispositivo;
		estructuraDispositivo->colaVacia = semaforo;
		estructuraDispositivo->mutexCola = mutex;
		dictionary_put(diccionarioDispositivos, idhio[i],
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
	retMultiplexorCPUs = pthread_create(&multiplexorCPUs, NULL, multiplexorCPUs,
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

/*void crearHilosDeEntradaSalida() {
 int i = 0;
 while (idhio[i] != '\0') {
 int retardo = buscarRetardo(idhio[i]);
 static t_queue* colaDispositivo = queue_create();
 static sem_t* semaforo = NULL;
 sem_init(semaforo, 0, 0);
 t_estructuraDispositivoIO* estructuraDispositivo;
 estructuraDispositivo->retardo = retardo;
 estructuraDispositivo->procesosBloqueados = colaDispositivo;
 estructuraDispositivo->colaVacia = semaforo;
 dictionary_put(diccionarioDispositivos, idhio[i],
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
 }*/

void* mandarAEjecutar(void* j) {
	while(1){
	sem_wait(vacioReady);
	sem_wait(colaReadyMutex);
	t_PCB* procesoAEjecutar = queue_pop(colaReady);
	queue_push(colaExec, procesoAEjecutar);
	int * sinParametro = NULL;
	enviarCPU(sinParametro);
	free(sinParametro);
	sem_post(colaReadyMutex);
	sem_post(colaExecVacia);
	}
}

//void* recibirCPU(void* j) {
//	int listenningSocket;
//	sem_wait(sYaInicializoElMT);
//	crearSocketL(listeningSocket, puerto_CPU);
//	listen(listenningSocket, BACKLOG);
//	struct sockaddr_in addr;
//	socklen_t addrlen = sizeof(addr);
//	int socketCliente = accept(listenningSocket, (struct sockaddr *) &addr,
//			&addrlen);
//	t_paquete_recibir_CPU paquete_CPU;
//	while (!laSenialDeTerminar) {
//		recv(socketCliente, (void*) paquete_R, sizeof(t_paquete_recibir_CPU),
//				MSG_WAITALL);
//		serializameCPU(paquete_R, &paquete_CPU);
//		queue_push(cCPUs, &paquete_CPU); //Guardo el puntero en la lista asi puedo acceder a todos los datos ?
//		switch (paquete_CPU->razon) {
//		case 'n': //La instancia de CPU es nueva
//			nuevaCPU(paquete_CPU.IDCpu);
//			break;
//		case 'q': //El Programa salio del CPU por quantum
//			programaSalioPorQuantum(paquete_CPU.pcb, paquete_CPU.IDCpu);
//			break;
//		case 't': //El Programa termino normalmente
//			moverAColaExit(paquete_CPU.pcb, paquete_CPU.IDCpu);
//			break;
//		case 'b': //El Programa salio por bloqueo
//			programaSalioPorBloqueo(paquete_CPU.pcb, paquete_CPU.tiempo,
//					paquete_CPU.dispositivoIO, paquete_CPU.IDCpu);
//			break;
//		case 'd': //la CPU se desconectó
//			sem_wait(semCPUDesconectadaMutex);
//			idUltimaCPUDesconectada = paquete_CPU.IDCpu;
//			seDesconectoCPU(paquete_CPU.IDCpu);
//			sem_post(semCPUDesconectadaMutex);
//			break;
//		}
//	}
//	default:
//	printf("Si ves esto es que algo mal hicimos");
//	break;
//
//}

void* enviarCPU(void* sinParametro) {
	while(1){
	sem_wait(CPUsLibres);
	int IDCpuLibre = encontrarPrimeraCpuLibreYOcuparla(CPUs);
	t_PCB* paquete = queue_pop(colaExec);
	printf("%d", IDCpuLibre);
	printf("%d", paquete->indice_de_Codigo);
	//lo hago para sacar el warning por variable sin usar,
	//TODO serializar(paquete);
	int i = posicionEnLaLista(CPUs, IDCpuLibre);
		t_estructuraCPU* estructura = malloc(sizeof(t_estructuraCPU));
		estructura->idCPU = IDCpuLibre;
		estructura->estado = 0;
		estructura->idProceso = (paquete->program_id);
		list_replace_and_destroy_element(CPUs, i, estructura, (void*) cpu_destroy);
	//TODO mandar el paquete serializado a esa id (sd)
	}
}

void* bloquearYDevolverAReady(void * param) {
	t_estructuraDispositivoIO* estructura = malloc(
			sizeof(t_estructuraDispositivoIO));
	estructura = (t_estructuraDispositivoIO *) param;
	sem_wait(estructura->colaVacia);
	sem_wait(estructura->mutexCola);
	t_estructuraProcesoBloqueado* estructuraBloqueada = queue_pop(
			estructura->procesosBloqueados);
	sem_post(estructura->mutexCola);
	delay(estructura->retardo);
	delay((estructuraBloqueada->tiempo) / 1000);
	sem_wait(colaReadyMutex);
	queue_push(colaReady, estructuraBloqueada->pcb);
	sem_post(colaReadyMutex);

}

int buscarRetardo(char* dispositivo) {
	int i = 0;
	while (idhio[i] != dispositivo) {
		i++;
	}
	return hio[i];
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
	list_add(CPUs, estructura);
	sem_post(CPUsLibres);
}

void programaSalioPorQuantum(t_PCB* pcb, int idCPU) {
	sem_wait(colaReadyMutex);
	queue_push(colaReady, pcb);
	sem_post(colaReadyMutex);
	seLiberoUnaCPU(idCPU);
}

void moverAColaExit(t_PCB* pcb, int idCPU) {
	sem_wait(colaExitMutex);
	queue_push(colaExit, pcb);
	sem_post(colaExitMutex);
	sem_post(colaExitVacio);
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
	sem_wait(estructura->mutexCola);
	queue_push(estructura->procesosBloqueados, procesoBloqueado);
	sem_post(estructura->mutexCola);
	sem_post(estructura->colaVacia);
	seLiberoUnaCPU(idCPU);
}

void seLiberoUnaCPU(int idCPU) {
	int i = posicionEnLaLista(CPUs, idCPU);
	t_estructuraCPU* estructura = malloc(sizeof(t_estructuraCPU));
	estructura->idCPU = idCPU;
	estructura->estado = 0;
	estructura->idProceso = -1;
	list_replace_and_destroy_element(CPUs, i, estructura, (void*) cpu_destroy);
	sem_post(CPUsLibres);
}

void cpu_destroy(t_estructuraCPU* estructura) {
	free(estructura);
}
bool tieneID(t_estructuraCPU* estructura) {
	return (estructura->idCPU == idUltimaCPUDesconectada);
}

void seDesconectoCPU(int idCPU) { //TODO
	if (estaLibreID(idCPU)) {
		list_remove_by_condition(CPUs, (void*) tieneID);
	} else {
		int idPrograma = buscarIDPrograma(idCPU);
		int sd = obtener_sd_programa(idPrograma);
		notificar_Programa(sd,"La CPU se desconectó");
		//queue_push(colaExit, paquete_CPU.pcb); lo comento para que no me tire el error de que no encuenra pquetecpu
		list_remove_by_condition(CPUs, (void*) tieneID);
	}
}

void seDesconectoCPUSigusr(int idCPU, t_PCB* pcb) {
	list_remove_by_condition(CPUs, (void*) tieneID);
	sem_wait(colaReadyMutex);
	queue_push(colaReady, pcb);
	sem_post(colaReadyMutex);

}

int posicionEnLaLista(t_list* lista, int idCpu) {
	int i = 0;
	while ((*(t_estructuraCPU*) list_get(lista, i)).idCPU != idCpu) {
		i++;
	}
	return i;
}

int estaLibreID(int idCPU) {
	int i = posicionEnLaLista(CPUs, idCPU);
	t_estructuraCPU* estructura = list_get(CPUs, i);
	return (estructura->estado == 0);

}

void mostrarColaDeProcesosListos() {
	printf("El estado de la Cola Ready es el siguiente:\n");
	mostrarColaDePCBs2(colaReady);
}
void mostrarColaDeProcesosFinalizados() {
	printf("El estado de la Cola Exit es el siguiente:\n");
	mostrarColaDePCBs2( colaExit);
}

void mostrarColaDeProcesosEnEjecucion() {
	printf("El estado de la Cola Exec es el siguiente:\n");
	mostrarColaDePCBs2(colaExec);
}

void mostrarColaDeProcesosBloqueados() {
	printf("El estado de la Cola de Bloqueados es el siguiente:\n");
	int a = 0;
	int b = 0;
	while (a< cantidadDeDispositivos){
		printf("Procesos bloqueados para el dispositivo %s es el siguiente: \n",
				idhio[a]);

		t_estructuraDispositivoIO* estructura = dictionary_get(
				diccionarioDispositivos, idhio[a]);
	 mostrarColaDePCBsBloqueados(estructura->procesosBloqueados);
	 a++;
	}
	while (b<cantidadDeSemaforos){
		printf("Procesos bloqueados para el semaforo %s es el siguiente: \n",
						semaforos[b]);
		t_estructuraSemaforo* semaforo = dictionary_get(semaforos,semaforos[b]);
		mostrarColaDePCBsBloqueadosSem(semaforo->procesosBloqueados);
		b++;

	}
}


void mostrarColaDePCBsBloqueados(t_queue* procesosBloqueados)
{
	list_iterate(procesosBloqueados->elements, (void*) (void*) imprimirNodosPCBsBloqueados);
}

void mostrarColaDePCBsBloqueadosSem(t_queue* procesosBloqueados)
{
	list_iterate(procesosBloqueados->elements, (void*) (void*) imprimirNodosPCBsBloqueadosSem);
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

int buscarIDPrograma(int idCPU)
{
	int posicion = posicionEnLaLista(CPUs, idCPU);
	t_estructuraCPU* CPU = list_get(CPUs, posicion);
	return CPU->idProceso;
}
