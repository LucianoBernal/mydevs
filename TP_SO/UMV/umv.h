/*
 * umv.h
 *
 *  Created on: 11/06/2014
 *      Author: utnso
 */

#ifndef UMV_H_
#define UMV_H_



#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <pthread.h>
#include <semaphore.h>
#include "consola_interfaz.h"
#include "atencioninterna_interfaz.h"

/*Constantes*/

/*Variables Globales*/
t_log* logger;
int tamanio_umv;
int retardo;
int puertoUMV;
bool algoritmo;
void *baseUMV;
extern t_list *listaProcesos;

pthread_t atencion_consola;
pthread_t atencion_interna;

typedef struct {
	int pid, activo;
	char tipo;
	t_list *tabla;
} t_tablaProceso;

t_tablaProceso vectorProcesos[10];

/*Funciones*/

int32_t validarConfig(t_config *config);
void cargarConfig(t_config *config);

#endif /* UMV_H_ */
