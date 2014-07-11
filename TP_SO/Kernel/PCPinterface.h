/*
 * PCPinterface.h
 *
 *  Created on: 11/07/2014
 *      Author: utnso
 */

#ifndef PCPINTERFACE_H_
#define PCPINTERFACE_H_
#include <stdbool.h>
void pcp_main();
void crearHilosPrincipales();
void* mandarAEjecutar(void*);
void enviarCPU();
void* bloquearYDevolverAReady(void*);
int buscarRetardo(char*);
int encontrarPrimeraCpuLibreYOcuparla(t_list*);
bool estaLibre(t_estructuraCPU*);
void nuevaCPU(int);
void programaSalioPorQuantum(t_PCB*, int);
void moverAColaExit(t_PCB*, int);
void programaSalioPorBloqueo(t_PCB*, int, char*, int);
void seLiberoUnaCPU(int);
void cpu_destroy(t_estructuraCPU*);
bool tieneID(t_estructuraCPU*);
void seDesconectoCPU(int);
void seDesconectoCPUSigusr(int, t_PCB* );
int posicionEnLaLista(t_list*, int);
int estaLibre(int);




#endif /* PCPINTERFACE_H_ */
