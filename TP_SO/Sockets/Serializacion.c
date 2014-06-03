#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include "bibSockets.h"

typedef struct{
	int tamano;
	char *p_var;
}t_tamYDir;

#define IP "127.0.0.1"
#define PACKAGESIZE 1024
#define PUERTO "6666"

char *armateUnoHernan(t_queue *colacha){
	char *myStream;
	int acum=0, i;
	t_tamYDir *aux;
	for(i=1 ; i<=queue_size(colacha);){
		aux = (t_tamYDir *)queue_pop(colacha);
		memcpy(myStream+acum, aux, 4);
		memcpy(myStream+acum, aux->p_var, aux->tamano);
		acum+=(aux->tamano+4);
	}
	return myStream;
}

int main(){
	int cantVariables, i;
	char vec[10][10];
	t_queue *cola;
	t_tamYDir aux;
	printf("ingrese la cantidad de variables a ingresar:");
	scanf("%d", &cantVariables);
	for (i=0 ; i<cantVariables ; i++){
		printf("\ningrese la %d variable:", i+1);
		scanf("%s", vec[i]);
	}
	for (i=0 ; i<cantVariables ; i++){
		aux.tamano = strlen(vec[i]);
		aux.p_var = vec[i];
		queue_push(cola, (void*)&aux);
	}
	//Hasta aca es medio comodito porque ya son las 11
	int serverSocket;
	crearSocketS(&serverSocket, PUERTO);
	//Bla
	//send(serverSocket, armateUnoHernan(cola), PACKAGESIZE, 0);
	printf("%s20", armateUnoHernan(cola));
	return 0;
}
