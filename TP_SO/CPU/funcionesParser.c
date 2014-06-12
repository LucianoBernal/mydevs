/*
 * funcionesParser.c
 *
 *  Created on: 09/06/2014
 *      Author: utnso
 */
#include <biblioteca_comun/definiciones.h>
#include <parser/parser.h>
#include <parser/metadata_program.h>
#include "funcionesParser.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#define TAMANO_CABECERA 16

void vincularPrimitivas() {
	funciones_Ansisop.AnSISOP_asignar = asignar;
	funciones_Ansisop.AnSISOP_asignarValorCompartida = asignarValorCompartida;
	funciones_Ansisop.AnSISOP_definirVariable = definirVariable;
	funciones_Ansisop.AnSISOP_dereferenciar = dereferenciar;
	funciones_Ansisop.AnSISOP_entradaSalida = entradaSalida;
	funciones_Ansisop.AnSISOP_finalizar = finalizar;
	funciones_Ansisop.AnSISOP_imprimir = imprimir;
	funciones_Ansisop.AnSISOP_imprimirTexto = imprimirTexto;
	funciones_Ansisop.AnSISOP_irAlLabel = irAlLabel;
	funciones_Ansisop.AnSISOP_llamarConRetorno = llamarConRetorno;
	funciones_Ansisop.AnSISOP_llamarSinRetorno = llamarSinRetorno;
	funciones_Ansisop.AnSISOP_obtenerPosicionVariable = obtenerPosicionVariable;
	funciones_Ansisop.AnSISOP_obtenerValorCompartida = obtenerValorCompartida;
	funciones_Ansisop.AnSISOP_retornar = retornar;
	funciones_kernel.AnSISOP_signal = signal;
	funciones_kernel.AnSISOP_wait = wait;

}
int socketUMV, socketKernel;
int programCounter;
t_puntero stackBase;
t_puntero desplazamiento;
t_dictionary *diccionario;
t_puntero cursorCtxto;
char *etiquetas;
t_PCB *pcb;

typedef enum {
	SEGMENTATION_FAULT,
	MEMORY_OVERLOAD,
	MOSTRAR_VALOR,
	MOSTRAR_TEXTO,
	ENTRADA_SALIDA,
	OBTENER_VALOR_COMPARTIDA,
	ASIGNAR_VALOR_COMPARTIDA,
	CREAR_SEGMENTO,
	CREAR_SEGMENTOS_PROGRAMA,
	DESTRUIR_SEGMENTOS,
	ESCRIBIR_EN_UMV,
	ESCRIBIR_EN_UMV_OFFSET_CERO,
	ENVIAR_PCB,
	SOLICITAR_A_UMV,
	PEDIR_ETIQUETAS,
	PEDIR_INSTRUCCION
}codigos_mensajes;

typedef struct {
	t_nombre_variable identificador_variable;
	t_valor_variable valor_variable;
} t_variable;

typedef struct {
	char *msj;
	int tamano; //un tamano de 255 PUEDE ser poco
	char cantVar; //pero es mejor que nada
} t_paquete;

typedef struct {
	t_nombre_variable identificador_variable;
	t_puntero desplazamiento;
} t_variable_diccionario;
typedef struct{
	int tamano;
	char *p_var;
}t_tamYDir;

t_tamYDir *crear_nodoVar(void *, int);
t_paquete *serializar2(t_tamYDir *uno, ...);
void desempaquetar2(char *, void *, ...);

t_paquete *serializar2(t_tamYDir *uno, ...){
	va_list(p);
	va_start(p, uno);
	int acum = 0;
	t_tamYDir *arg=uno;
	t_paquete *paquete=malloc(sizeof(t_paquete));
	paquete->msj=malloc(50);
	do{
		memcpy(acum+(paquete->msj), &(arg->tamano), 4);
		memcpy(acum+4+(paquete->msj), arg->p_var, arg->tamano);
		acum+=((arg->tamano)+4);
	}while((arg = va_arg(p, t_tamYDir *)));
	va_end(p);
	paquete->tamano=acum;
	return paquete;
}
void desempaquetar2(char *msjRecibido, void *uno, ...){
	va_list(p);
	va_start(p, uno);
	void * arg=uno;
	int tamano;
	int acum=0;
	do{
		memcpy(&tamano, msjRecibido+acum, 4);
		memcpy(arg, msjRecibido+acum+4, tamano);
		acum+=(tamano+4);
	}while((arg = va_arg(p, void*)));

}
t_tamYDir *crear_nodoVar(void *p_var, int tamano){
	t_tamYDir *nuevo=malloc(sizeof(t_tamYDir));
	nuevo->p_var=p_var;
	nuevo->tamano=tamano;
	return nuevo;
}

static t_variable_diccionario *crear_nodoDiccionario(
		t_nombre_variable identificador, int desplazamiento) {
	t_variable_diccionario *nuevo = malloc(sizeof(t_variable_diccionario));
	nuevo->identificador_variable = identificador;
	nuevo->desplazamiento = desplazamiento;
	return nuevo;
}
/*
static void destruir_variable(t_variable *self) {
	free(self);
}
*/
//claramente a este socket hace falta crearlo (y incluir las librerias)
void enviarBytesAUMV(t_puntero base, t_puntero desplazamiento, void *datos,
		int tamano) {
	t_paquete *paquete = serializar2(crear_nodoVar(&base, sizeof(t_puntero)),
			crear_nodoVar(&desplazamiento, sizeof(t_puntero)),
			crear_nodoVar(datos, tamano), 0);
	//Aca faltaria un queue_push(queue, crear_nodoVar(ID MENSAJE, 1));
	int *razon = malloc(sizeof(int));
	*razon = ESCRIBIR_EN_UMV;
	t_paquete *header = serializar2(crear_nodoVar(&(paquete->tamano), 4),
			crear_nodoVar(razon, 4), 0);
	send(socketUMV, header->msj, TAMANO_CABECERA, 0);
	send(socketUMV, paquete->msj, paquete->tamano, 0);
	//tambien deberia escuchar un mensaje de confirmacion
}
char *solicitarBytesAUMV(t_puntero base, t_puntero desplazamiento, int tamano) {
	t_paquete *paquete = serializar2(crear_nodoVar(&base, sizeof(t_puntero)),
			crear_nodoVar(&desplazamiento, sizeof(t_puntero)),
			crear_nodoVar(&tamano, 4));
	//Tambien faltaria lo mismo que en enviarbytes
	int *razon = malloc(sizeof(int));
	*razon = SOLICITAR_A_UMV;
	t_paquete *header = serializar2(crear_nodoVar(&(paquete->tamano), 4),
			crear_nodoVar(razon, 4));
	send(socketUMV, header->msj, TAMANO_CABECERA, 0);
	send(socketUMV, paquete->msj, paquete->tamano, 0);
	//bind y listen mediante
	char *msjCabecera = malloc(TAMANO_CABECERA);
	recv(socketUMV, msjCabecera, TAMANO_CABECERA, MSG_WAITALL);
	int tamanoMensaje;
	desempaquetar2(msjCabecera, &tamanoMensaje);
	char *mensaje = malloc(tamanoMensaje);
	recv(socketUMV, mensaje, tamanoMensaje, MSG_WAITALL);
	char *aux = malloc(tamano);
	desempaquetar2(mensaje, aux);
	return aux;
} //UFF SOY TAN HOMBRE

t_puntero definirVariable(t_nombre_variable identificador_variable) {
	enviarBytesAUMV(stackBase, desplazamiento, &identificador_variable, 1);
	t_variable_diccionario *aux = crear_nodoDiccionario(identificador_variable,
			desplazamiento);
	desplazamiento += 5; //PIENSO que en el diccionario se guarda un puntero al COMIENZO de la variable
	dictionary_put(diccionario, &(aux->identificador_variable), aux);
	return stackBase + desplazamiento - 4; //El tp dice posicion, para mi es desplazamiento nomas
}

t_puntero obtenerPosicionVariable(t_nombre_variable identificador_variable) {
	t_variable_diccionario *aux = dictionary_get(diccionario,
			&(identificador_variable));
	return (aux == NULL ) ? -1 : aux->desplazamiento;
}

t_valor_variable dereferenciar(t_puntero direccion_variable) {
	return *solicitarBytesAUMV(stackBase, direccion_variable + 1, 4);
}

void asignar(t_puntero direccion_variable, t_valor_variable valor) {
	enviarBytesAUMV(stackBase, direccion_variable, &valor, 4);
}

t_valor_variable obtenerValorCompartida(t_nombre_compartida variable) {
	int *razon=malloc(sizeof(int));
	*razon=OBTENER_VALOR_COMPARTIDA;
	t_paquete *paquete=serializar2(crear_nodoVar(variable, strlen(variable)));
	t_paquete *header=serializar2(crear_nodoVar(&(paquete->tamano), 4), crear_nodoVar(razon, 4));
	send(socketKernel, header->msj, TAMANO_CABECERA, 0);
	send(socketKernel, paquete->msj, paquete->tamano, 0);
	char *msjCabecera = malloc(TAMANO_CABECERA);
	//bind y listen mediante
	recv(socketKernel, msjCabecera, TAMANO_CABECERA, MSG_WAITALL);
	int tamanoMensaje;
	desempaquetar2(msjCabecera, &tamanoMensaje, razon, 0);
	//quizas deberia preguntar por la razon, pero ni da la verdad.
	char *mensaje = malloc(tamanoMensaje);
	recv(socketKernel, mensaje, tamanoMensaje, MSG_WAITALL);
	t_valor_variable *aux = malloc(sizeof(t_valor_variable));
	desempaquetar2(mensaje, aux, 0);
	return *aux;
}

t_valor_variable asignarValorCompartida(t_nombre_compartida variable,
		t_valor_variable valor) {
	int *razon=malloc(sizeof(int));
	*razon=ASIGNAR_VALOR_COMPARTIDA;
	t_paquete *paquete=serializar2(crear_nodoVar(&variable, sizeof(t_nombre_compartida)), crear_nodoVar(&valor, sizeof(t_valor_variable)), 0);
	t_paquete *header=serializar2(crear_nodoVar(&(paquete->tamano), 4), crear_nodoVar(razon, 4), 0);
	send(socketKernel, header->msj, TAMANO_CABECERA, 0);
	send(socketKernel, paquete->msj, paquete->tamano, 0);
	//Quizas deberias esperar a la confirmacion PROBABLEMENTE
	return valor;
}

void irAlLabel(t_nombre_etiqueta etiqueta) {
	//Segun yo a las 12, al comienzo del programa en el cpu, debemos traer el segmento de etiquetas hacia el.
	//etiquetas es una global en la que copiamos enterito el indice de etiquetas
	programCounter=metadata_buscar_etiqueta(etiqueta, etiquetas, pcb->tamanio_Indice_de_Etiquetas);
}

void llamarSinRetorno(t_nombre_etiqueta etiqueta) {
	//Yo crearia un nuevo diccionario, pero no se como es esta cosa.
	enviarBytesAUMV(stackBase, desplazamiento, &cursorCtxto, 4);
	enviarBytesAUMV(stackBase, desplazamiento + 4, &programCounter, 4);
	desplazamiento += 8;
	cursorCtxto = stackBase + desplazamiento;
	irAlLabel(etiqueta);
}
void llamarConRetorno(t_nombre_etiqueta etiqueta, t_puntero donde_retornar) {
	enviarBytesAUMV(stackBase, desplazamiento, &cursorCtxto, 4);
	enviarBytesAUMV(stackBase, desplazamiento + 4, &programCounter, 4);
	enviarBytesAUMV(stackBase, desplazamiento + 8, &donde_retornar, 4);
	desplazamiento += 12;
	cursorCtxto = stackBase + desplazamiento;
	irAlLabel(etiqueta);
}
void finalizar(void) {
	desplazamiento = cursorCtxto - 8 - stackBase;
	programCounter = *solicitarBytesAUMV(stackBase, cursorCtxto - 4, 4);
	cursorCtxto = *solicitarBytesAUMV(stackBase, cursorCtxto - 8, 4);
}
void retornar(t_valor_variable retorno) {
	desplazamiento = cursorCtxto - 12 - stackBase;
	enviarBytesAUMV(stackBase,
			*solicitarBytesAUMV(stackBase, cursorCtxto - 4, 4), &retorno, 4);
	programCounter = *solicitarBytesAUMV(stackBase, cursorCtxto - 8, 4);
	cursorCtxto = *solicitarBytesAUMV(stackBase, cursorCtxto - 12, 4);
}
void imprimir(t_valor_variable valor_mostrar) {
	int *razon=malloc(sizeof(int));
	*razon=MOSTRAR_VALOR;
	t_paquete *paquete=serializar2(crear_nodoVar(&valor_mostrar, sizeof(t_valor_variable)), 0);
	t_paquete *header=serializar2(crear_nodoVar(&(paquete->tamano), 4), crear_nodoVar(razon, 4), 0);
	send(socketKernel, header->msj, TAMANO_CABECERA, 0);
	send(socketKernel, paquete->msj, paquete->tamano, 0);
	//Quizas deberiamos esperar la respuesta
}
void imprimirTexto(char* texto) {
	int *razon=malloc(sizeof(int));
	*razon=MOSTRAR_TEXTO;
	t_paquete *paquete=serializar2(crear_nodoVar(texto, strlen(texto)+1), 0);
	t_paquete *header=serializar2(crear_nodoVar(&(paquete->tamano), 4), crear_nodoVar(razon, 4), 0);
	send(socketKernel, header->msj, TAMANO_CABECERA, 0);
	send(socketKernel, paquete->msj, paquete->tamano, 0);
	//Quizas deberiamos esperar la respuesta
}
void entradaSalida(t_nombre_dispositivo dispositivo, int tiempo) {
	int *razon=malloc(sizeof(int));
	*razon=ENTRADA_SALIDA;
	t_paquete *paquete=serializar2(crear_nodoVar(dispositivo, strlen(dispositivo)), crear_nodoVar(&tiempo, 4), 0);
	t_paquete *header=serializar2(crear_nodoVar(&(paquete->tamano), 4), crear_nodoVar(razon, 4), 0);
	send(socketKernel, header->msj, TAMANO_CABECERA, 0);
	send(socketKernel, paquete->msj, paquete->tamano, 0);
	//lo mismo que arriba
}
void wait(t_nombre_semaforo identificador_semaforo) {
	//PASO
}
void signal(t_nombre_semaforo identificador_semaforo) {

}
