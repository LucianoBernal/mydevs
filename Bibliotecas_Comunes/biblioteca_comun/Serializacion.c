#include "Serializacion.h"


#define IP "127.0.0.1"
#define PACKAGESIZE 1024
#define PUERTO "6666"

t_tamYDir *crear_nodoVar(void *, int);
t_paquete *serializar2(t_tamYDir *uno, ...);
void desempaquetar2(char *, void *, ...);

int mainNO(){
	int g=333, h=0;
	char *a="pato ", *b ="es muy", *c=" pro", *e=malloc(10), *d=malloc(10), *f=malloc(10);


	t_paquete *paquete=serializar2(crear_nodoVar(a, strlen(a)), crear_nodoVar(b, strlen(b)), crear_nodoVar(c, strlen(c)), crear_nodoVar(&g, 4), 0);

	desempaquetar2(paquete->msj, d, e, f, &h, 0);
	printf("%s%s%s%d", d, e, f, h);
//	ay estas funciones que me dicen cosas lindas (?
	return 0;
}
//	Ahora reciben la cantidad de parametros que se les antoje (?
t_paquete *serializarPCB(t_PCB *pcb) {
	return serializar2(crear_nodoVar(&pcb->cursor_Stack, 4),
			crear_nodoVar(&pcb->indice_de_Codigo, 4),
			crear_nodoVar(&pcb->indice_de_Etiquetas, 4),
			crear_nodoVar(&pcb->program_Counter, 4),
			crear_nodoVar(&pcb->program_id, 4),
			crear_nodoVar(&pcb->segmento_Codigo, 4),
			crear_nodoVar(&pcb->segmento_Stack, 4),
			crear_nodoVar(&pcb->tamanio_Contexto_Actual, 4),
			crear_nodoVar(&pcb->tamanio_Indice_de_Etiquetas, 4), 0);
}

void desempaquetarPCB(t_PCB* pcb, char* pcbserializado) {
	desempaquetar2(pcbserializado, &pcb->cursor_Stack, &pcb->indice_de_Codigo,
			&pcb->indice_de_Etiquetas, &pcb->program_Counter, &pcb->program_id,
			&pcb->segmento_Codigo, &pcb->segmento_Stack,
			&pcb->tamanio_Contexto_Actual, &pcb->tamanio_Indice_de_Etiquetas, 0);
}

t_paquete *serializar2(t_tamYDir *uno, ...){
	va_list(p);
	va_start(p, uno);
	int acum = 0;
	t_tamYDir *arg=uno;
	t_paquete *paquete=malloc(sizeof(t_paquete));
	paquete->msj=malloc(50);
	do{
		memcpy(acum+(paquete->msj), &(arg->tamano), 4);
		memcpy(acum+4+(paquete->msj), arg->p_var, arg->tamano);
		acum+=((arg->tamano)+4);
	}while((arg = va_arg(p, t_tamYDir *)));
	va_end(p);
	paquete->tamano=acum;
	return paquete;
}
//Las dejo por si tienen algun problema las nuevas
/*
t_paquete *Serializar(t_queue *cola){
	t_paquete *paquete=malloc(sizeof(t_paquete));
	t_tamYDir *aux;
	paquete->msj=malloc(50);
	paquete->cantVar=0;
	int acum=0;
	while(!queue_is_empty(cola)){
		aux=queue_pop(cola);
		memcpy(acum+(paquete->msj), &(aux->tamano), 1);
		memcpy(acum+1+(paquete->msj),aux->p_var, aux->tamano);
		acum+=((aux->tamano)+1);
		paquete->cantVar++;
	}
	paquete->tamano=acum;
	return paquete;
}
*/
void desempaquetar2(char *msjRecibido, void *uno, ...){
	va_list(p);
	va_start(p, uno);
	void * arg=uno;
	int tamano;
	int acum=0;
	do{
		memcpy(&tamano, msjRecibido+acum, 4);
		memcpy(arg, msjRecibido+acum+4, tamano);
		acum+=(tamano+4);
	}while((arg = va_arg(p, void*)));

}
/*
void Desempaquetar(char *msjRecibido, t_queue *cola){
	void *aux=malloc(50);
	char tamano;
	int acum=0;
	while(!queue_is_empty(cola)){
		memcpy(&tamano, msjRecibido+acum, 1);
		aux=queue_pop(cola);
		memcpy(aux, msjRecibido+acum+1, tamano);
		acum+=(tamano+1);
	}
}
*/
t_tamYDir *crear_nodoVar(void *p_var, int tamano){
	t_tamYDir *nuevo=malloc(sizeof(t_tamYDir));
	nuevo->p_var=p_var;
	nuevo->tamano=tamano;
	return nuevo;
}

