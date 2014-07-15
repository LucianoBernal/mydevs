/*
 * syscall.c
 *
 *  Created on: 11/07/2014
 *      Author: utnso
 */

#include "Kernel.h"
#include <string.h>
/*
typedef struct {
	int valor;
	t_queue* procesosBloqueados;
} t_estructuraSemaforo;

t_dictionary* semaforos;

int obtener_valor(char id, int idCpu) {
	sem_wait(mutexVG);
	int a = dictionary_get(variables_globales, &id);
	send(idCpu, a, 4, 0); //TODO
	sem_wait(mutexVG);
	return a;
}

int grabar_valor(char id, int valor) {
	sem_wait(mutexVG);
	dictionary_put(variables_globales, &id, &valor);
	sem_wait(mutexVG);
	return valor;
}

void signal(char* idSem) {
	t_estructuraSemaforo* semaforo = dictionary_get(semaforos, idSem);
	semaforo->valor = (semaforo->valor) + 1;
	if (!queue_is_empty(semaforo->procesosBloqueados)) {
		int * programaADesbloquear = queue_pop(semaforo->procesosBloqueados);
		int sd = obtener_sd_Programa(*programaADesbloquear);
		send(sd, 's', 1, 0);
	}
}

void wait(char* idSem, int idCpu) {
	t_estructuraSemaforo* semaforo = dictionary_get(semaforos, idSem);
	int a = semaforo->valor;
	if (a > 0) {
		semaforo->valor = (semaforo->valor) - 1;
		send(idCpu, 's', 1, 0); //TODO
	} else {
		t_estructuraSemaforo* semaforo = dictionary_get(semaforos, idSem);
		int programID = programIdDeCpu(idCpu);
		queue_push(semaforo->procesosBloqueados, &programID);
	}
}

int imprimir(){
//??
}

int imprimirTexto(char* texto, int idCpu) {
	int programID = programIdDeCpu(idCpu);
	int sd = obtener_sd_Programa(programID);
	notificar_Programa(sd,texto);
	return strlen(texto);
}

void armarDiccionarioDeSemaforos() {
	int i = 0;
	while (i < cantidadDeSemaforos) {
		t_estructuraSemaforo* semaforo = malloc(sizeof(t_estructuraSemaforo));
		semaforo->procesosBloqueados = queue_create();
		int* valor = list_get(semaforos,i);
		semaforo->valor = *valor;
		dictionary_put(semaforos, valor, semaforo);
		i++;
	}
}

int programIdDeCpu(int idCPU) {
	int posicion = posicionEnLaLista(CPUs, idCPU);
	t_estructuraCPU* CPU = list_get(CPUs, posicion);
	return (CPU->idProceso);

}*/
