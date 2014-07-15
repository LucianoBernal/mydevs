/*
 * syscall.c
 *
 *  Created on: 11/07/2014
 *      Author: utnso
 */

#include "Kernel.h"
// valor_semaforos[];
// semaforos[];

typedef struct{
	int valor;
	t_queue* procesosBloqueados;
} t_estructuraSemaforo;

t_dictionary* semaforos;

void obtener_valor(char id, int idCpu) {
	sem_wait(mutexVG);
	int a = dictionary_get(variables_globales, &id);
	send(idCpu, a, strlen(a), 0); //TODO
	sem_wait(mutexVG);
}
void grabar_valor(char id, int valor) {
	sem_wait(mutexVG);
	dictionary_put(variables_globales, &id, &valor);
	sem_wait(mutexVG);
}

void signal(char* idSem) {
	t_estructuraSemaforo* semaforo = dictionary_get(semaforos, idSem);

}

void wait(char* idSem, int idCpu) {
	int a = buscarValorSemaforo(idSem);
	int pos = buscarPosicion(idSem);
	if (a > 0) {
		semaforos[pos] = semaforos[pos] - 1;
		send(idCpu, 's', strlen(), 0); //ya sé que esto está mal xD
	}
	else
	{
		//encolar el proceso
	}
}
int buscarValorSemaforo(char* semaforo) {
	int i = 0;
	while (valor_semaforos[i] != semaforo) {
		i++;
	}
	return semaforos[i];
}

int buscarPosicion(char* semaforo) {
	int i = 0;
	while (valor_semaforos[i] != semaforo) {
		i++;
	}
	return i;
}

void armarDiccionarioDeSemaforos()
{
	int i = 0;
	while (i<cantidadDeSemaforos){
		t_estructuraSemaforo* semaforo = malloc(sizeof(t_estructuraSemaforo));
		semaforo->procesosBloqueados= queue_create();
		semaforo->valor = valor_semaforos[i];
		dictionary_put(semaforos,semaforos[i],semaforo);
		i++;
	}
}
