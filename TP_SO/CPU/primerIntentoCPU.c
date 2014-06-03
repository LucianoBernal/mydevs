/*
 * primerIntentoCPU.c
 *
 *  Created on: 30/05/2014
 *      Author: utnso
 */

#include <biblioteca_comun/definiciones.h>
#include <parser/parser.h>

char* const instruccion;
static AnSISOP_funciones * funcionesAnSISOP;
static AnSISOP_kernel * funcionesKernel;

void ejecucion(t_PCB* pcb){
	incrementarProgramCounter(pcb);
	solicitar_UMV_proxima_sentencia(pcb,instruccion);
	analizadorLinea(instruccion,funcionesAnSISOP,funcionesKernel);

}
