/*
 * Kernel.h
 *
 *  Created on: 10/07/2014
 *      Author: utnso
 */

#ifndef KERNEL_H_
#define KERNEL_H_

#include "PLPinterface.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <parser/metadata_program.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/config.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <commons/collections/dictionary.h>
#include <biblioteca_comun/definiciones.h>



static sem_t * grado_Multiprogramacion;
static int puerto_programa;
static int puerto_CPU;
static int quantum;
static int retardo;
static int multiprogramacion;
static t_queue* colaReady;
static t_dictionary* variables_globales;
static int semaforos[];
static char* valor_semaforos[];
static int hio[];
static char* idhio[];
static sem_t * colaReadyMutex;
static sem_t * vacioReady;
static sem_t* mutexVG;
static sem_t* colaExitVacio;
static sem_t* colaExitMutex;
static t_queue* colaExit;

#endif /* KERNEL_H_ */
