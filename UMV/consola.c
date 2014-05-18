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

typedef struct{
	char* identificador;
	int inicio;
	int tamano;
	int ubicacion;
}t_tsegmento;

typedef struct{
	int id_programa;
	t_tsegmento *baseTabla;
}t_tprogramas;



int main (){
	const int COMANDO_SIZE=70;
	char* comando=malloc(COMANDO_SIZE);
	//En realidad deberia leerlo de un archivo config
	//int tamanoMundo=10000;
	//void *baseUMV = malloc(tamanoMundo);
	//t_tprogramas *tablaProgramas;
	//tablaProgramas =(t_tprogramas *)baseUMV;
	do{
		gets(comando);
		analizarYEjecutar(comando);

	}while(strncmp("exit",comando,4));
	free(comando);
	return 0;
}
