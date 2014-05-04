/*
 * consola.c
 *
 *  Created on: 03/05/2014
 *      Author: utnso
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
void analizarYEjecutarComando(char *);

int main (){
	//En realidad deberia leerlo de un archivo config
	//int tamanoMundo=10000;
	char comando[4];
	//void *baseUMV = malloc(tamanoMundo);
	//t_tprogramas *tablaProgramas;
	//tablaProgramas =(t_tprogramas *)baseUMV;
	do{
		gets(comando);
		analizarYEjecutarComando(comando);
	}while(strcmp(comando, "exit\n"));
	return 0;
}

void analizarYEjecutarComando(char *com){
	printf("%s\n",com);
	return;
}
