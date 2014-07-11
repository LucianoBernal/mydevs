#include "PCP.h"
#include "Kernel.h"

sem_t * CPUsLibres = NULL;
sem_t * sPLP = NULL;
sem_t * sYaInicializoElMT = NULL;
sem_t * sBloqueado = NULL;
sem_t * colaExecVacia = NULL;
sem_t * semCPUDesconectadaMutex = NULL;
int laSenialDeTerminar = 0;
static t_queue* colaExec = queue_create();
pthread_t ejecutar;
pthread_t envCPU;
pthread_t recCPU;
int retMandarAEjecutar, retRecibirCPU;
int* sinParametros = NULL;
static t_dictionary diccionarioDispositivos = dictionary_create();
t_list* CPUs = list_create();
int idUltimaCPUDesconectada;

void pcp_main() {
	sem_init(CPUsLibres, 0, 0);
	sem_init(sPLP, 0, 0);
	sem_init(sYaInicializoElMT, 0, 0);
	sem_init(sBloqueado, 0, 0);
	sem_init(colaExecVacia, 0, 0);
	sem_init(semCPUDesconectadaMutex, 0, 1);
	//crearHilosDeEntradSalida
	int i = 0;
	while (idhio[i] != '\0') {
		int retardo = buscarRetardo(idhio[i]);
		static t_queue* colaDispositivo = queue_create();
		static sem_t* semaforo = NULL;
		sem_init(semaforo, 0, 0);
		t_estructuraDispositivoIO* estructuraDispositivo=malloc(sizeof(t_estructuraDispositivoIO));
		estructuraDispositivo->retardo = retardo;
		estructuraDispositivo->procesosBloqueados = colaDispositivo;
		estructuraDispositivo->semaforoCola = semaforo;
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
}

void crearHilosPrincipales() {

	retMandarAEjecutar = pthread_create(&ejecutar, NULL, mandarAEjecutar,
			(void*) sinParametros);
	if (retMandarAEjecutar) {
		fprintf(stderr, "Error - pthread_create() return code: %d\n",
				retMandarAEjecutar);
		exit(EXIT_FAILURE);
	}
	retRecibirCPU = pthread_create(&recCPU, NULL, recibirCPU,
			(void*) sinParametros);
	if (retRecibirCPU) {
		fprintf(stderr, "Error - pthread_create() return code: %d\n",
				retRecibirCPU);
		exit(EXIT_FAILURE);
	}
	pthread_join(ejecutar, NULL );
	pthread_join(recCPU, NULL );

}

/*void crearHilosDeEntradaSalida() {
	int i = 0;
	while (idhio[i] != '\0') {
		int retardo = buscarRetardo(idhio[i]);
		static t_queue* colaDispositivo = queue_create();
		static sem_t* semaforo = NULL;
		sem_init(semaforo, 0, 0);
		t_estructuraDispositivoIO* estructuraDispositivo; //TODO
		estructuraDispositivo->retardo = retardo;
		estructuraDispositivo->procesosBloqueados = colaDispositivo;
		estructuraDispositivo->semaforoCola = semaforo;
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
	//saca procesos de la cola de ready y los manda a exec

	sem_wait(vacioReady);
	sem_wait(colaReadyMutex);
	t_PCB* procesoAEjecutar = queue_pop(colaReady);
	queue_push(colaExec, procesoAEjecutar);
	enviarCPU();
	sem_post(colaReadyMutex);
	sem_post(colaExecVacia);

}

void* recibirCPU(void* j) {
	int listenningSocket;
	sem_wait(sYaInicializoElMT);
	crearSocketL(listeningSocket, puerto_CPU);
	listen(listenningSocket, BACKLOG);
	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);
	int socketCliente = accept(listenningSocket, (struct sockaddr *) &addr,
			&addrlen);
	t_paquete_recibir_CPU paquete_CPU;
	while (!laSenialDeTerminar) {
		recv(socketCliente, (void*) paquete_R, sizeof(t_paquete_recibir_CPU),
				MSG_WAITALL);
		serializameCPU(paquete_R, &paquete_CPU);
		queue_push(cCPUs, &paquete_CPU); //Guardo el puntero en la lista asi puedo acceder a todos los datos ?
		switch (paquete_CPU->razon) {
		case 'n': //La instancia de CPU es nueva
			nuevaCPU(paquete_CPU.IDCpu);
			break;
		case 'q': //El Programa salio del CPU por quantum
			programaSalioPorQuantum(paquete_CPU.pcb, paquete_CPU.IDCpu);
			break;
		case 't': //El Programa termino normalmente
			moverAColaExit(paquete_CPU.pcb, paquete_CPU.IDCpu);
			break;
		case 'b': //El Programa salio por bloqueo
			programaSalioPorBloqueo(paquete_CPU.pcb, paquete_CPU.tiempo,
					paquete_CPU.dispositivoIO, paquete_CPU.IDCpu);
			break;
		case 'd': //la CPU se desconectó
			sem_wait(semCPUDesconectadaMutex);
			idUltimaCPUDesconectada = paquete_CPU.IDCpu;
			seDesconectoCPU(paquete_CPU.IDCpu);
			sem_post(semCPUDesconectadaMutex);
			break;
		}
	}
	default:
	printf("Si ves esto es que algo mal hicimos");
	break;

}

void enviarCPU() {
	sem_wait(CPUsLibres);
	int IDCpuLibre = encontrarPrimeraCpuLibreYOcuparla(CPUs);
	t_PCB* paquete = queue_pop(colaExec);
	//TODO serializar(paquete);
	//TODO ponerle el program id de ese pcb a la cpu en la lista
}

void* bloquearYDevolverAReady(void * param) {
	t_estructuraDispositivoIO estructura;
	estructura = (t_estructuraDispositivoIO *) param;
	sem_wait(estructura->semaforoCola);
	t_estructuraProcesoBloqueado estructuraBloqueada = queue_pop(
			estructura->procesosBloqueados);
	delay(estructura->retardo);
	delay((estructuraBloqueada.tiempo) / 1000);
	sem_wait(colaReadyMutex);
	queue_pop(colaReady, estructuraBloqueada.pcb);
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
	queue_push(colaExit, pcb);
	seLiberoUnaCPU(idCPU);
}

void programaSalioPorBloqueo(t_PCB* pcb, int tiempo, char* dispositivo,
		int idCPU) {
	t_estructuraDispositivoIO estructura = dictionary_get(
			diccionarioDispositivos, dispositivo);
	t_estructuraProcesoBloqueado procesoBloqueado;
	procesoBloqueado.pcb = pcb;
	procesoBloqueado.tiempo = tiempo;
	queue_push(estructura.procesosBloqueados, procesoBloqueado);
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
	if (estaLibre(idCPU)) {
		list_remove_by_condition(CPUs, (void*) tieneID);
	} else {
		// TODO se manda un error a la consola del programa
		queue_push(colaExit, paquete_CPU.pcb);
		list_remove_by_condition(CPUs, (void*) tieneID);
	}
}

void seDesconectoCPUSigusr(int idCPU, t_PCB* pcb) {
	list_remove_by_condition(CPUs, (void*) tieneID());
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

int estaLibre(int idCPU) {
	int i = posicionEnLaLista(CPUs, idCPU);
	t_estructuraCPU* estructura = list_get(CPUs, i);
	return (estructura->estado == 0);

}

