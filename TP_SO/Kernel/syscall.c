#include "syscall.h"

int sc_obtener_valor(char* id, int idCpu) {
	sem_wait(&mutexVG);
	int* a = dictionary_get(variables_globales, id);
	send(idCpu, a, 4, 0);
	sem_wait(&mutexVG);
	return *a;
}

int sc_grabar_valor(char* id, int valor) {
	sem_wait(&mutexVG);
	dictionary_put(variables_globales, id, &valor);
	sem_wait(&mutexVG);
	return valor;
}

void sc_signal(char* idSem, int idCPU) {
	sem_wait(&diccionarioSemaforosMutex);
	t_estructuraSemaforo* semaforo = dictionary_get(diccionarioSemaforos,
			idSem);
	semaforo->valor = (semaforo->valor) + 1;
	sem_post(&diccionarioSemaforosMutex);
	if (!queue_is_empty(semaforo->procesosBloqueados)) {
		sem_wait(&(semaforo->mutexCola));
		t_PCB* pcbADesbloquear = queue_pop(semaforo->procesosBloqueados);
		sem_post(&(semaforo->mutexCola));
		sem_wait(&colaReadyMutex);
		queue_push(colaReady, pcbADesbloquear);
		sem_post(&colaReadyMutex);
	}
}

void sc_wait(char* idSem, t_PCB* pcb, int idCPU) {
	int * estado_semaforo=malloc(sizeof(int));
	sem_wait(&diccionarioSemaforosMutex);
	t_estructuraSemaforo* semaforo = dictionary_get(diccionarioSemaforos,
			idSem);
	int a = semaforo->valor;
	if (a > 0) {
		semaforo->valor = (semaforo->valor) - 1;
		*estado_semaforo=1;
		//serializar2(estado_semaforo,0);
		send(idCPU, estado_semaforo, 4, 0); //(puede seguir)
	} else {
		t_estructuraSemaforo* semaforo = dictionary_get(diccionarioSemaforos,
				idSem);
		*estado_semaforo=0;
		//serializar2(estado_semaforo,0);
		send(idCPU, estado_semaforo, 4, 0); //(se libera)
		seLiberoUnaCPU(idCPU);
		sem_wait(&(semaforo->mutexCola));
		queue_push(semaforo->procesosBloqueados, pcb);
		sem_post(&(semaforo->mutexCola));
	}
	sem_post(&diccionarioSemaforosMutex);
}

int sc_imprimir() {
	//TODO ?
	return 0;

}

int sc_imprimirTexto(char* texto, int idCpu) {
	int programID = programIdDeCpu(idCpu);
	int sd = obtener_sd_Programa(programID);
	notificar_Programa(sd, texto);
	return strlen(texto);
}


int programIdDeCpu(int idCPU) {
	sem_wait(&CPUsMutex);
	int posicion = posicionEnLaLista(CPUs, idCPU);
	t_estructuraCPU* CPU = list_get(CPUs, posicion);
	sem_post(&CPUsMutex);
	return (CPU->idProceso);

}
