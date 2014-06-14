/*
 * consola.h
 *
 *  Created on: 11/06/2014
 *      Author: utnso
 */

#ifndef CONSOLA_H_
#define CONSOLA_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "commons/collections/list.h"
//#include "funcionesaux.h"
#include "pruebasUMV.h"

/*Constantes*/

/*Variables Globales*/
typedef struct{
	//int32_t proceso;
	int32_t base;
	int32_t offset;
	int32_t tamano;
	char accion;
	char *archivo;
}t_operacion;

/*Funciones*/
int consola();

#endif /* CONSOLA_H_ */
