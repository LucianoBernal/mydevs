/*
 * PLP.h
 *
 *  Created on: 27/05/2014
 *      Author: utnso
 */

#ifndef PLP_H_
#define PLP_H_

#include <biblioteca_comun/definiciones.h>



typedef struct {
	t_PCB * pcb;
	int peso;
} t_new;


void gestionarProgramaNuevo(const char* );
void deNewAReady();
void* plp_main(void*);


#endif /* PLP_H_ */
