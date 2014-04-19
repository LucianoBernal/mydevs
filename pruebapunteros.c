/*
 * pruebapunteros.c
 *
 *  Created on: 19/04/2014
 *      Author: utnso
 */

#include <stdio.h>
#include <stdlib.h>

void meterVariable(void **direccion_sp, char nom, int valor){
	//Castear es horrible...
	*(char *)*direccion_sp = nom;
	*direccion_sp +=1;
	*(int *)*direccion_sp = valor;
	*direccion_sp +=4;
	return;
}

void sacarVariable(void **direccion_sp, char *p_nom, int *p_val){
	//pero no temais.
	*direccion_sp-=4;
	*p_val = *(int *)*direccion_sp;
	*direccion_sp-=1;
	*p_nom = *(char *)*direccion_sp;
	printf("los valores adentro son: %c y %d \n", *p_nom, *p_val);
	return;
}
