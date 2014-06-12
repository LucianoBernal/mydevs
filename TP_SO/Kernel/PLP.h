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

typedef struct{
	char *msj;
	int tamano;//un tamano de 255 PUEDE ser poco
	char cantVar;//pero es mejor que nada
}t_paquete;


void gestionarProgramaNuevo(const char* );
void deNewAReady();


#endif /* PLP_H_ */
