/*
 * PLP.h
 *
 *  Created on: 27/05/2014
 *      Author: utnso
 */

#ifndef PLPinterface_H_
#define PLPinterface_H_

#include<biblioteca_comun/definiciones.h>
#include<parser/parser/metadata_program.h>
#include<commons/collections/queue.h>

typedef struct {
	t_PCB * pcb;
	int peso;
} t_new;

int intentar_sacar_de_Programas_Finalizados(int);
void agregar_a_Programas_Finalizados(int);
int buscarSocketenVector(int,int*);
void cerrarSocketPrograma(int);
int obtener_pid_de_un_sd(int);
void liberar_nodo_Diccionario_PIDySD(int);
void mostrar_todas_Las_Listas(void);
void crear_Nuevo_Proceso( int );
void gestionarProgramaNuevo(char*, int ,int);
void* deNewAReady(void*);
void* plp_main(void*);
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
int crearSegmentos_Memoria(t_metadata_program *, t_PCB *, char* ,int);
void escribir_en_Memoria(t_metadata_program *, t_PCB *,	 char *,int);
void agregar_En_Diccionario(int , int );
void notificar_Memoria_Llena(int);
void encolar_en_Ready(t_PCB*);
void imprimirNodosNew(t_new*);
void mostrarListaNew();
void imprimirNodosPCBs(t_PCB*);
void mostrarColaDePCBs(t_queue*);
void notificar_Programa(int,char*);
void solicitar_Destruccion_Segmentos();
void enviar_Mensaje_Final(int);
void cerrar_conexion(int);
void* manejoColaExit(void* );
int obtener_sd_Programa(int);
void enviar_Mensaje_Final(int);



#endif /* PLP_H_ */
