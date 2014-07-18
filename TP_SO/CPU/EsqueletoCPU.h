/*
 * EsqueletoCPU.h
 *
 *  Created on: 18/07/2014
 *      Author: utnso
 */

#ifndef ESQUELETOCPU_H_
#define ESQUELETOCPU_H_

#include <commons/log.h>
#include <signal.h>

int socketUMV, socketKernel;
int sigusr1_activado;
t_log* log;

void manejar_seniales(int );
#endif /* ESQUELETOCPU_H_ */
