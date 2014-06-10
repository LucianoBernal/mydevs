/*
 * funcionesParser.c
 *
 *  Created on: 09/06/2014
 *      Author: utnso
 */

#include "funcionesParser.h"

void vincularPrimitivas(){
funciones_Ansisop.AnSISOP_asignar= asignar;
funciones_Ansisop.AnSISOP_asignarValorCompartida= asignarValorCompartida;
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
funciones_Ansisop.AnSISOP_obtenerValorCompartida =obtenerValorCompartida;
funciones_Ansisop.AnSISOP_retornar = retornar;
funciones_kernel.AnSISOP_signal = signal;
funciones_kernel.AnSISOP_wait = wait;

}
int socketUMV;
int programCounter;
t_puntero stackBase;
t_puntero desplazamiento;
t_dictionary *diccionario=dictionary_create();
t_puntero cursorCtxto;

typedef struct{
	t_nombre_variable identificador_variable;
	t_valor_variable valor_variable;
}t_variable;

typedef struct{
	t_nombre_variable identificador_variable;
	t_puntero desplazamiento;
}t_variable_diccionario;

static t_variable_diccionario *crear_nodoDiccionario(t_nombre_variable identificador, int desplazamiento){
	t_variable_diccionario *nuevo= malloc(sizeof(t_variable_diccionario));
	nuevo->identificador_variable=identificador;
	nuevo->desplazamiento=desplazamiento;
	return nuevo;
}

static void destruir_variable(t_variable *self){
	free(self);
}
//claramente a este socket hace falta crearlo (y incluir las librerias)
void enviarUnosBytes(t_puntero base, t_puntero desplazamiento, void *datos, int tamano){
	t_queue *queue=queue_create();
	queue_push(queue, crear_nodoVar(base, sizeof(t_puntero)));
	queue_push(queue, crear_nodoVar(base, sizeof(t_puntero)));
	queue_push(queue, crear_nodoVar(datos, tamano));
	t_paquete *paquete =Serializar(queue);
	queue_push(queue, crear_nodoVar(paquete->tamano, 4));
	//Aca faltaria un queue_push(queue, crear_nodoVar(ID MENSAJE, 1));
	t_header *header =Serializar(queue);
	send(socketUMV, header, TAMANO_CABECERA, 0);
	send(socketUMV, paquete->msj, paquete->tamano, 0);
	//tambien deberia escuchar un mensaje de confirmacion
}
void *solicitarUnosBytes(t_puntero base, t_puntero desplazamiento, int tamano){
	t_queue *queue=queue_create();
	queue_push(queue, crear_nodoVar(base, sizeof(t_puntero)));
	queue_push(queue, crear_nodoVar(desplazamiento, sizeof(t_puntero)));
	queue_push(queue, crear_nodoVar(tamano, 4));
	t_paquete *paquete =Serializar(queue);
	queue_push(queue, crear_nodoVar(paquete->tamano, 4));
	//Tambien faltaria lo mismo que en enviarbytes
	t_header *header=Serializar(queue);
	send(socketUMV, header, TAMANO_CABECERA, 0);
	send(socketUMV, paquete->msj, paquete->tamano, 0);
	//bind y listen
	recv(socketUMV, header, TAMANO_CABECERA, MSG_WAITALL);
	int tamanoMensaje;
	queue_push(queue, &tamanoMensaje);
	Desempaquetar(header, queue);
	char *mensaje =malloc(tamanoMensaje);
	recv(socketUMV, mensaje, tamanoMensaje, MSG_WAITALL);
	char *aux=malloc(tamano);
	queue_push(queue, aux);
	Desempaquetar(mensaje, queue);
	return aux;
}//UFF SOY TAN HOMBRE
t_puntero definirVariable(t_nombre_variable identificador_variable ){
	enviarUnosBytes(stackBase, desplazamiento, &identificador_variable, 1);
	t_variable_diccionario *aux=crear_nodoDiccionario(identificador_variable, desplazamiento);
	desplazamiento+=5;//PIENSO que en el diccionario se guarda un puntero al COMIENZO de la variable
	dictionary_put(diccionario, aux->identificador_variable, aux);
	return stackBase+desplazamiento-4;//El tp dice posicion, para mi es desplazamiento nomas
}

t_puntero obtenerPosicionVariable(t_nombre_variable identificador_variable ){
	t_variable_diccionario *aux = dictionary_get(diccionario, identificador_variable);
	return (aux==NULL)?-1:aux->desplazamiento;
}

t_valor_variable dereferenciar(t_puntero direccion_variable){
	return solicitarUnosBytes(stackBase, direccion_variable, 4);
}

void asignar(t_puntero direccion_variable, t_valor_variable valor ){
	enviarUnosBytes(stackBase, direccion_variable, &valor, 4);
}

t_valor_variable obtenerValorCompartida(t_nombre_compartida variable){

}

t_valor_variable asignarValorCompartida(t_nombre_compartida variable,t_valor_variable valor){

}
t_puntero_instruccion irAlLabel(t_nombre_etiqueta etiqueta){
	//Ni idea che;
}
t_puntero_instruccion llamarSinRetorno(t_nombre_etiqueta etiqueta){
	enviarUnosBytes(stackBase, desplazamiento, cursorCtxto, 4);
	enviarUnosBytes(stackBase, desplazamiento+4, programCounter, 4);
	desplazamiento+=8;
	cursorCtxto=stackBase+desplazamiento;
	return irALabel(etiqueta);
}
t_puntero_instruccion llamarConRetorno(t_nombre_etiqueta etiqueta,t_puntero donde_retornar){
	enviarUnosBytes(stackBase, desplazamiento, cursorCtxto, 4);
	enviarUnosBytes(stackBase, desplazamiento+4, programCounter, 4);
	enviarUnosBytes(stackBase, desplazamiento+8, donde_retornar, 4);
	desplazamiento+=12;
	cursorCtxto=stackBase+desplazamiento;
	return irALabel(etiqueta);
}
t_puntero_instruccion finalizar(void){
	desplazamiento=cursorCtxto-8-stackBase;
	programCounter=solicitarUnosBytes(stackBase, cursorCtxto-4, 4);
	cursorCtxto=solicitarUnosBytes(stackBase, cursorCtxto-8, 4);
	return conseguirPunteroInstruccion(programCounter);
}
void retornar(t_valor_variable retorno){
	desplazamiento=cursorCtxto-12-stackBase;
	enviarUnosBytes(stackBase, solicitarUnosBytes(stackBase, cursorCtxto-4, 4), retorno, 4);
	programCounter=solicitarUnosBytes(stackBase, cursorCtxto-8, 4);
	cursorCtxto=solicitarUnosBytes(stackBase, cursorCtxto-12,4);
	return conseguirPunteroInstruccion(programCounter);
}
void imprimir(t_valor_variable valor_mostrar){

}
void imprimirTexto(char* texto){

}
void entradaSalida(t_nombre_dispositivo dispositivo, int tiempo){

}
void wait(t_nombre_semaforo identificador_semaforo){

}
void signal(t_nombre_semaforo identificador_semaforo){

}
