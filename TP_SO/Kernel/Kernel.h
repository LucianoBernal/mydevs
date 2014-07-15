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
#include <sockets/Serializacion.h>



sem_t * grado_Multiprogramacion;
int puerto_programa;
int puerto_CPU;
int quantum;
int retardo;
int tamanio_stack;
int multiprogramacion;
t_queue* colaReady;
sem_t * colaReadyMutex;
sem_t * vacioReady;
sem_t* mutexVG;
sem_t* colaExitVacio;
sem_t* colaExitMutex;
t_queue* colaExit;
t_dictionary* variables_globales;
static int cantidadDeDispositivos;
static int cantidadDeSemaforos;
static int valor_semaforos[];
static char* semaforos[];
static int hio[];
static char* idhio[];
static t_dictionary* variables_globales;
char** variables_globales_aux;
char** valor_semaforos_aux;
char** semaforos_aux;
char** hio_aux;
char** idhio_aux;

void cargarConfig(t_config *);
int32_t validarConfig(t_config*);
/*void obtener_valor(char id, int idCpu);
void grabar_valor(char id, int valor);
void wait(char* idSem, int idCpu);
void signal(char* idSem);
int buscarValorSemaforo(char* semaforo);
int buscarPosicion(char* semaforo);*/

#endif /* KERNEL_H_ */
