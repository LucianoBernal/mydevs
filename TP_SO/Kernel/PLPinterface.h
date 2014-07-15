/*
 * PLP.h
 *
 *  Created on: 27/05/2014
 *      Author: utnso
 */

#ifndef PLPinterface_H_
#define PLPinterface_H_

#include<biblioteca_comun/definiciones.h>
#include<parser/metadata_program.h>
#include<commons/collections/queue.h>

typedef struct {
	t_PCB * pcb;
	int peso;
} t_new;


void gestionarProgramaNuevo(const char*, int );
void* deNewAReady(void*);
void* plp_main(int*);
void* manejoColaExit(void*);
void* deNewAReady(void*);
void asignaciones_desde_metada(t_metadata_program*, t_PCB*);
bool esDistintoANumAleatorio(int* );
int estaRepetido();
int generarProgramID();
t_new *crear_nodoNew(t_PCB* , int );
bool menor_Peso(t_new *, t_new *);
void encolar_New(t_PCB* , int );
int calcularPeso(t_metadata_program* );
bool esIgualANumABorrar(int* );
void liberar_numero(int );
int solicitar_Memoria(t_metadata_program *, t_PCB *,const char* );
void escribir_en_Memoria(t_metadata_program *, t_PCB *,	const char *);
void agregar_En_Diccionario(int , int );
void notificar_Memoria_Llena(int);
void encolar_en_Ready(t_PCB*);
void imprimirNodosNew(t_new*);
void mostrarListaNew();
void imprimirNodosPCBs(t_PCB*);
void mostrarColaDePCBs(t_queue*);
void notificar_Programa(int,char*);
void solicitar_Destruccion_Segmentos(t_PCB*);
void enviar_Mensaje_Final(int);
void cerrar_conexion(int);
void* manejoColaExit(void* );
int obtener_sd_Programa(int);
void enviar_Mensaje_Final(int);



#endif /* PLP_H_ */
