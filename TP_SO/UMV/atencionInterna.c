/*
 * atencionInterna.c
 *
 *  Created on: 11/06/2014
 *      Author: utnso
 */
#include "atencionInterna.h"

void atencionInterna() {
	while (1) {

		//Creo hilo que atiende al kernel.
			if (pthread_create(&hiloKernel, NULL, (void *) atencionKernel, NULL )) {
				//ERROR. Acá van cosas con log.
			} else {
				//Se creó correctamente.
			}
	//Espero conexión de cpu.
	//Si conexión es correcta, entonces:
	 cantCpu++;
	//Creo hilo que atiende a las cpu's.
			if (pthread_create(&hiloCpu[cantCpu], NULL, (void *) atencionCpu, NULL )) {
				//ERROR. Acá van cosas con log.
			} else {
				//Se creó correctamente.
			}
	}

}
