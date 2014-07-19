/*
 * atencionInterna.h
 *
 *  Created on: 11/06/2014
 *      Author: utnso
 */

#ifndef ATENCIONINTERNA_H_
#define ATENCIONINTERNA_H_

#include "umv.h"
#include "pruebasUMV_interfaz.h"
/*Constantes*/
#define MAX_CPUS 25;

/*Variables Globales*/
int32_t cantCpu;
pthread_t hiloKernel;
pthread_t hiloCpu;
extern t_log*logger;


#endif /* ATENCIONINTERNA_H_ */
