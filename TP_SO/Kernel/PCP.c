#include "PCP.h"

int puertoCPU; //se saca del config tambien
const dispositivosEntradaSalida, retardosDispositivos; //son dos vectores que están en el Kernel, se leen también del config

sem_t * CPUsLibres = 0;
sem_t * sPLP = 0;
sem_t * sYaInicializoElMT = 0;
sem_t * sBloqueado = 0;
sem_t * colaExecVacia = 0;
int laSenialDeTerminar = 0;
static t_queue* colaExec;
pthread_t mandarAEjecutar;
pthread_t enviarCPU;
pthread_t recibirCPU;
t_dictionary diccionarioDispositivos;
t_list CPUs;

void crearHilosPrincipales() {
	pthread_create(&mandarAEjecutar, NULL, mandarAEjecutar, NULL );
	pthread_create(&enviarCPU, NULL, enviarCPU, NULL );
	pthread_create(&recibirCPU, NULL, recibirCPU, NULL );
}

void crearHilosDeEntradaSalida() {
	int i = 0;
	while (dispositivosEntradaSalida[i] != '\0') {
		int retardo = buscarRetardo(dispositivosEntradaSalida[i]);
		static t_queue* colaDispositivo;
		sem_t* semaforo = 0;
		t_estructuraDispositivoIO estructuraDispositivo;
		estructuraDispositivo.retardo = retardo;
		estructuraDispositivo.procesosBloqueados = colaDispositivo;
		estructuraDispositivo.semaforoCola = semaforo;
		dictionary_put(diccionarioDispositivos, dispositivosEntradaSalida[i],
				estructuraDispositivo);
		pthread_t dispositivo;
		pthread_create(dispositivo, NULL, bloquearYDevolverAReady,
				(void*) &estructuraDispositivo);
	}
}

void* mandarAEjecutar() {
// saca procesos de la cola de ready y los manda a la cola de Exec

	sem_wait(vacioReady);
	sem_wait(colaReadyMutex);
	queue_push(colaExec, queue_pop(colaReady));
	sem_post(colaReadyMutex);
	sem_post(colaExecVacia);
	return (NULL );

}

void* recibirCPU() {
	//recibe procesos que salieron de la cpu y según el motivo hace lo que tenga que hacer
	int listenningSocket;
	sem_wait(sYaInicializoElMT);
	crearSocketL(listeningSocket, PUERTOCPU);
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
		queue_push(cCPUs, &paquete_CPU); //Guardo el puntero en la lista asi puedo acceder a todos los datos
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
			seDesconectoCPU(paquete_CPU.IDCpu);
			break;
		}
	}
	default:
	printf("Si ves esto es que algo mal hicimos");
	break;

	return (NULL );
}

void* enviarCPU() {
	sem_wait(colaExecVacia);
	sem_wait(CPUsLibres);
	int IDCpuLibre = encontrarPrimeraCpuLibre();
	t_PCB* pcb = queue_pop(colaExec);
	t_paquete_enviar_CPU paquete;
	paquete.pcb = pcb;
	paquete.IDCpu = IDCpuLibre;
//paquete ->esto hay que mandarselo a la cpu por sockets, aunque no sé si hay que sacarlo de la cola...

	return (NULL );
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
	return (NULL );

}

int buscarRetardo(char* dispositivo) {
	int i = 0;
	while (dispositivosEntradaSalida[i] != dispositivo) {
		i++;
	}
	return retardosDispositivos[i];
}

int encontrarPrimeraCpuLibre() {
	t_estructuraCPU estructura = list_find_element(CPUs, (void*) estaLibre,
			NULL );
	estructura.estado = 1;
	return (estructura.idCPU);
}

bool estaLibre(t_estructuraCPU estructura) {
	return (estructura.estado == 0);
}

void nuevaCPU(int idCPU) {
	t_estructuraCPU estructura;
	estructura.idCPU = idCPU;
	estructura.estado = 0;
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

void programaSalioPorBloqueo(t_PCB* pcb, int tiempo, dispositivo, int idCPU) {
	t_estructuraDispositivoIO estructura = dictionary_get(
			diccionarioDispositivos, dispositivo);
	t_estructuraProcesoBloqueado procesoBloqueado;
	procesoBloqueado.pcb = pcb;
	procesoBloqueado.tiempo = tiempo;
	queue_push(estructura.procesosBloqueados, procesoBloqueado); //lo cambia en el diccionario?
	seLiberoUnaCPU(idCPU);
}

void seLiberoUnaCPU(int idCPU) {
//me falta seguir aca
	sem_post(CPUsLibres);
}

bool tieneID(int id, t_estructuraCPU estructura) //no se si se banca esto
{
	return (estructura.idCPU == id);
}

void seDesconectoCPU(int idCPU) {
	if (la cpu esta libre /*me falta esto*/) {
		list_remove_by_condition(CPUs, (void*) tieneID(paquete_CPU.IDCpu));
	} else {
		//se manda un error a la consola del programa
		queue_push(colaExit, paquete_CPU->pcb);
		list_remove_by_condition(CPUs, (void*) tieneID(paquete_CPU.IDCpu));
	}
}
