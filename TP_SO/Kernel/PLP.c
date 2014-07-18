/*
 * PLP.C
 *
 *  Created on: 18/05/2014
 *      Author: utnso
 */

#include "PLP.h"
#include "PLPinterface.h"
#include <biblioteca_comun/definiciones.h>

typedef struct {
	char* literal;
	int sd;
} t_gestionarPrograma;

#define TAMANO_CABECERA 16
int tamanoStack = 20; //Deberia leerlo desde config

void* plp_main(void* sinParametro) {
	colaNew = list_create();
	randoms = list_create();
	sem_init(&mutexProcesoActivo, 0, 1);
	sem_init(&PidSD_Mutex, 0, 1); // quizas vayan en el del kernel
	pidYSockets = dictionary_create(); // quizas vayan en el del kernel
	sem_init(&colaNuevosVacio, 0, 0);
	sem_init(&randomMutex, 0, 1);
	sem_init(&numABorrarMutex, 0, 1);
	sem_init(&colaNuevosMutex, 0, 1);
	pthread_t thread_multiplexorScripts, threadColaNew, threadColaExit;
	int iretMultiScripts, iretColaNew, iretColaExit;
	int* sinParametros = NULL;
	iretMultiScripts = pthread_create(&thread_multiplexorScripts, NULL,
			atencionScripts, (void*) sinParametros);
	if (iretMultiScripts) {
		fprintf(stderr, "Error - pthread_create() return code: %d\n",
				iretMultiScripts);
		exit(EXIT_FAILURE);
	}
	iretColaNew = pthread_create(&threadColaNew, NULL, deNewAReady,
			(void*) sinParametros);
	if (iretColaNew) {
		fprintf(stderr, "Error - pthread_create() return code: %d\n",
				iretColaNew);
		iretColaExit = pthread_create(&threadColaExit, NULL, manejoColaExit,
				(void*) sinParametros);
	}
	if (iretColaExit) {
		fprintf(stderr, "Error - pthread_create() return code: %d\n",
				iretColaExit);
	}

	pthread_join(thread_multiplexorScripts, NULL );
	pthread_join(threadColaNew, NULL );
	pthread_join(threadColaExit, NULL );

	return EXIT_SUCCESS;
}

void asignaciones_desde_metada(t_metadata_program* metadata, t_PCB* pcb) {
	pcb->program_Counter = metadata->instruccion_inicio;
	pcb->tamanio_Indice_de_Etiquetas = metadata->etiquetas_size;

	/*pcb-> = metadata->instrucciones_size;
	 pcb-> = metadata->instrucciones_serializado;
	 pcb-> = metadata->etiquetas;
	 pcb-> = metadata->cantidad_de_funciones;
	 pcb-> = metadata->cantidad_de_etiquetas;*/
}

bool esDistintoANumAleatorio(int* numero) {
	return (*numero != numAleatorio);
}

int estaRepetido() {
	return (!(list_all_satisfy(randoms, (void*) esDistintoANumAleatorio)));
}

int generarProgramID() {
	srand(time(NULL )); // Semilla
	sem_wait(&randomMutex);
	numAleatorio = rand() % 10000; //Numero Aleatorio entre 0 - 9999
	while (estaRepetido()) {
		numAleatorio = rand() % 10000;
	}
	int* numFinal = malloc(4);
	*numFinal = numAleatorio;
	list_add(randoms, numFinal);
	sem_post(&randomMutex);
	return (*numFinal);
}

t_new *crear_nodoNew(t_PCB* pcb, int peso) {
	t_new *nuevo = malloc(sizeof(t_new));
	nuevo->pcb = pcb;
	nuevo->peso = peso;
	return nuevo;
}

bool menor_Peso(t_new *program1, t_new *program2) {
	return program1->peso < program2->peso;
}

void encolar_New(t_PCB* pcb, int peso) {
	t_new* nodoNuevo = crear_nodoNew(pcb, peso);
	sem_wait(&colaNuevosMutex);
	list_add(colaNew, nodoNuevo);
	list_sort(colaNew, (void*) menor_Peso);
	sem_post(&colaNuevosMutex);
	sem_post(&colaNuevosVacio);

}

int calcularPeso(t_metadata_program* metadata) {
	int peso = 5 * (metadata->cantidad_de_etiquetas)
			+ 3 * (metadata->cantidad_de_funciones)
			+ metadata->instrucciones_size;
	return peso;
}

bool esIgualANumABorrar(int* numero) {
	return (*numero == numABorrar);
}

void liberar_numero(int pid) {
	sem_wait(&numABorrarMutex);
	numABorrar = pid;
	list_remove_by_condition(randoms, (void*) esIgualANumABorrar);
	sem_post(&numABorrarMutex);

}

int crearSegmentos_Memoria(t_metadata_program *metadata, t_PCB *pcb,
		char* literal, int tamanioScript) {
	int fin;
	codigos_Mensajes razon;
	razon = CREAR_SEGMENTOS_PROGRAMA;
	printf("razon de CREAR SEGMENTOS es%d\n", razon);
	int *tamanoMensaje = malloc(sizeof(int));
	int *razon2 = malloc(sizeof(int)), *tamano2 = malloc(4);
	*tamanoMensaje = 32;
	printf("%d %d %d %d", tamanioScript, metadata->instrucciones_size,
			metadata->etiquetas_size, tamanio_stack);
	t_paquete * aSerializarPaquete = (t_paquete *) serializar2(
			crear_nodoVar(&tamanioScript, 4),
			crear_nodoVar(&metadata->instrucciones_size, 4),
			crear_nodoVar(&metadata->etiquetas_size, 4),
			crear_nodoVar(&tamanio_stack, 4), 0);
	t_paquete * aSerializarHeader = (t_paquete *) serializar2(
			crear_nodoVar(&razon, 4),
			crear_nodoVar(&(aSerializarPaquete->tamano), 4), 0);
	desempaquetar2(aSerializarHeader->msj, razon2, tamano2, 0);
	printf("razon es%d %d\n", razon, *razon2);
	printf("tamano mensaje vale%d %d\n", aSerializarPaquete->tamano, *tamano2);
//	for(i=0; i<16;i++){
//		putchar(aSerializarHeader->msj[i]==0?'0':aSerializarHeader->msj[i]);
//	}
	send(socketUMV, aSerializarHeader->msj, 16, 0);
	send(socketUMV, aSerializarPaquete->msj, *tamanoMensaje, 0);
	free(aSerializarHeader);
	free(aSerializarPaquete);
	char *header = malloc(16);
	recv(socketUMV, (void*) header, 16, 0);
	desempaquetar2(header, &razon, tamanoMensaje, 0);
	if (*tamanoMensaje) {
		char* message = malloc(*tamanoMensaje);
		recv(socketUMV, (void*) message, *tamanoMensaje, MSG_WAITALL);
		int indice_Codigo, segmentoStack, segmentoCodigo, indice_Funciones;
		desempaquetar2(message, &segmentoCodigo, &indice_Codigo,
				&indice_Funciones, &segmentoStack, 0);
		pcb->segmento_Codigo = segmentoCodigo;
		pcb->indice_de_Codigo = indice_Codigo;
		pcb->indice_de_Etiquetas = indice_Funciones;
		pcb->segmento_Stack = segmentoStack;
		log_info(logKernel,
				"Los segmentos del programa fueron Creados exitosamente en la UMV");
		printf(
				"segmento de codigo %d\n indice de codigo %d \n indice etiquetas %d \n segmento stack %d \n",
				pcb->segmento_Codigo, pcb->indice_de_Codigo,
				pcb->indice_de_Etiquetas, pcb->segmento_Stack);
		free(message);
		fin = 0;
	} else {
		fin = -1; //Memory_Overload
	}
	free(header);
	free(tamanoMensaje);
	return fin;
}
/*int *codigoRespuesta = malloc(sizeof(int));
 char *tamanoRespuesta = malloc(sizeof(char));
 t_paquete *mensaje = serializar2(strlen(literal), metadata->etiquetas_size,
 tamanoStack, metadata->instrucciones_size);
 int *razon = malloc(sizeof(int));
 *razon = CREAR_SEGMENTOS_PROGRAMA;
 t_paquete *header = serializar2(crear_nodoVar(&mensaje->tamano, 1),
 crear_nodoVar(razon, sizeof(int)), 0);
 send(socketUMV, (void*) header, TAMANO_CABECERA, 0);
 send(socketUMV, (void*) mensaje, mensaje->tamano, 0);
 char *hRespuesta = malloc(TAMANO_CABECERA);
 recv(socketUMV, (void*) hRespuesta, TAMANO_CABECERA, MSG_WAITALL);
 desempaquetar2(hRespuesta, tamanoRespuesta, codigoRespuesta, 0);
 if (codigoRespuesta) {
 char *msjRespuesta = malloc(tamanoRespuesta);
 recv(socketUMV, (void*) msjRespuesta, tamanoRespuesta, MSG_WAITALL);
 desempaquetar2(msjRespuesta, pcb->segmento_Codigo,
 pcb->indice_de_Etiquetas, pcb->segmento_Stack,
 pcb->indice_de_Codigo, 0);
 } else {
 printf("No habia espacio en memoria");
 }
 return *codigoRespuesta;
 }*/

void escribir_en_Memoria(t_metadata_program * metadata, t_PCB *pcb,
		char *literal, int tamanoLiteral) {
	int *base = malloc(sizeof(int)), *offset = malloc(sizeof(int));
	*offset = 0;
	codigos_Mensajes razon;
	razon = ESCRIBIR_EN_UMV;
	int *tamanoMensaje = malloc(sizeof(int));
	//Escribo Literal en Memoria
	*tamanoMensaje = tamanoLiteral + 28;
	*base = pcb->segmento_Codigo;
	t_paquete * aSerializarPaquete = (t_paquete *) serializar2(
			crear_nodoVar(base, 4), crear_nodoVar(offset, 4),
			crear_nodoVar(&tamanoLiteral, 4),
			crear_nodoVar((char*) literal, tamanoLiteral), 0);
	t_paquete * aSerializarHeader = (t_paquete *) serializar2(
			crear_nodoVar(&razon, 4),
			crear_nodoVar(&aSerializarPaquete->tamano, 4), 0);
	printf("%d == %d != %d", aSerializarPaquete->tamano, *tamanoMensaje, &aSerializarPaquete->tamano);
	printf("tamaño mensaje%d\n", aSerializarPaquete->tamano);
	send(socketUMV, aSerializarHeader->msj, 16, 0);
	send(socketUMV, aSerializarPaquete->msj, *tamanoMensaje, 0);

	//Escribo Indice de Codigo

//	*tamanoMensaje = metadata->instrucciones_size + 28;
//	*base = pcb->indice_de_Codigo;
//	aSerializarPaquete = (t_paquete *) serializar2(crear_nodoVar(base, 4),
//			crear_nodoVar(offset, 4),
//			crear_nodoVar(&metadata->instrucciones_size, 4),
//			crear_nodoVar(metadata->instrucciones_serializado,
//					metadata->instrucciones_size), 0);
//	aSerializarHeader = (t_paquete *) serializar2(
//			crear_nodoVar(&razon, sizeof(razon)),
//			crear_nodoVar(tamanoMensaje, 4), 0);
//	send(socketUMV, aSerializarHeader->msj, 16, 0);
//	send(socketUMV, aSerializarPaquete->msj, *tamanoMensaje, 0);

	//Escribo Indice de Etiquetas y Funciones
//	*tamanoMensaje = metadata->etiquetas_size + 28;
//	*base = pcb->indice_de_Etiquetas;
//	aSerializarPaquete = (t_paquete *) serializar2(crear_nodoVar(base, 4),
//			crear_nodoVar(offset, 4),
//			crear_nodoVar(&metadata->etiquetas_size, 4),
//			crear_nodoVar(&metadata->etiquetas, metadata->etiquetas_size), 0);
//	aSerializarHeader = (t_paquete *) serializar2(
//			crear_nodoVar(&razon, sizeof(razon)),
//			crear_nodoVar(tamanoMensaje, 4), 0);
//	send(socketUMV, aSerializarHeader->msj, 16, 0);
//	send(socketUMV, aSerializarPaquete->msj, *tamanoMensaje, 0);

	//Libero
//	free(base);
//	free(offset);
//	free(tamanoMensaje);
//	free(aSerializarHeader);
//	free(aSerializarPaquete);

}
/*
 }
 int *razon = malloc(sizeof(int));
 *razon = ESCRIBIR_EN_UMV_OFFSET_CERO;
 char error = 1;
 //Segmento codigo
 t_paquete *paquete = serializar2(crear_nodoVar(&(pcb->program_id), 4),
 crear_nodoVar((pcb->segmento_Codigo), 4),
 crear_nodoVar(literal, strlen(literal)), 0);
 t_paquete *header = serializar2(crear_nodoVar(&(paquete->tamano), 4),
 crear_nodoVar(razon, sizeof(int)), 0);
 if (error)
 error = send(socketUMV, (void*) header, TAMANO_CABECERA, 0);
 if (error)
 error = send(socketUMV, (void*) paquete, paquete->tamano, 0);
 //indice etiquetas
 t_paquete *paquete = serializar2(crear_nodoVar(&(pcb->program_id), 4),
 crear_nodoVar((pcb->indice_de_Etiquetas), 4),
 crear_nodoVar(metadata->etiquetas, metadata->etiquetas_size), 0);
 t_paquete *header = serializar2(crear_nodoVar(&(paquete->tamano), 4),
 crear_nodoVar(razon, sizeof(int)), 0);
 if (error)
 error = send(socketUMV, (void*) header, TAMANO_CABECERA, 0);
 if (error)
 error = send(socketUMV, (void*) paquete, paquete->tamano, 0);
 //indice codigo
 t_paquete *paquete = serializar2(crear_nodoVar(&(pcb->program_id), 4),
 crear_nodoVar((pcb->indice_de_Codigo), 4),
 crear_nodoVar(metadata->instrucciones_serializado,
 metadata->instrucciones_size), 0);
 t_paquete *header = serializar2(crear_nodoVar(&(paquete->tamano), 4),
 crear_nodoVar(razon, sizeof(int)), 0);
 if (error)
 error = send(socketUMV, (void*) header, TAMANO_CABECERA, 0);
 if (error)
 error = send(socketUMV, (void*) paquete, paquete->tamano, 0);
 if (!error)
 printf("Alguno de los sends fallo, noob");
 }*/

void agregar_En_Diccionario(int pid, int sd) {
	sem_wait(&PidSD_Mutex);
	dictionary_put(pidYSockets, (char*) &pid, &sd);
	sem_post(&PidSD_Mutex);

}

void crear_Nuevo_Proceso(int progid) {
	codigos_Mensajes razon;
	razon = CREAR_PROCESO_NUEVO;
	printf("razon proceso nuevo asd%d\n", razon);
	int *tamanoMensaje = malloc(sizeof(int));
	*tamanoMensaje = 8;
	int *pid = malloc(sizeof(int));
	*pid = progid;
	t_paquete * aSerializarHeader = (t_paquete *) serializar2(
			crear_nodoVar(&razon, sizeof(razon)),
			crear_nodoVar(tamanoMensaje, 4), 0);
	t_paquete * aSerializarPaquete = (t_paquete *) serializar2(
			crear_nodoVar(pid, 4), 0);
	printf("pid:%d \n", *pid);
	//puts("hola");
	send(socketUMV, aSerializarHeader->msj, 16, 0);
	send(socketUMV, aSerializarPaquete->msj, *tamanoMensaje, 0);
	free(aSerializarHeader);
	free(aSerializarPaquete);
	free(tamanoMensaje);
	free(pid);
	//puts("terminaronLosFrees");

}

void gestionarProgramaNuevo(char* literal, int sd, int tamanioLiteral) {
	t_PCB* pcb = malloc(sizeof(t_PCB));
	t_metadata_program* metadata;
	metadata = metadata_desde_literal(literal);
	pcb->program_id = generarProgramID();
    asignaciones_desde_metada(metadata, pcb);
	int peso = calcularPeso(metadata);
	printf("\nEl codigo cambia pato, no seas paranoico\n");
	//sem_wait(&mutexProcesoActivo);
	crear_Nuevo_Proceso(pcb->program_id);
	cambiar_Proceso_Activo(pcb->program_id);
	if (crearSegmentos_Memoria(metadata, pcb, literal, tamanioLiteral) == 0) {
		//escribir_en_Memoria(metadata, pcb, literal, tamanioLiteral);
		/*sem_post(&mutexProcesoActivo);
		encolar_New(pcb, peso);
		agregar_En_Diccionario(pcb->program_id, sd);
	} else {
		sem_post(&mutexProcesoActivo);
		notificar_Memoria_Llena(sd);
		close(sd);
		//free(pcb);
		liberar_numero(pcb->program_id);*/
	}
	metadata_destruir(metadata); //OJO QUIZAS SOLO SEA EN EL ELSE REVISAR!
		printf("QESO:%d\n",peso);
		fflush(stdin);
		free(pcb);
}


void* deNewAReady(void* sinParametro) { // OTRO HILO
	while (1) {
		sem_wait(&colaNuevosVacio);
		sem_wait(&grado_Multiprogramacion);
		t_new* elementoSacado;
		sem_wait(&colaNuevosMutex);
		elementoSacado = list_remove(colaNew, 0);
		sem_post(&colaNuevosMutex);
		t_PCB* pcb_Ready;
		pcb_Ready = elementoSacado->pcb;
		encolar_en_Ready(pcb_Ready);
		free(elementoSacado);
	}
	return NULL ;
}

void encolar_en_Ready(t_PCB* pcb) {
	sem_wait(&colaReadyMutex);
	queue_push(colaReady, pcb);
	sem_post(&colaReadyMutex);
	sem_post(&vacioReady);
}

void imprimirNodosNew(t_new* nodo) {
	printf("Program id:%i     Peso:%i\n", nodo->pcb->program_id, nodo->peso);
}

void mostrarListaNew() {
	printf("El estado de la Cola New es el siguiente:\n");
	list_iterate(colaNew, (void*) (void*) imprimirNodosNew);
}

void imprimirNodosPCBs(t_PCB* pcb) {
	printf("Program id:%i \n", pcb->program_id);
}

void mostrarColaDePCBs(t_queue* cola) {
	//printf("El estado de la Cola Zarasa es el siguiente:\n"); SE HACE EN EL LLAMADO
	list_iterate(cola->elements, (void*) (void*) imprimirNodosPCBs);
}

/*void notificar_Memoria_Llena(int sd) {
 char* message =
 "No hay espacio Suficiente en memoria para alojar el programa\r\n";
 int* tamano = malloc(4);
 *tamano = strlen(message);
 if (send(sd, tamano, 4, 0) == 0) {
 perror("send");
 } else {
 if (send(sd, message, *tamano, 0) != *tamano) {
 perror("send");
 }
 }
 free(message);
 free(tamano);
 }*/

void notificar_Memoria_Llena(int sd) {
	notificar_Programa(sd,
			"No hay espacio Suficiente en memoria para alojar el programa\r\n");

}

void* manejoColaExit(void* sinParametros) {
	while (1) {
		sem_wait(&colaExitVacio);
		sem_wait(&colaExitMutex);
		t_PCB* pcb = queue_pop(colaExit);
		sem_post(&colaExitMutex);
		sem_wait(&mutexProcesoActivo);
		cambiar_Proceso_Activo(pcb->program_id);
		solicitar_Destruccion_Segmentos();
		sem_post(&mutexProcesoActivo);
		enviar_Mensaje_Final(pcb->program_id);
		cerrar_conexion(pcb->program_id);
		liberar_numero(pcb->program_id);
		free(pcb);
	}
	return NULL ;
}

void solicitar_Destruccion_Segmentos() {
	codigos_Mensajes razon;
	razon = DESTRUIR_SEGMENTOS;
	int *tamanoMensaje = malloc(sizeof(int));
	*tamanoMensaje = 4;
	t_paquete * aSerializarHeader = (t_paquete *) serializar2(
			crear_nodoVar(&razon, 4), crear_nodoVar(tamanoMensaje, 4), 0);
	send(socketUMV, aSerializarHeader->msj, 16, 0);
	send(socketUMV, (void*) tamanoMensaje, *tamanoMensaje, 0); //ES BASURA ES PARA QUE ANDE NOMAS
	free(tamanoMensaje);
	free(aSerializarHeader);
}

void cambiar_Proceso_Activo(int progid) {
	codigos_Mensajes razon;
	razon = CAMBIAR_PROCESO_ACTIVO;
	int *tamanoMensaje = malloc(sizeof(int));
	*tamanoMensaje = 8;
	int *pid = malloc(sizeof(int));
	*pid = progid;
	t_paquete * aSerializarHeader = (t_paquete *) serializar2(
			crear_nodoVar(&razon, 4), crear_nodoVar(tamanoMensaje, 4), 0);
	t_paquete * aSerializarPaquete = (t_paquete *) serializar2(
			crear_nodoVar(pid, 4), 0);
	send(socketUMV, aSerializarHeader->msj, 16, 0);
	send(socketUMV, aSerializarPaquete->msj, *tamanoMensaje, 0);
	free(aSerializarHeader);
	free(aSerializarPaquete);
	free(tamanoMensaje);
	free(pid);

}

void enviar_Mensaje_Final(int pid) {
	int sd = obtener_sd_Programa(pid);
	notificar_Programa(sd,
			"El kernel conluyo con la Ejecucion del Programa\r\n");
}

int obtener_sd_Programa(int pid) {
	sem_wait(&PidSD_Mutex);
	int* sd = dictionary_get(pidYSockets, (char*) &pid);
	sem_post(&PidSD_Mutex);
	return *sd;
}

void notificar_Programa(int sd, char* message) {
	int* tamano = malloc(4);
	*tamano = strlen(message);
	if (send(sd, tamano, 4, 0) == 0) {
		perror("send");
	} else {
		if (send(sd, message, *tamano, 0) != *tamano) {
			perror("send");
		}
	}
	free(tamano);
}

void cerrar_conexion(int pid) {
	int sd = obtener_sd_Programa(pid);
	close(sd);

}

