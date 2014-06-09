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

void ejecucion(t_PCB* pcb) {
	incrementarProgramCounter(pcb);
	solicitar_UMV_proxima_sentencia(pcb, instruccion);
	analizadorLinea(instruccion, funcionesAnSISOP, funcionesKernel);

}
int quantum;
bool terminar = 0;

//La idea es que sea mas complejo m3n
int laSenialDeTerminar() {
	return terminar;
}

t_PCB *conseguirPCB();

int main() {
	while (!laSenialDeTerminar) {
		t_PCB *PCB = conseguirPCB();
		int i = 0;
		char *aParsear, *aEjecutar;
		//es un while o es un for.
		for (i = 0; (i < quantum) && (!laSenialDeTerminar()); i++) {
			aParsear = buscarInstruccion(PCB);
			aEjecutar = parsear(aParsear);
			ejectuar(aEjectuar);
			notificarQuantum();
		}
		actualizarKernel(PCB);
	}
