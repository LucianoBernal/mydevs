/*
 * syscall.c
 *
 *  Created on: 11/07/2014
 *      Author: utnso
 */

#include "Kernel.h"
#include <string.h>

typedef struct {
	int valor;
	t_queue* procesosBloqueados;
	sem_t mutexCola;
} t_estructuraSemaforo;

t_dictionary* diccionarioSemaforos;
sem_t diccionarioSemaforosMutex;

int sc_obtener_valor(char id, int idCpu) {
	sem_wait(mutexVG);
	int a = dictionary_get(variables_globales, &id);
	send(idCpu, a, 4, 0); //TODO
	sem_wait(mutexVG);
	return a;
}

int sc_grabar_valor(char id, int valor) {
	sem_wait(mutexVG);
	dictionary_put(variables_globales, &id, &valor);
	sem_wait(mutexVG);
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
		seLiberoUnaCPU(idCPU);
		sem_wait(&colaReadyMutex);
		queue_push(colaReady, pcbADesbloquear);
		sem_post(&colaReadyMutex);
	}
}

void sc_wait(char* idSem, int idCpu) {
	sem_wait(&diccionarioSemaforosMutex);
	t_estructuraSemaforo* semaforo = dictionary_get(diccionarioSemaforos,
			idSem);
	int a = semaforo->valor;
	if (a > 0) {
		semaforo->valor = (semaforo->valor) - 1;
		send(idCpu, 's', 1, 0); //TODO
	} else {
		t_estructuraSemaforo* semaforo = dictionary_get(diccionarioSemaforos,
				idSem);
		int programID = programIdDeCpu(idCpu);
		sem_wait(&(semaforo->mutexCola));
		queue_push(semaforo->procesosBloqueados, &programID);
		sem_post(&(semaforo->mutexCola));
	}
	sem_post(&diccionarioSemaforosMutex);
}

int sc_imprimir() {
	//??
}

int sc_imprimirTexto(char* texto, int idCpu) {
	int programID = programIdDeCpu(idCpu);
	int sd = obtener_sd_Programa(programID);
	notificar_Programa(sd, texto);
	return strlen(texto);
}

void armarDiccionarioDeSemaforos() {
	int i = 0;
	semaforos = dictionary_create();
	sem_init(&diccionarioSemaforosMutex, 0, 1);
	while (i < cantidadDeSemaforos) {
		t_estructuraSemaforo* semaforo = malloc(sizeof(t_estructuraSemaforo));
		int* valor = list_get(semaforos, i);
		sem_t mutex;
		t_queue* cola = queue_create();
		sem_init(&mutex, 0, 1);
		semaforo->mutexCola = mutex;
		semaforo->procesosBloqueados = cola;
		semaforo->valor = *valor;
		sem_wait(&diccionarioSemaforosMutex);
		dictionary_put(diccionarioSemaforos, valor, semaforo);
		sem_post(&diccionarioSemaforosMutex);
		i++;
	}
}

int programIdDeCpu(int idCPU) {
	sem_wait(CPUsMutex);
	int posicion = posicionEnLaLista(CPUs, idCPU);
	t_estructuraCPU* CPU = list_get(CPUs, posicion);
	sem_post(CPUsMutex);
	return (CPU->idProceso);

}
