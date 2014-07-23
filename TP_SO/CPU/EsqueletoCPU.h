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
#include <parser/parser/parser.h>

typedef void (*sighandler_t)(int);
int socketUMV, socketKernel;
int sigusr1_activado;
t_log* logs;
t_dictionary* diccionarioDeVariables;
int proceso_terminado;
int proceso_bloqueado;
int quantumDeKernel;
int retardo;
char * etiquetas;
sighandler_t manejar_seniales (int);
void actualizarDiccionarioDeVariables (t_PCB*);
void recuperarDiccionario();
#endif /* ESQUELETOCPU_H_ */
