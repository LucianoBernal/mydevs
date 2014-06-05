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
#include <commons/collections/list.h>

typedef struct{
	void *comienzo, *final;
}t_limites;

typedef struct {
	int identificador, inicioLogico, tamano;
	void* memPpal;
}t_tablaSegmento;

typedef struct{
	int pid, activo;
	char tipo;
	t_list *tabla;
}t_tablaProceso;

int cantProcesosActivos = 0;
t_tablaProceso vectorProcesos[10];
void *baseUMV;
int tamanoUMV;
int flag_compactado = 1;
int flag = 1;//Esta ni se para que esta.
int k=0;//Esta esta solo para mostrar unos mensajes.
char *algoritmo="FIRST FIT";//Lo ponemos por defecto porque es el mas lindo*

static t_tablaSegmento *crear_nodoSegm(int, int, int, void *);
static void tsegm_destroy(t_tablaSegmento *);
void crearEstructurasGlobales();
void agregarProceso(int, char);
int crearSegmento(int, int);
void destruirSegmento(int, int);
void *obtenerInicioReal(int);
void *seleccionarSegunAlgoritmo(t_list *);
t_list *obtenerEspaciosDisponibles();
t_list *obtenerListaSegmentosOrdenada();
void conseguirDeArchivo(int *);
//void compactar();
int obtenerInicioLogico(int);
int buscarPid(int);

static t_limites *crear_nodoLim(void *comienzo, void *final){
	t_limites *nuevo = malloc(sizeof(t_limites));
	nuevo->comienzo = comienzo;
	nuevo->final = final;
	return nuevo;
}

static t_tablaSegmento *crear_nodoSegm(int identificador, int inicioLogico, int tamano, void *memPpal) {
	t_tablaSegmento *nuevo = malloc(sizeof(t_tablaSegmento));
	nuevo->identificador = identificador;
	nuevo->inicioLogico = inicioLogico;
	nuevo->tamano = tamano;
	nuevo->memPpal = memPpal;
	return nuevo;
}
static void tsegm_destroy(t_tablaSegmento *self){
	if(self->memPpal!=baseUMV) free(self->memPpal);
	free(self);
}

void crearEstructurasGlobales(){
	conseguirDeArchivo(&tamanoUMV);
	baseUMV = malloc(tamanoUMV);

}

void agregarProceso(int pid, char tipo){
	vectorProcesos[cantProcesosActivos].pid=pid;
	vectorProcesos[cantProcesosActivos].tipo=tipo;
	vectorProcesos[cantProcesosActivos].activo=0;
 	vectorProcesos[cantProcesosActivos].tabla=list_create();
	cantProcesosActivos++;
}

int buscarPid(int pid){
	int j=0;
	while(vectorProcesos[j].pid!=pid){
		j++;
	}
	return j;
}

int crearSegmento(int pid, int tamano){
	t_tablaSegmento *nuevoSegmento = crear_nodoSegm(pid, obtenerInicioLogico(pid), tamano, obtenerInicioReal(tamano));
	list_add(vectorProcesos[buscarPid(pid)].tabla, nuevoSegmento);
	return nuevoSegmento->inicioLogico;
}//Creo que es necesario que devuelva el inicio logico asi el programa puede identificarlo posteriormente.

int obtenerInicioLogico(int pid){
	//Aca deberia haber algo complicado con randoms y chequeos;
	return 5;
}

void *obtenerInicioReal(int tamano){
	t_list *lista_mascapita2 = list_create();

	t_list *lista_mascapita = obtenerEspaciosDisponibles();
	printf("%x %x\n", (unsigned int)((t_limites *)(lista_mascapita->head->data))->comienzo, (unsigned int)((t_limites *)(lista_mascapita->head->data))->final);
	bool _tiene_tamano_suficiente(t_limites *self){
		printf("%d %d\n", ((self->final)-(self->comienzo))>tamano, tamano);
		return ((self->final)-(self->comienzo))>tamano;
	}
	lista_mascapita2 = list_filter(lista_mascapita, (void*)_tiene_tamano_suficiente);
	if (list_is_empty(lista_mascapita2) && !list_is_empty(lista_mascapita)){
		//deberias compactar y llamar otra vez a la funcion
		return baseUMV;//Si no pones un flag de compactacion esto explota
	} else {
		if (list_is_empty(lista_mascapita)) return baseUMV;
		return seleccionarSegunAlgoritmo(lista_mascapita2);
	}
}

void *seleccionarSegunAlgoritmo(t_list *lista){
	if (strcmp(algoritmo,"BEST FIT")){
		bool _mayorTamano(t_limites *mayorTamano, t_limites *menorTamano){
			return (mayorTamano->final-mayorTamano->comienzo)>(menorTamano->final-menorTamano->comienzo);
		}
		list_sort(lista, (void*)_mayorTamano);
	}
	t_limites *aux = list_get(lista, 0);
	//Con list_get 0 tenemos el comienzo y el final del segmento segun el algoritmo
	//Yo elijo el primer bit porque ni se como usar las funciones random
	return(aux->comienzo);
}

//Habla bastante por si sola.
void cambiarAlgoritmo(int numerin){
	if (numerin==0){
		algoritmo="FIRST FIT";
	}
	if (numerin==1){
		algoritmo="BEST FIT";
	}
}

t_list *obtenerEspaciosDisponibles(){
	int i=1;

	t_list *listaParaAmasar = obtenerListaSegmentosOrdenada();

	t_limites *_mapear_t_limites(t_tablaSegmento *segmento){
		t_limites *aux=malloc(sizeof(t_limites));
		aux->comienzo=segmento->memPpal;
		aux->final=(segmento->memPpal)+(segmento->tamano);
		return aux;
	}

	t_list *lista2 = list_map(listaParaAmasar, (void*) _mapear_t_limites);
	bool _es_verdad(t_limites *self){
		return 1;
	}
	t_list *copialista2 = list_filter(lista2, (void*) _es_verdad);

	int l=0;
	while (l<k){
		if (copialista2->head->next!=NULL) copialista2->head=copialista2->head->next;
		l++;
	}
	if (!list_is_empty(copialista2)) printf("%x %x\n", (unsigned int)((t_limites *)(copialista2->head->data))->comienzo, (unsigned int)((t_limites *)(copialista2->head->data))->final);
	k++;

	t_limites *_delimitar_espacios_libres(t_limites *limites){
		t_limites *aux = malloc(sizeof(t_limites));
		if ((i==1) && (limites->comienzo!=baseUMV)){//A esto lo hago por si hay un espacio libre antes del primer segmento
			aux->comienzo=baseUMV;
			aux->final=limites->comienzo;
		} else {
			aux->comienzo = limites->final;
			if (list_get(listaParaAmasar, i)==NULL){ //Significa que no hay mas segmentos
				aux->final = baseUMV + tamanoUMV;//Si no me falla el calculo es el final de nuestro gran segmento
			} else {
				aux->final = ((t_limites *)list_get(listaParaAmasar, i))->comienzo;
			}
		}
		i++;
		return aux;
	}

	t_list *lista3 = list_map(lista2, (void*) _delimitar_espacios_libres);

	flag = 0;

	if (list_is_empty(lista3)) list_add(lista3, crear_nodoLim(baseUMV, baseUMV+tamanoUMV));

	bool _no_es_un_error(t_limites *unaCosa){
		return (unaCosa->final>unaCosa->comienzo);
	}//No comprendo este comportamiento, pero es muy necesario
	t_list *lista4 = list_filter(lista3, (void*) _no_es_un_error);
	return lista4;
}

t_list *obtenerListaSegmentosOrdenada(){

	int i;
	t_list *listaAux=list_create(), *listaSegmentos = list_create();
	t_tablaSegmento *_dejar_tal_cual(t_tablaSegmento *tabla){
		return tabla;
	}
	for (i=0;i<cantProcesosActivos;i++){
		//Es la manera mas pancha que se me ocurrio de copiar la lista
		listaAux = list_map(vectorProcesos[i].tabla, (void*)_dejar_tal_cual);
		//copio las listas porque si al hacer add se cambiarian las referencias del final de cada tabla
		list_add_all(listaSegmentos, listaAux);
	}
	list_destroy_and_destroy_elements(listaAux, (void*)tsegm_destroy);

	bool _posicion_menor(t_tablaSegmento *posicionMenor, t_tablaSegmento *posicionMayor){
		return posicionMenor->memPpal < posicionMayor->memPpal;
	}

	list_sort(listaSegmentos, (void*) _posicion_menor);
	return listaSegmentos;
}

void destruirSegmento(int pid, int base){
	bool _coincide_base(t_tablaSegmento *self){
		return self->inicioLogico==base;
	}
	free(list_remove_by_condition(vectorProcesos[buscarPid(pid)].tabla, (void*)_coincide_base));
}

void conseguirDeArchivo(int *p_tamanoUMV){
	//en un futuro buscaria en el archivo de config.
	*p_tamanoUMV = 1000;
}

int main(){
	int pid[2];
	char tipo = 'c';
	pid[0]=1000;
	pid[1]=1001;
	pid[2]=1002;
	crearEstructurasGlobales();
	agregarProceso(pid[0], tipo);
	agregarProceso(pid[1], tipo);
	agregarProceso(pid[2], tipo);

	crearSegmento(pid[1], 100);
	crearSegmento(pid[0], 50);
	destruirSegmento(pid[1], 5);//Es la base que le puse a todos
	crearSegmento(pid[2], 30);
	crearSegmento(pid[0], 20);



	printf("%x %x\n", (unsigned int)((t_tablaSegmento *)vectorProcesos[0].tabla->head->data)->memPpal, (unsigned int)baseUMV);
	printf("%x\n", (unsigned int)((t_tablaSegmento *)vectorProcesos[1].tabla->head->data)->memPpal);
	printf("%x", (unsigned int)((t_tablaSegmento *)vectorProcesos[2].tabla->head->data)->memPpal);
	return 0;
}
