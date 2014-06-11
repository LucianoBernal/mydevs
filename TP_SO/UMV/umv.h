/*
 * umv.h
 *
 *  Created on: 11/06/2014
 *      Author: utnso
 */

#ifndef UMV_H_
#define UMV_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <pthread.h>
#include <semaphore.h>
#include "consola.h"
#include "atencionInterna.h"

/*Variables Globales*/
extern uint32_t tamanio_umv;
extern void *baseUMV;
extern t_list *listaProcesos;

pthread_t atencion_consola;
pthread_t atencion_interna;

typedef struct {
	int pid, activo;
	char tipo;
	t_list *tabla;
} t_tablaProceso;

t_tablaProceso vectorProcesos[10];

/*Constantes*/

/*Funciones*/
int32_t validarConfig(t_config *config);
void cargarConfig(t_config *config);

#endif /* UMV_H_ */
