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
typedef struct{
	char *msj;
	char tamano;//un tamano de 255 PUEDE ser poco
	char cantVar;//pero es mejor que nada
}t_paquete;

#define IP "127.0.0.1"
#define PACKAGESIZE 1024
#define PUERTO "6666"

void Desempaquetar(char *, t_queue *);
t_paquete *Serializar(t_queue *);
t_tamYDir *crear_nodoVar(void *, int);

int main(){
	char *a="pato ", *b ="es", *c=" pro", *e=malloc(10), *d=malloc(10), *f=malloc(10);
	t_queue *queue=queue_create();
	t_queue *queue2=queue_create();
	queue_push(queue, crear_nodoVar(a, strlen(a)));
	queue_push(queue, crear_nodoVar(b, strlen(b)));
	queue_push(queue, crear_nodoVar(c, strlen(c)));
//	El mecanismo seria este, pusheamos a una cola las variables y su tamaño con esa funcion
	t_paquete *paquete=Serializar(queue);
	queue_push(queue2, d);
	queue_push(queue2, e);
	queue_push(queue2, f);
//  y del otro lado pusheamos una cola con punteros a las variables
	Desempaquetar(paquete->msj, queue2);
	printf("%s%s%s", d, e, f);
//	ay estas funciones que me dicen cosas lindas (?
	return 0;
}

//	Es un poco tonto que no le mande a las funciones la cantidad de variables a serializar
//	y le mande una lista con esa cantidad implicitamente, si se les ocurre una forma mas
//	practica mejor.

t_paquete *Serializar(t_queue *cola){
	t_paquete *paquete=malloc(sizeof(t_paquete));
	t_tamYDir *aux;
	paquete->msj=malloc(50);
	paquete->cantVar=0;
	int acum=0;
	while(!queue_is_empty(cola)){
		aux=queue_pop(cola);
		memcpy(acum+(paquete->msj), &(aux->tamano), 1);
		memcpy(acum+1+(paquete->msj),aux->p_var, aux->tamano);
		acum+=((aux->tamano)+1);
		paquete->cantVar++;
	}
	paquete->tamano=acum;
	return paquete;
}

void Desempaquetar(char *msjRecibido, t_queue *cola){
	void *aux=malloc(50);
	char tamano;
	int acum=0;
	while(!queue_is_empty(cola)){
		memcpy(&tamano, msjRecibido+acum, 1);
		aux=queue_pop(cola);
		memcpy(aux, msjRecibido+acum+1, tamano);
		acum+=(tamano+1);
	}
}

t_tamYDir *crear_nodoVar(void *p_var, int tamano){
	t_tamYDir *nuevo=malloc(sizeof(t_tamYDir));
	nuevo->p_var=p_var;
	nuevo->tamano=tamano;
	return nuevo;
}

