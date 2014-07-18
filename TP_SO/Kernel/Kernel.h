/*
 * Kernel.h
 *
 *  Created on: 10/07/2014
 *      Author: utnso
 */

#ifndef KERNEL_H_
#define KERNEL_H_

#include "PLPinterface.h"
#include "PCPinterface.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <parser/parser/metadata_program.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/config.h>
#include <commons/log.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <commons/collections/dictionary.h>
#include <biblioteca_comun/definiciones.h>
#include <biblioteca_comun/Serializacion.h>
#include <biblioteca_comun/bibliotecaSockets.h>
#include <time.h>

int socketUMV;
sem_t  grado_Multiprogramacion;
char* ip_UMV;
char* puerto_UMV;
int puerto_programa;
int puerto_CPU;
int quantum;
int retardo;
int tamanio_stack;
int multiprogramacion;
t_queue* colaReady;
sem_t  colaReadyMutex;
sem_t  vacioReady;
sem_t mutexVG;
sem_t colaExitVacio;
sem_t colaExitMutex;
t_queue* colaExit;
int cantidadDeDispositivos;
int cantidadDeSemaforos;
t_list* valor_semaforos;
t_list* semaforos;
t_list* retardos;
t_list* idDispositivos;
t_dictionary* variables_globales;
char** variables_globales_aux;
char** valor_semaforos_aux;
char** semaforos_aux;
char** hio_aux;
char** idhio_aux;
t_log *logKernel;

void cargarConfig(t_config *);
int32_t validarConfig(t_config*);
/*void obtener_valor(char id, int idCpu);
void grabar_valor(char id, int valor);
void wait(char* idSem, int idCpu);
void signal(char* idSem);
int buscarValorSemaforo(char* semaforo);
int buscarPosicion(char* semaforo);*/

#endif /* KERNEL_H_ */
