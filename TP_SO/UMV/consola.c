/*
 * consola.c
 *
 *  Created on: 03/05/2014
 *      Author: utnso
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "funcionesaux.h"

//const int CANT_PROCESOS =20;
typedef struct {
	int identificador;
	int inicioLogico;
	int tamano;
	void* memPpal;
}t_tablaSegmento;

typedef struct{
	int pid;
	char *tipo;
	t_tablaSegmento *tabla;
	int activo;
}t_tablaProceso;

t_tablaProceso vectorProcesos[20];

int cantProcesos;

int buscarPid(int pid){
	int i;
	for(i = 0; i < cantProcesos; i++){
		if (vectorProcesos[i].pid==pid) return i;
	}
	printf("algo malo paso");
	return 0;
}
*/


int main2 (){
	const int COMANDO_SIZE=70;
	char* comando=malloc(COMANDO_SIZE);
	//En realidad deberia leerlo de un archivo config
	//int tamanoMundo=10000;
	//void *baseUMV = malloc(tamanoMundo);
	//t_tprogramas *tablaProgramas;
	//tablaProgramas =(t_tprogramas *)baseUMV;
	do{
		gets(comando);
//		analizarYEjecutar(comando);

	}while(strncmp("exit",comando,4));
	free(comando);
	return 0;
}
