#include<semaphore.h>
#include<pthread.h>

int puertoCPU; //se saca del config tambien
const dispositivosEntradaSalida, retardosDispositivos; //son dos vectores que están en el Kernel, se leen también del config

typedef struct {
	int ID, PIDPrev;
	char razon;
	t_dispositivo dispositivoIO;
	int tiempo;
} t_paquete_CPU;

sem_t * sCPU, sPLP, sYaInicializoElMT, sBloqueado, colaExecVacia = 0;
int laSenialDeTerminar = 0;
static t_queue* colaExec;
static t_queue* colaBloqueados;

pthread_t pcp, enviarCPU, recibirCPU;

void crearHilosPrincipales() {
	pthread_create(&pcp, NULL, pcp, NULL );
	pthread_create(&enviarCPU, NULL, enviarCPU, NULL );
	pthread_create(&recibirCPU, NULL, recibirCPU, NULL );
}

void crearHilosDeEntradaSalida() {
	int i = 0;
	while (dispositivosEntradaSalida[i] != '\0') {
		t_dispositivo dispositivo = dispositivosEntradaSalida[i];
		int retardo = buscarRetardo(dispositivo);
		pthread_t dispositivosEntradaSalida[i];
		pthread_create(&dispositivosEntradaSalida[i], NULL,
				bloquearYDevolverAReady, retardo);
	}
}

void *pcp() {
// saca procesos de la cola de ready y los manda a la cola de Exec

	sem_wait(vacioReady);
	sem_wait(colaReadyMutex);
	queue_push(colaExec, queue_pop(colaReady));
	sem_post(colaReadyMutex);
	sem_post(colaExecVacia);

}

void* hilo_RecibirCPU() {
	//recibe procesos que salieron de la cpu y según el motivo hace lo que tenga que hacer
	int listenningSocket;
	wait(sYaInicializoElMT);
	crearSocketL(listeningSocket, PUERTOCPU);
	listen(listenningSocket, BACKLOG);
	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);
	int socketCliente = accept(listenningSocket, (struct sockaddr *) &addr,
			&addrlen);
	t_paquete_CPU paquete_CPU;
	while (!laSenialDeTerminar) {
		recv(socketCliente, (void*) paquete_R, sizeof(t_paquete_CPU),
				MSG_WAITALL);
		serializameCPU(paquete_R, &paquete_CPU);
		queue_push(cCPUs, &paquete_CPU); //Guardo el puntero en la lista asi puedo acceder a todos los datos
		switch (paquete_CPU.razon) { //razon es un char que representa por que vuelve el proceso
		case 'n': //La instancia de CPU es nueva

		case 'q': //El Programa salio del CPU por quantum
			wait(colaReadyMutex);
			queue_push(colaReady, obtenerDireccionPCB(paquete_CPU.PIDPrev));
			signal(colaReadyMutex);
			break;
		case 't': //El Programa termino normalmente
		queue_push(colaExit, obtenerDireccionPCB(paquete_CPU.PIDPrev)).
		break;
		case 'b': //El Programa salio por bloqueo
			estructuraEntradaSalida.proceso = obtenerDireccionPCB(
					paquete_CPU.PIDPrev);
			estrcuturaEntradaSalida.dispositivo = paquete_CPU.dispositivoIO; //supongamos que en dispositivoIO si no es IO pone 0 o lo que sea :P
			// tambien supongamos que estructuraentradasalida son los nodos de la lista de bloqeuados, después lo hago
			queue_push(colaBloqueados, estructuraEntradaSalida);
			signal(sBloqueado);
		default:
			printf("Si ves esto es que algo mal hicimos");
			break;
		}
	}
}

void* hilo_EnviarCPU() {
	sem_wait(colaExecVacia);
	queue_pop(colaExec); //->esto hay que mandarselo a la cpu por sockets, aunque no sé si hay que sacarlo de la cola... en realidad no

}

/*void hilo_bloqueados() {

	while (!laSenialDeTerminar) {
		sem_wait(sBloqueado);
		estructura = queue_pop(colaBloqueados);
		bloquearPor(tiempoBloqueo, estrucutra.dispositivo, estructura.proceso);
		queue_push(colaReady, proceso);
	}
}*/

void * bloquearYDevolverAReady(void* param) {

	delay(param);

}

int buscarRetardo(char* dispositivo) {
	int i = 0;
	while (dispositivosEntradaSalida[i] != dispositivo) {
		i++;
	}
	return retardosDispositivos[i];
}

