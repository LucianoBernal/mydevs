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


t_puntero definirVariable(t_nombre_variable identificador_variable ){

}

t_puntero obtenerPosicionVariable(t_nombre_variable identificador_variable ){

}

t_valor_variable dereferenciar(t_puntero direccion_variable){
}

void asignar(t_puntero direccion_variable, t_valor_variable valor ){

}

t_valor_variable obtenerValorCompartida(t_nombre_compartida variable){

}

t_valor_variable asignarValorCompartida(t_nombre_compartida variable,t_valor_variable valor){

}
void irAlLabel(t_nombre_etiqueta etiqueta){

}
void llamarSinRetorno(t_nombre_etiqueta etiqueta){

}
void llamarConRetorno(t_nombre_etiqueta etiqueta,t_puntero donde_retornar){

}
void finalizar(void){

}
void retornar(t_valor_variable retorno){

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
