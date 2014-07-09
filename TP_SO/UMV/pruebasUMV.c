/*
 * pruebas.c
 *
 *  Created on: 04/06/2014
 *      Author: utnso
 */
//La idea es que vallamos probando las funciones auxiliares
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <commons/collections/list.h>
#include <stdbool.h>

#include "pruebasUMV.h"

int cantProcesosActivos = 0;
t_list *listaProcesos; //dinamico>estatico
void *baseUMV;
int tamanoUMV;
int flag_compactado = 0;
int flag = 1; //Esta ni se para que esta.
int k = 0; //Esta esta solo para mostrar unos mensajes.
bool algoritmo = 0; //0 significa FF, lo ponemos por defecto porque es el mas lindo*
static t_tablaSegmento *crear_nodoSegm(int, int, int, int, void *);
static void tsegm_destroy(t_tablaSegmento *);
pthread_mutex_t mutexCantProcActivos, mutexFlagCompactado, mutexAlgoritmo = PTHREAD_MUTEX_INITIALIZER;
static t_limites *crear_nodoLim(void *comienzo, void *final) {
	t_limites *nuevo = malloc(sizeof(t_limites));
	nuevo->comienzo = comienzo;
	nuevo->final = final;
	return nuevo;
}

int main() {
	tamanoUMV=1000;
	crearEstructurasGlobales();
	agregarProceso(1001, 'c');
	cambiarProcesoActivo(1001);
	crearSegmento(30);
	crearSegmento(40);
	crearSegmento(50);
	agregarProceso(1002, 'c');
	crearSegmento(100);
	cambiarProcesoActivo(1002);
	crearSegmento(40);
	crearSegmento(10);
	dumpMemoriaLibreYSegmentos(0);
	cambiarProcesoActivo(1001);
	crearSegmento(10);
	crearSegmento(100);
	cambiarProcesoActivo(1002);
	destruirTodosLosSegmentos();
	dumpMemoriaLibreYSegmentos(0);
	compactarMemoria();
	dumpMemoriaLibreYSegmentos(0);
	free(baseUMV);
	return 0;
}

static t_tablaProceso *crear_nodoProc(int pid, int activo, char tipo) {
	t_tablaProceso *nuevo = malloc(sizeof(t_tablaProceso));
	nuevo->pid = pid;
	nuevo->activo = activo;
	nuevo->tipo = tipo;
	nuevo->tabla = list_create();
	return nuevo;
}

static t_tablaSegmento *crear_nodoSegm(int pidOwner, int identificador,
		int inicioLogico, int tamano, void *memPpal) {
	t_tablaSegmento *nuevo = malloc(sizeof(t_tablaSegmento));
	nuevo->pidOwner = pidOwner;
	nuevo->identificador = identificador;
	nuevo->inicioLogico = inicioLogico;
	nuevo->tamano = tamano;
	nuevo->memPpal = memPpal;
	return nuevo;
}

static void tsegm_destroy(t_tablaSegmento *self) {
	free(self);
}

void crearEstructurasGlobales() {
	conseguirDeArchivo(&tamanoUMV);
	baseUMV = malloc(tamanoUMV);
	listaProcesos = list_create();
}

void agregarProceso(int pid, char tipo) {
	if (buscarPid(pid) == -1) {
		list_add(listaProcesos, crear_nodoProc(pid, 0, tipo));
		pthread_mutex_lock(&mutexCantProcActivos);
		cantProcesosActivos++;
		pthread_mutex_unlock(&mutexCantProcActivos);
	} else {
		printf("El numero pid ya esta en uso (?");
		//Creo que lean ya evito esto desde el plp
	}
}

int buscarPid(int pid) {
	int i = 0;
	bool tienePid(t_tablaProceso *self) {
		i++;
		return self->pid == pid;
	}
	t_tablaProceso *element = list_find(listaProcesos, (void*) tienePid);
	return element == NULL ? -1 : i - 1;
}

int crearSegmento(int tamano) {
	t_tablaSegmento *nuevoSegmento = crear_nodoSegm(procesoActivo(), 073,
			obtenerInicioLogico(procesoActivo(), tamano), tamano,
			obtenerInicioReal(tamano));
	t_tablaProceso *proceso = list_get(listaProcesos,
			buscarPid(procesoActivo()));
	if (proceso->tabla == NULL )
		proceso->tabla = list_create();
	list_add(proceso->tabla, nuevoSegmento);
	return nuevoSegmento->inicioLogico;
}

bool tieneProblemas(int inicio, int pid, int tamano) {
	if ((((t_tablaProceso *) list_get(listaProcesos, buscarPid(pid)))->tabla)
			== NULL )
		return false;
	t_list* tabla =
			((t_tablaProceso *) list_get(listaProcesos, buscarPid(pid)))->tabla;
	int i = 1;

	bool ordenarPorInicioLogico(t_tablaSegmento *inicioMenor,
			t_tablaSegmento *inicioMayor) {
		return inicioMenor->inicioLogico < inicioMayor->inicioLogico;
	}
	list_sort(tabla, (void*) ordenarPorInicioLogico);
	t_limites_logico *delimitar_espacios_libres(t_tablaSegmento *elemento) {
		t_limites_logico *aux = malloc(sizeof(t_limites));
		if ((i == 1) && (elemento->inicioLogico != 0)) {
			aux->comienzo = 0;
			aux->final = elemento->inicioLogico - 1;
		}
		aux->comienzo = elemento->inicioLogico + elemento->tamano + 1;
		if (list_get(tabla, i) == NULL ) {
			aux->final = 1000;
		} else {
			aux->final = ((t_tablaSegmento *) list_get(tabla, i))->inicioLogico
					- 1;
		}
		i++;
		return aux;
	}
	t_list *listaEspacios = list_map(tabla, (void*) delimitar_espacios_libres);
	t_limites_logico *aux = malloc(sizeof(t_limites_logico));
	if (list_is_empty(listaEspacios)) {
		aux->comienzo = 0;
		aux->final = 1000;
		list_add(listaEspacios, aux);
	}
	if ((listaEspacios->elements_count == tabla->elements_count)
			&& (!list_is_empty(listaEspacios))) {
		aux->comienzo = 0;
		aux->final = ((t_tablaSegmento *) tabla->head->data)->inicioLogico - 1;
		list_add_in_index(listaEspacios, 0, aux);
	}
	bool estaDentroDeUnEspacio(t_limites_logico* espacio) {
		return ((inicio > espacio->comienzo)
				&& ((inicio + tamano) < (espacio->final)));
	}
	//list_destroy_and_destroy_elements(tabla, (void*)tsegm_destroy);//con esta tira segmentation fault
	//list_clean_and_destroy_elements(tabla, (void*)tsegm_destroy);//con esta tiene comportamiento raro (un memory overload nuestro)
	return !list_any_satisfy(listaEspacios, (void*) estaDentroDeUnEspacio);
	//list_destroy_and_destroy_elements(listaEspacios, (void*) free);//TODO guarda!!!
}

int obtenerInicioLogico(int pid, int tamano) {
	const int SIZE_SEGMENT = 1000;
	int inicioLogico, error = 0;
	srand(time(NULL ));
	inicioLogico = rand() % SIZE_SEGMENT;

	while (tieneProblemas(inicioLogico, pid, tamano) || (error > 20)) {
		inicioLogico = rand() % SIZE_SEGMENT;
		error++;
	}
	if (error > 20)
		printf("Hay un tema con las bases logicas");
	return inicioLogico;
}
void compactarMemoria() {
	int i = -1;
	t_list *listaSegmentosOrdenada = obtenerListaSegmentosOrdenada();
	void _cambiar_posiciones_chetamente(t_tablaSegmento *self) {
		t_tablaSegmento *aux;
		if (i == -1) {
			self->memPpal = baseUMV;
		} else {
			aux = list_get(listaSegmentosOrdenada, i);
			if (aux != NULL )
				self->memPpal = aux->memPpal + aux->tamano + 1;
		}
		i++;
	}
	list_iterate(listaSegmentosOrdenada,
			(void*) _cambiar_posiciones_chetamente);
	printf("Compacte\n");
	//list_destroy_and_destroy_elements(listaSegmentosOrdenada, (void*)free);
}

void *obtenerInicioReal(int tamano) {
	t_list *lista_mascapita2 = list_create();

	t_list *lista_mascapita = obtenerEspaciosDisponibles();
	bool _tiene_tamano_suficiente(t_limites *self) {
		return ((self->final) - (self->comienzo)) > tamano;
	}
	lista_mascapita2 = list_filter(lista_mascapita,
			(void*) _tiene_tamano_suficiente);
	if (list_is_empty(lista_mascapita2) && !list_is_empty(lista_mascapita)) {
		pthread_mutex_lock(&mutexFlagCompactado);
		if (flag_compactado == 0) {
			compactarMemoria();
			flag_compactado = 1;
			//list_destroy_and_destroy_elements(lista_mascapita, (void*)free);
			return obtenerInicioReal(tamano);
		} else {
			printf("Memory overload, u win \n");
			//list_destroy_and_destroy_elements(lista_mascapita, (void*)free);
			return baseUMV; //Solo pongo esto para que me deje compilar, deberiamos crear un error.
		}
		pthread_mutex_unlock(&mutexFlagCompactado);
	} else {
		if (list_is_empty(lista_mascapita)){
			//list_destroy_and_destroy_elements(lista_mascapita, (void*)free);
			return baseUMV;
		}
		//list_destroy_and_destroy_elements(lista_mascapita, (void*)free);
		return seleccionarSegunAlgoritmo(lista_mascapita2);
	}
}

void *seleccionarSegunAlgoritmo(t_list *lista) {
	pthread_mutex_lock(&mutexAlgoritmo);
	if (algoritmo) {
		bool _mayorTamano(t_limites *mayorTamano, t_limites *menorTamano) {
			return (mayorTamano->final - mayorTamano->comienzo)
					> (menorTamano->final - menorTamano->comienzo);
		}
		list_sort(lista, (void*) _mayorTamano);
	}
	pthread_mutex_unlock(&mutexAlgoritmo);
	return (((t_limites *) list_head(lista))->comienzo);
}
//Habla bastante por si sola.
void cambiarAlgoritmo() {
	pthread_mutex_lock(&mutexAlgoritmo);
	algoritmo = !algoritmo;
	algoritmo ?
			printf("Ahora el algoritmo es First-Fit") :
			printf("Ahora el algoritmo es Worst-Fit");
	pthread_mutex_unlock(&mutexAlgoritmo);
}

void mostrarListaSegmentos(t_list *listaSegmento) {
	printf("			  Comienzo Final pidOwner BaseLogica Tamano\n");
	void _mostrar_posiciones_y_pid_owner(t_tablaSegmento* elemento) {
		printf("Contenido lista Segmentos:%x %x %d       %d       %d\n",
				(unsigned int) elemento->memPpal,
				(unsigned int) elemento->memPpal + elemento->tamano,
				elemento->pidOwner, elemento->inicioLogico, elemento->tamano);
	}
	list_iterate(listaSegmento, (void*) _mostrar_posiciones_y_pid_owner);
}

void mostrarListaEspacios(t_list *listaEspacios) {
	void _mostrar_limites(t_limites *self) {
		printf("Espacios libres: %x, %x\n", (unsigned int) self->comienzo,
				(unsigned int) self->final);
	}
	list_iterate(listaEspacios, (void*) _mostrar_limites);
}

t_list *obtenerEspaciosDisponibles() {
	int i = 1;

	t_list *listaParaAmasar = obtenerListaSegmentosOrdenada();

	t_limites *_mapear_t_limites(t_tablaSegmento *segmento) {
		t_limites *aux = malloc(sizeof(t_limites));
		aux->comienzo = segmento->memPpal;
		aux->final = (segmento->memPpal) + (segmento->tamano);
		return aux;
	}

	t_list *lista2 = list_map(listaParaAmasar, (void*) _mapear_t_limites);
	//list_destroy_and_destroy_elements(listaParaAmasar, (void*)free);
	t_limites *_delimitar_espacios_fisicos_libres(t_limites *limites) {
		t_limites *aux = malloc(sizeof(t_limites));
		if ((i == 1) && (limites->comienzo != baseUMV)) { //A esto lo hago por si hay un espacio libre antes del primer segmento
			aux->comienzo = baseUMV;
			aux->final = limites->comienzo;
		} else {
			aux->comienzo = limites->final + 1;
			if (list_get(lista2, i) == NULL ) { //Significa que no hay mas segmentos
				aux->final = baseUMV + tamanoUMV; //Si no me falla el calculo es el final de nuestro gran segmento
			} else {
				aux->final = ((t_limites *) list_get(lista2, i))->comienzo;
			}
		}
		i++;
		return aux;
	}
	t_list *lista3 = list_map(lista2,
			(void*) _delimitar_espacios_fisicos_libres);
	//list_destroy_and_destroy_elements(lista2, (void*)free);
	if (list_is_empty(lista3))
		list_add(lista3, crear_nodoLim(baseUMV, baseUMV + tamanoUMV));
	bool _no_es_un_error(t_limites *unaCosa) {
		return (unaCosa->final > unaCosa->comienzo);
	} //Antes habia algo mal, ahora solo sirve para sacar los que tiene comienzo==final
//Generan nodos de tamaño==0 al dope
	t_list *lista4 = list_filter(lista3, (void*) _no_es_un_error);
	//list_destroy_and_destroy_elements(lista3, (void*)free);
	return lista4;
}

t_list *obtenerListaSegmentosOrdenada() {

	int i;
	t_list *listaAux = list_create(), *listaSegmentos = list_create();
	bool _tiene_pid_owner(t_tablaSegmento *self) {
		return self->pidOwner
				== ((t_tablaProceso *) list_get(listaProcesos, i))->pid;
	}
	pthread_mutex_lock(&mutexCantProcActivos);
	for (i = 0; i < cantProcesosActivos; i++) {
		listaAux = list_filter(
				((t_tablaProceso *) list_get(listaProcesos, i))->tabla,
				(void*) _tiene_pid_owner);
		list_add_all(listaSegmentos, listaAux);
	}
	list_clean(listaAux);
	bool _posicion_menor(t_tablaSegmento *posicionMenor,
			t_tablaSegmento *posicionMayor) {
		return posicionMenor->memPpal < posicionMayor->memPpal;
	}
	list_sort(listaSegmentos, (void*) _posicion_menor);
	pthread_mutex_unlock(&mutexCantProcActivos);
	return listaSegmentos;
}
//No me consta que sea necesaria esta funcion.
void destruirSegmento(int base) {
	bool _coincide_base_logica(t_tablaSegmento *self) {
		return self->inicioLogico == base;
	}
	t_tablaSegmento* aux = list_remove_by_condition(
			((t_tablaProceso *) list_get(listaProcesos,
					buscarPid(procesoActivo())))->tabla,
			(void*) _coincide_base_logica);
	if (NULL == aux) {
		printf("Quisiste borrar algo que no hay papa\n");
	}
	free(aux);	//No hay problema con hacer free(NULL)
}

void destruirTodosLosSegmentos() {
	list_destroy_and_destroy_elements(
			((t_tablaProceso *) list_get(listaProcesos,
					buscarPid(procesoActivo())))->tabla, (void*) tsegm_destroy);
}

void conseguirDeArchivo(int *p_tamanoUMV) {
	*p_tamanoUMV = 2000;
}
//Estaria bueno agregarla a las commons
t_link_element *list_head(t_list *list) {
	return list_get(list, 0);
}

t_tablaSegmento *obtenerPtrASegmento(int base, int pid) {
	bool _coincide_base_logica(t_tablaSegmento *self) {
		return self->inicioLogico == base;
	}
	return (t_tablaSegmento *) list_head(
			list_filter(
					((t_tablaProceso *) list_get(listaProcesos, buscarPid(pid)))->tabla,
					(void*) _coincide_base_logica));
}//El list_head a.k.a list_get(lista, 0) sacaria el primer elemento, no deberia ser necesario pero
//por el momento todos tendrian la misma base logica (073) entonces lo necesito.

void *obtenerDirFisica(int base, int offset, int pid) {
	return (obtenerPtrASegmento(base, pid)->memPpal) + offset;
}

int verificarEspacio(int pid, int base, int offset, int tamano) {
	if (tamano - 1 < (int) (obtenerPtrASegmento(base, pid)->tamano) - offset) {
		return 1;
	} else {
		if ((int) (obtenerPtrASegmento(base, pid)->tamano) - offset < 0) {
			printf("Segmentation fault (?\n");//Me dio un offset que te saca del segmento
		} else {
			printf("El mensaje es demasiado largo para el segmento\n");
		}
	}
	return 0;
}

void enviarUnosBytes(int base, int offset, int tamano, void *mensaje) {
	int pid = procesoActivo();
	if (verificarEspacio(pid, base, offset, tamano))
		memcpy(obtenerDirFisica(base, offset, pid), mensaje, tamano);
}

int procesoActivo() {
	bool estaActivo(t_tablaProceso *self) {
		return self->activo == 1;
	}
	t_tablaProceso *elemento = list_find(listaProcesos, (void*) estaActivo);
	return elemento->pid;	//Podria tambien devolver i
}

void cambiarProcesoActivo(int pid) {
	void cambiarActivo(t_tablaProceso *self) {
		if (self->pid == pid)
			(self->activo) = 1;
		else
			(self->activo) = 0;
	}
	list_iterate(listaProcesos, (void*) cambiarActivo);
}

char *solicitarBytes(int base, int offset, int tamano) {
	char *aux = malloc(50);
	int pid = procesoActivo();
	if (verificarEspacio(pid, base, offset, tamano)) {
		memcpy(aux, obtenerDirFisica(base, offset, pid), tamano);
		*(aux + tamano + 1) = 0;
		return aux;
	}
	return "";
}

void dumpMemoriaLibreYSegmentos(bool archivo) {
	int i = 0;
	mostrarListaSegmentos(obtenerListaSegmentosOrdenada());
	void _acumularEspacio(t_limites *elemento) {
		i += elemento->final - elemento->comienzo;
	}
	list_iterate(obtenerEspaciosDisponibles(), (void*) _acumularEspacio);
	printf("El espacio actual disponible en memoria es: %d b\n", i);
	//TODO: guardar en archivo
}
void dumpTablaSegmentos(bool archivo, int pid) {
	int i;
	if (pid == -1) {
		for (i = 0; i < cantProcesosActivos; i++) {
			printf("Se muestra la tabla de segmentos del proceso %d\n",
					((t_tablaProceso *) list_get(listaProcesos, i))->pid);
			mostrarListaSegmentos(
					((t_tablaProceso *) list_get(listaProcesos, i))->tabla);
		}
	} else {
		printf("Se muestra la tabla de segmentos del proceso %d\n",
				((t_tablaProceso *) list_get(listaProcesos, buscarPid(pid)))->pid);
		mostrarListaSegmentos(
				((t_tablaProceso *) list_get(listaProcesos, buscarPid(pid)))->tabla);
	}
	//TODO: guardar en archivo
}

void dumpMemoriaChata(int offset, int tamano, bool archivo) {
	int i, *a = malloc(sizeof(int));
	printf("Se muestra la memoria desde la posicion %x a la %x \n",
			(unsigned int) baseUMV + offset,
			(unsigned int) baseUMV + offset + tamano);
	for (i = 0; i < tamano;) {
		memcpy(a, baseUMV + offset + i, 4);
		printf("%x ", (unsigned int) *a);
		i += 4;
		if (i % 96 == 0)
			printf(" \n");
	}
	//TODO: guardar en archivo
}

//FIXME: no funciona si se pasa el retardo por parámetro
void aplicarRetardo(int ret){
/*int const NANO_TO_MILI 1000000;
int tiempoRetardo;
tiempoRetardo= retardo * NANO_TO_MILI;
struct timespec ts;
ts.tv_sec = 5;
ts.tv_nsec = tiempoRetardo;
nanosleep(&ts,NULL);*/
	float retardoPosta = ret / 1000;
	sleep(retardoPosta);
}

