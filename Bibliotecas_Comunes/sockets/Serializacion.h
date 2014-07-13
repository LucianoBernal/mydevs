/*
 * Serializacion.h
 *
 *  Created on: 09/06/2014
 *      Author: utnso
 */

#ifndef SERIALIZACION_H_
#define SERIALIZACION_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include "bibSockets.h"


typedef struct{
	char *msj;
	char tamano;//un tamano de 255 PUEDE ser poco
	char cantVar;//pero es mejor que nada
}t_paquete;
typedef struct{
	int tamano;
	char *p_var;
}t_tamYDir;


t_tamYDir *crear_nodoVar(void *, int);
t_paquete *serializar2(t_tamYDir *uno, ...);
void desempaquetar2(char *, void *, ...);


#endif /* SERIALIZACION_H_ */
