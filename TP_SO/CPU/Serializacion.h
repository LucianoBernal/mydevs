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
	int tamano;//un tamano de 255 PUEDE ser poco
	char cantVar;//pero es mejor que nada
}t_paquete;

t_tamYDir *crear_nodoVar(void *, int);
t_paquete *serializar2(t_tamYDir *uno, ...);
void desempaquetar2(char *, void *, ...);


#endif /* SERIALIZACION_H_ */
