/*
 * PLP.h
 *
 *  Created on: 27/05/2014
 *      Author: utnso
 */

#ifndef PLP_H_
#define PLP_H_

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <parser/metadata_program.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <semaphore.h>
#include <biblioteca_comun/Definiciones.h>



typedef struct {
	t_PCB * pcb;
	int peso;
} t_new;


void gestionarProgramaNuevo(const char* );
void deNewAReady();


#endif /* PLP_H_ */
