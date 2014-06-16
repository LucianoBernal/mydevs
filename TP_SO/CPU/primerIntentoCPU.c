/*
 * primerIntentoCPU.c
 *
 *  Created on: 30/05/2014
 *      Author: utnso
 */

#include <biblioteca_comun/definiciones.h>
#include <parser/parser.h>
#include "funcionesParser.h"

//Demas esta decir que por lo menos los puertos deberian ser diferentes.
#define IP_KERNEL, IP_UMV "127.0.0.1" //Deberiamos obtenerla de archivo config
#define PUERTO_PCP, PUERTO_UMV "3000" //Gran numero si los hay

char* const instruccion;
static AnSISOP_funciones * funcionesAnSISOP;
static AnSISOP_kernel * funcionesKernel;
char *etiquetas;

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

//t_PCB *conseguirPCB();
/*
int main() {
	struct addrinfo *serverInfo;
	obtenerAddrInfoServer(IP_KERNEL, PUERTO_PCP, &serverInfo); //Estan en bibsockets2
	int socketKernel=crearSocket(serverInfo);
	conectarSocket(socketKernel, serverInfo);
	obtenerAddrInfoServer(IP_UMV, PUERTO_UMV, &serverInfo);
	int socketUMV=crearSocket(serverInfo);
	conectarSocket(socketUMV, serverInfo);

	while (!laSenialDeTerminar) {
		t_PCB *PCB = conseguirPCB();
		etiquetas = solicitarBytesAUMV(*(PCB->indice_de_Etiquetas), 0, PCB->tamanio_Indice_de_Etiquetas);
		int ubInstruccion, largoInstruccion;
		int i = 0;
		//es un while o es un for.
		for (i = 0; (i < quantum) && (!laSenialDeTerminar()); i++) {
			//Si empieza en la instruccion 0 deberia ser progra_Counter-1
			char *msjInstruccion = solicitarBytesAUMV(*(PCB->indice_de_Codigo), PCB->program_Counter*8, 8);
			desempaquetar2(msjInstruccion, &ubInstruccion, &largoInstruccion, 0);
			char *literalInstruccion = solicitarBytesAUMV(*(PCB->segmento_Codigo), ubInstruccion, largoInstruccion);
			//Con eso deberiamos tener el literal.
			analizadorLinea(literalInstruccion, funcionesAnSISOP, funcionesKernel);
			PCB->program_Counter++;
			//No se bien que mandarle
			//Pero con eso deberia ejecutar
			//notificarQuantum();
		}
		//actualizarKernel(PCB);
	}
	return 0;
}
*/
