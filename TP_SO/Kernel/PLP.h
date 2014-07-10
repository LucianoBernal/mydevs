/*
 * PLP.h
 *
 *  Created on: 27/05/2014
 *      Author: utnso
 */

#ifndef PLP_H_
#define PLP_H_

#include <biblioteca_comun/definiciones.h>
#include "Kernel.h"



typedef struct {
	t_PCB * pcb;
	int peso;
} t_new;


void gestionarProgramaNuevo(const char*, int );
void deNewAReady();
void* plp_main(void*);


#endif /* PLP_H_ */
