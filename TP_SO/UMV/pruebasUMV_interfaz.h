/*
 * pruebasUMV_interfaz.h
 *
 *  Created on: 11/07/2014
 *      Author: utnso
 */

#ifndef PRUEBASUMV_INTERFAZ_H_
#define PRUEBASUMV_INTERFAZ_H_

typedef struct {
	int pidOwner, identificador, inicioLogico, tamano;
	void* memPpal;
} t_tablaSegmento;

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


#endif /* PRUEBASUMV_INTERFAZ_H_ */
