/*
 * pruebasUMV.h
 *
 *  Created on: 10/06/2014
 *      Author: utnso
 */

#ifndef PRUEBASUMV_H_
#define PRUEBASUMV_H_


#include "umv.h"


//extern int cantProcesosActivos;
//extern t_list *listaProcesos; //dinamico>estatico
////t_tablaProceso vectorProcesos[10];
//extern void *baseUMV;
//extern int tamanoUMV;
//extern int flag_compactado;
//extern int flag; //Esta ni se para que esta.
//extern int k; //Esta esta solo para mostrar unos mensajes.
//extern bool algoritmo; //0 significa FF, lo ponemos por defecto porque es el mas lindo*
pthread_mutex_t m_Segmentos;

typedef struct {
	void *comienzo, *final;
} t_limites;
typedef struct {
	int comienzo, final;
} t_limites_logico;



/*typedef struct {
	int pid, activo;
	char tipo;
	t_list *tabla;
} t_tablaProceso;*/




#endif /* PRUEBASUMV_H_ */
