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
t_dictionary* diccionarioDeVariables;
int proceso_terminado;
int proceso_bloqueado;
//void procesarPCB(t_PCB*);
int quantumDeKernel;
int retardo;
//void ejecutar_linea(t_PCB*);
//void regenerar_diccionario();
//
//void manejar_seniales(int );
#endif /* ESQUELETOCPU_H_ */
