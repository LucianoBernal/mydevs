/*
 * pruebasUMV.h
 *
 *  Created on: 10/06/2014
 *      Author: utnso
 */

#ifndef PRUEBASUMV_H_
#define PRUEBASUMV_H_


#include "umv.h"//TODO se borra?


extern int cantProcesosActivos;
extern t_list *listaProcesos; //dinamico>estatico
//t_tablaProceso vectorProcesos[10];
extern void *baseUMV;
extern int tamanoUMV;
extern int flag_compactado;
extern int flag; //Esta ni se para que esta.
extern int k; //Esta esta solo para mostrar unos mensajes.
extern bool algoritmo; //0 significa FF, lo ponemos por defecto porque es el mas lindo*
pthread_mutex_t m_Segmentos;

typedef struct {
	void *comienzo, *final;
} t_limites;
typedef struct {
	int comienzo, final;
} t_limites_logico;

typedef struct {
	int pidOwner, identificador, inicioLogico, tamano;
	void* memPpal;
} t_tablaSegmento;

/*typedef struct {
	int pid, activo;
	char tipo;
	t_list *tabla;
} t_tablaProceso;*/

t_link_element *list_head(t_list *);
t_tablaSegmento *obtenerPtrASegmento(int, int);
void dumpMemoriaChata(int, int, bool);
void dumpMemoriaLibreYSegmentos(bool);
void dumpTablaSegmentos(bool, int);
void crearEstructurasGlobales();
void agregarProceso(int, char);
int crearSegmento(int);
void destruirSegmento(int);
void destruirTodosLosSegmentos();
void *obtenerInicioReal(int);
void *seleccionarSegunAlgoritmo(t_list*);
void compactarMemoria();
void mostrarListaSegmentos(t_list*);
void mostrarListaEspacios(t_list*);
t_list *obtenerEspaciosDisponibles();
t_list *obtenerListaSegmentosOrdenada();
void conseguirDeArchivo(int *);
void *obtenerDirFisica(int, int, int);
void enviarUnosBytes(int, int, int, void*);
char *solicitarBytes(int, int, int);
int obtenerInicioLogico(int, int);
int buscarPid(int);
int procesoActivo();
void cambiarProcesoActivo(int);
int verificarEspacio(int, int, int, int);
void cambiarAlgoritmo();


#endif /* PRUEBASUMV_H_ */
