/*
 * consola.h
 *
 *  Created on: 11/06/2014
 *      Author: utnso
 */

#ifndef CONSOLA_H_
#define CONSOLA_H_


#include "umv.h"
#include "pruebasUMV_interfaz.h"

/*Constantes*/

/*Variables Globales*/
typedef struct{
	//int32_t proceso;
	int32_t base;
	int32_t offset;
	int32_t tamano;
	char accion;
	char* accionCadena;
	char *archivo;
}t_operacion;

/*Funciones*/


#endif /* CONSOLA_H_ */
