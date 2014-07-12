/*
 * PCPinterface.h
 *
 *  Created on: 11/07/2014
 *      Author: utnso
 */

#ifndef PCPINTERFACE_H_
#define PCPINTERFACE_H_

#include <stdbool.h>
#include <commons/collections/list.h>
#include <biblioteca_comun/definiciones.h>

typedef struct {
	int idCPU;
	int estado;
	int idProceso;
} t_estructuraCPU;

void* pcp_main(void*);
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
//int estaLibre(int); FIXME Bely, hay dos funciones que se llaman estaLibre




#endif /* PCPINTERFACE_H_ */
