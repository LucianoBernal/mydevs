/*
 * atencionInterna.h
 *
 *  Created on: 11/06/2014
 *      Author: utnso
 */

#ifndef ATENCIONINTERNA_H_
#define ATENCIONINTERNA_H_

#include "umv.h"
#include <sockets/bibSockets2.h>

/*Constantes*/
#define MAX_CPUS 25;

/*Variables Globales*/
int32_t cantCpu;
pthread_t hiloKernel;
pthread_t hiloCpu;

/*Funciones*/
void atencionInterna();
void atencionKernel();
void atencionCpu();

#endif /* ATENCIONINTERNA_H_ */
