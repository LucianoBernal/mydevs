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
	int pidOwner, identificador, inicioLogico, tamano;
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
int flag_compactado = 0;
int flag = 1;//Esta ni se para que esta.
int k=0;//Esta esta solo para mostrar unos mensajes.
char *algoritmo="FIRST FIT";//Lo ponemos por defecto porque es el mas lindo*

t_link_element *list_head(t_list *);
static t_tablaSegmento *crear_nodoSegm(int, int, int, int, void *);
static void tsegm_destroy(t_tablaSegmento *);
t_tablaSegmento *obtenerPtrASegmento(int, int);
void crearEstructurasGlobales();
void agregarProceso(int, char);
int crearSegmento(int, int);
void destruirSegmento(int, int);
void *obtenerInicioReal(int);
void *seleccionarSegunAlgoritmo(t_list *);
void compactarMemoria();
void mostrarListaSegmentos();
t_list *obtenerEspaciosDisponibles();
t_list *obtenerListaSegmentosOrdenada();
void conseguirDeArchivo(int *);
void *obtenerDirFisica(int, int, int);
void enviarUnosBytes(int, int, int, int, void*);
char *solicitarBytes(int, int, int, int);
int obtenerInicioLogico(int);
int buscarPid(int);

void probarFuncionesSegmentos(int *, char);
int verificarEspacio(int, int, int, int);
static t_limites *crear_nodoLim(void *comienzo, void *final){
	t_limites *nuevo = malloc(sizeof(t_limites));
	nuevo->comienzo = comienzo;
	nuevo->final = final;
	return nuevo;
}

static t_tablaSegmento *crear_nodoSegm(int pidOwner, int identificador, int inicioLogico, int tamano, void *memPpal) {
	t_tablaSegmento *nuevo = malloc(sizeof(t_tablaSegmento));
	nuevo->pidOwner = pidOwner;
	nuevo->identificador = identificador;
	nuevo->inicioLogico = inicioLogico;
	nuevo->tamano = tamano;
	nuevo->memPpal = memPpal;
	return nuevo;
}
static void tsegm_destroy(t_tablaSegmento *self){
	free(self->memPpal);
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
	t_tablaSegmento *nuevoSegmento = crear_nodoSegm(pid, 073, obtenerInicioLogico(pid), tamano, obtenerInicioReal(tamano));
	list_add(vectorProcesos[buscarPid(pid)].tabla, nuevoSegmento);
	return nuevoSegmento->inicioLogico;
}//Creo que es necesario que devuelva el inicio logico asi el programa puede identificarlo posteriormente.

int obtenerInicioLogico(int pid){
	//Aca deberia haber algo complicado con randoms y chequeos;
	return 5;//El numero 5 es casi tan bueno como muchos algoritmos raros
}
void compactarMemoria(){
	int i = -1;
	t_list *listaSegmentosOrdenada = obtenerListaSegmentosOrdenada();
	void _cambiar_posiciones_chetamente(t_tablaSegmento *self){
		t_tablaSegmento *aux;
		if (i==-1) {
			self->memPpal=baseUMV;
		} else{
			aux=list_get(listaSegmentosOrdenada, i);
			if (aux!=NULL) self->memPpal=aux->memPpal+aux->tamano+1;
		}
	i++;
	}
	list_iterate(listaSegmentosOrdenada, (void*)_cambiar_posiciones_chetamente);
}
void *obtenerInicioReal(int tamano){
	t_list *lista_mascapita2 = list_create();

	t_list *lista_mascapita = obtenerEspaciosDisponibles();
	bool _tiene_tamano_suficiente(t_limites *self){
		return ((self->final)-(self->comienzo))>tamano;
	}
	lista_mascapita2 = list_filter(lista_mascapita, (void*)_tiene_tamano_suficiente);
	if (list_is_empty(lista_mascapita2) && !list_is_empty(lista_mascapita)){
		if (flag_compactado==0){
			compactarMemoria();
			flag_compactado=1;
			return obtenerInicioReal(tamano);//deberias compactar y llamar otra vez a la funcion
		} else{
			printf("no hay espacio suficiente en memoria");
			return baseUMV; //Solo pongo esto para que me deje compilar, deberiamos crear un error.
		}
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
	//Con list_get 0 tenemos el comienzo y el final del segmento segun el algoritmo
	//Yo elijo el primer bit porque ni se como usar las funciones random
	return(((t_limites *)list_head(lista))->comienzo);
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
void mostrarListaSegmentos(t_list *listaSegmento){
	printf("			  Comienzo Final pidOwner\n");
	void _mostrar_posiciones_y_pid_owner(t_tablaSegmento* elemento){
		printf("Contenido lista Segmentos:%x %x %d\n", (unsigned int)elemento->memPpal, (unsigned int)elemento->memPpal+elemento->tamano, elemento->pidOwner);
	}

	list_iterate(listaSegmento, (void*)_mostrar_posiciones_y_pid_owner);
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

	t_limites *_delimitar_espacios_libres(t_limites *limites){
		t_limites *aux = malloc(sizeof(t_limites));
		if ((i==1) && (limites->comienzo!=baseUMV)){//A esto lo hago por si hay un espacio libre antes del primer segmento
			aux->comienzo=baseUMV;
			aux->final=limites->comienzo;
		} else {
			aux->comienzo = limites->final+1;
			if (list_get(lista2, i)==NULL){ //Significa que no hay mas segmentos
				aux->final = baseUMV + tamanoUMV;//Si no me falla el calculo es el final de nuestro gran segmento
			} else {
				aux->final = ((t_limites *)list_get(lista2, i))->comienzo;
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
	}//Antes habia algo mal, ahora solo sirve para sacar los que tiene comienzo==final
	//Generan nodos de tamaño==0 al dope
	t_list *lista4 = list_filter(lista3, (void*) _no_es_un_error);

	return lista4;
}

t_list *obtenerListaSegmentosOrdenada(){

	int i;
	t_list *listaAux=list_create(), *listaSegmentos = list_create();

	bool _es_verdad(t_tablaSegmento *self){
		return 1;
	}

	for (i=0;i<cantProcesosActivos;i++){
		bool _tiene_pid_owner(t_tablaSegmento *self){
			return self->pidOwner==i;
		}
		list_add_all(listaSegmentos, vectorProcesos[i].tabla);
		list_remove_by_condition(vectorProcesos[i].tabla, (void*)_tiene_pid_owner);
	}

	bool _posicion_menor(t_tablaSegmento *posicionMenor, t_tablaSegmento *posicionMayor){
		return posicionMenor->memPpal < posicionMayor->memPpal;
	}

	list_sort(listaSegmentos, (void*) _posicion_menor);
	list_destroy_and_destroy_elements(listaAux, (void*)tsegm_destroy);
	return listaSegmentos;
}


void destruirSegmento(int pid, int base){
	bool _coincide_base_logica(t_tablaSegmento *self){
		return self->inicioLogico==base;
	}
	t_tablaSegmento* aux=list_remove_by_condition(vectorProcesos[buscarPid(pid)].tabla, (void*)_coincide_base_logica);
	if (NULL==aux){
		printf("Quisiste borrar algo que no hay papa\n");
	}
	free(aux);//No hay problema con hacer free(NULL)
}

void conseguirDeArchivo(int *p_tamanoUMV){
	//en un futuro buscaria en el archivo de config.
	*p_tamanoUMV = 1000;
}
//Estaria bueno agregarla a las commons
t_link_element *list_head(t_list *list){
	return list_get(list, 0);
}
t_tablaSegmento *obtenerPtrASegmento(int base, int pid){
	bool _coincide_base_logica(t_tablaSegmento *self){
		return self->inicioLogico==base;
	}
	return (t_tablaSegmento *)list_head(list_filter(vectorProcesos[buscarPid(pid)].tabla, (void*)_coincide_base_logica));
}//El list_head a.k.a list_get(lista, 0) sacaria el primer elemento, no deberia ser necesario pero
//por el momento todos tendrian la misma base logica (073) entonces lo necesito.

void *obtenerDirFisica(int base, int offset, int pid){
	return (obtenerPtrASegmento(base, pid)->memPpal)+offset;
}

int verificarEspacio(int pid, int base, int offset, int tamano){
	if (tamano - 1< (int)(obtenerPtrASegmento(base, pid)->tamano) - offset){
		return 1;
	} else{
		if ((int)(obtenerPtrASegmento(base, pid)->tamano) - offset<0){
			printf("Segmentation fault (?\n");//Me dio un offset que te saca del segmento
		} else{
			printf("El mensaje es demasiado largo para el segmento\n");
		}
	}
	return 0;
}

void enviarUnosBytes(int pid, int base, int offset, int tamano, void *mensaje){
	if (verificarEspacio(pid, base, offset, tamano)) memcpy(obtenerDirFisica(base, offset, pid), mensaje, tamano);
}

char *solicitarBytes(int pid, int base, int offset, int tamano){
	char *aux=malloc(50);
	if(verificarEspacio(pid, base, offset, tamano)){
		memcpy(aux, obtenerDirFisica(base, offset, pid), tamano);
		*(aux+tamano+1)=0;
		return aux;
	}
	return "";
}

void probarFuncionesSegmentos(int pid[2], char tipo){
	agregarProceso(pid[0], tipo);
	agregarProceso(pid[1], tipo);
	agregarProceso(pid[2], tipo);
	crearSegmento(pid[1], 100);
	crearSegmento(pid[0], 50);
	destruirSegmento(pid[1], 5);//Es la base que le puse a todos
	crearSegmento(pid[2], 30);
	crearSegmento(pid[0], 20);
	crearSegmento(pid[1], 100);
	crearSegmento(pid[1], 100);
	crearSegmento(pid[0], 20);
	crearSegmento(pid[0], 20);
	crearSegmento(pid[0], 20);
	crearSegmento(pid[0], 20);
	crearSegmento(pid[0], 20);
	destruirSegmento(pid[1], 5);//Es la base que le puse a todos
	mostrarListaSegmentos(obtenerListaSegmentosOrdenada());
	compactarMemoria();
	printf("\nSeparador rustiquisimo, posiciones despues de compactar\n");
	mostrarListaSegmentos(obtenerListaSegmentosOrdenada());
}
int main(){
	int pid[2];
	char tipo = 'c';
	pid[0]=1000;
	pid[1]=1001;
	pid[2]=1002;
//	probarFuncionesSegmentos(pid, tipo);
	char mensajeEnviado[20];
	crearEstructurasGlobales();
	agregarProceso(pid[0], tipo);
	crearSegmento(pid[0], 20);
	mostrarListaSegmentos(obtenerListaSegmentosOrdenada());
	printf("a ver dame un mensaje papin: ");
	gets(mensajeEnviado);
	enviarUnosBytes(pid[0], 5, 10, strlen(mensajeEnviado), mensajeEnviado);
	printf("%s", solicitarBytes(pid[0], 5, 10, strlen(mensajeEnviado)));
	return 0;
}
