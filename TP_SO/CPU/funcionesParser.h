/*
 * funcionesParser.h
 *
 *  Created on: 09/06/2014
 *      Author: utnso
 */

#ifndef FUNCIONESPARSER_H_
#define FUNCIONESPARSER_H_

#include <parser/parser/parser.h>
#include <commons/collections/queue.h>
#include <commons/collections/dictionary.h>

AnSISOP_funciones funciones_Ansisop;
AnSISOP_kernel funciones_kernel;

char *solicitarBytesAUMV(t_puntero base, t_puntero desplazamiento, int tamano);
void enviarBytesAUMV(t_puntero base, t_puntero desplazamiento,
		int tamano, void *datos);
void recuperarDiccionario();
t_puntero definirVariable(t_nombre_variable identificador_variable );
t_puntero obtenerPosicionVariable(t_nombre_variable identificador_variable );
t_valor_variable dereferenciar(t_puntero direccion_variable);
void asignar(t_puntero direccion_variable, t_valor_variable valor );
t_valor_variable obtenerValorCompartida(t_nombre_compartida variable);
t_valor_variable asignarValorCompartida(t_nombre_compartida variable,t_valor_variable valor);
void irAlLabel(t_nombre_etiqueta etiqueta);
void llamarSinRetorno(t_nombre_etiqueta etiqueta);
void llamarConRetorno(t_nombre_etiqueta etiqueta,t_puntero donde_retornar);
void finalizar(void);
void retornar(t_valor_variable retorno);
void imprimir(t_valor_variable valor_mostrar);
void imprimirTexto(char* texto);
void entradaSalida(t_nombre_dispositivo dispositivo, int tiempo);

void wait(t_nombre_semaforo identificador_semaforo);
void signalPropia(t_nombre_semaforo identificador_semaforo);

void vincPrimitivas();



#endif /* FUNCIONESPARSER_H_ */
