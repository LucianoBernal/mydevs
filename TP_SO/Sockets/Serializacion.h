/*
 * Serializacion.h
 *
 *  Created on: 09/06/2014
 *      Author: utnso
 */

#ifndef SERIALIZACION_H_
#define SERIALIZACION_H_

typedef struct{
	int tamano;
	char *p_var;
}t_tamYDir;
typedef struct{
	char *msj;
	char tamano;//un tamano de 255 PUEDE ser poco
	char cantVar;//pero es mejor que nada
}t_paquete;

void Desempaquetar(char *, t_queue *);
t_paquete *Serializar(t_queue *);
t_tamYDir *crear_nodoVar(void *, int);


#endif /* SERIALIZACION_H_ */
