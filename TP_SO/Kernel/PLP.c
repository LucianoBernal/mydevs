/*
 * PLP.C
 *
 *  Created on: 18/05/2014
 *      Author: utnso
 */

#include "PLP.h"
#include "PLPinterface.h"

typedef struct {
	char* literal;
	int sd;
} t_gestionarPrograma;

#define TAMANO_CABECERA 16

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
		log_error(logKernel,
				"Error al crear hilo de multiplexor de Scripts, Error %d",
				iretMultiScripts);
		exit(EXIT_FAILURE);
	}
	log_info(logKernel, "Hilo Multiplexor Scripts lanzado exitosamente");
	iretColaNew = pthread_create(&threadColaNew, NULL, deNewAReady,
			(void*) sinParametros);
	if (iretColaNew) {
		log_error(logKernel, "Error al crear hilo de Cola New,Error %d",
				iretColaNew);
		exit(EXIT_FAILURE);
	}
	log_info(logKernel, "Hilo Cola New lanzado exitosamente");
	iretColaExit = pthread_create(&threadColaExit, NULL, manejoColaExit,
			(void*) sinParametros);

	if (iretColaExit) {
		log_error(logKernel, "Error al crear hilo de Cola Exit,Error %d",
				iretColaExit);
		exit(EXIT_FAILURE);
	}
	log_info(logKernel, "Hilo Cola Exit lanzado exitosamente");
	pthread_join(thread_multiplexorScripts, NULL );
	pthread_join(threadColaNew, NULL );
	pthread_join(threadColaExit, NULL );

	return EXIT_SUCCESS;
}

void asignaciones_desde_metada(t_metadata_program* metadata, t_PCB* pcb) {
	pcb->program_Counter = metadata->instruccion_inicio; //FIXME
	pcb->tamanio_Indice_de_Etiquetas = metadata->etiquetas_size;
	pcb->tamanio_Contexto_Actual = 0;
	pcb->cursor_Stack = pcb->segmento_Stack;

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
	nuevo->pcb = malloc(sizeof(t_PCB));
	memcpy(nuevo->pcb, pcb, sizeof(t_PCB));
	nuevo->peso = peso;
	return nuevo;
}

bool menor_Peso(t_new *program1, t_new *program2) {
	return program1->peso < program2->peso;
}

void encolar_New(t_PCB* pcb, int peso) {
	log_info(logKernel,
			"Encolando Nuevo Programa en Cola de NEW,con Pid%d y Peso %d",
			pcb->program_id, peso);
	t_new* nodoNuevo = crear_nodoNew(pcb, peso);
	sem_wait(&colaNuevosMutex);
	list_add(colaNew, nodoNuevo);
	list_sort(colaNew, (void*) menor_Peso);
	sem_post(&colaNuevosMutex);
	mostrar_todas_Las_Listas();
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
	int razon;
	printf("razon de CREAR SEGMENTOS es%d\n", CREAR_SEGMENTOS_PROGRAMA);
	printf("%d %d %d %d", tamanioScript, metadata->instrucciones_size,
			metadata->etiquetas_size, tamanio_stack);
	int tamanoInstrucciones=metadata->instrucciones_size*8;
	enviarConRazon(socketUMV, logKernel, CREAR_SEGMENTOS_PROGRAMA,
			serializar2(crear_nodoVar(&tamanioScript, 4),
					crear_nodoVar(&tamanoInstrucciones, 4),
					crear_nodoVar(&metadata->etiquetas_size, 4),
					crear_nodoVar(&tamanio_stack, 4), 0));
	char *mensaje = recibirConRazon(socketUMV, &razon, logKernel);
	if (razon == BASES_LOGICAS) {
		desempaquetar2(mensaje, &pcb->segmento_Codigo, &pcb->indice_de_Codigo,
				&pcb->indice_de_Etiquetas, &pcb->segmento_Stack, 0);
		printf(
				"\nsegmento de codigo %d\n indice de codigo %d \n indice etiquetas %d \n segmento stack %d \n",
				pcb->segmento_Codigo, pcb->indice_de_Codigo,
				pcb->indice_de_Etiquetas, pcb->segmento_Stack);
		free(mensaje);
		return 1;
	} else {
		printf("memory_Overload\n");
		return 0;
	}
}

void escribir_en_Memoria(t_metadata_program * metadata, t_PCB *pcb,
		char *literal, int tamanoLiteral) {
	int *offset = malloc(sizeof(int));
	*offset = 0;
	printf("Estoy en escribir memoria\n");
	int segmentoCodigo = pcb->segmento_Codigo;
	int indiceCodigo = pcb->indice_de_Codigo;
	int instruccionesSize = metadata->instrucciones_size*8;
	int indiceEtiquetas = pcb->indice_de_Etiquetas;
	int etiquetasSize = metadata->etiquetas_size;
	enviarConRazon(socketUMV, logKernel, ESCRIBIR_EN_UMV,
			serializar2(crear_nodoVar(&segmentoCodigo, 4),
					crear_nodoVar(offset, 4), crear_nodoVar(&tamanoLiteral, 4),
					crear_nodoVar((char*) literal, tamanoLiteral), 0));
	enviarConRazon(socketUMV, logKernel, ESCRIBIR_EN_UMV,
			serializar2(crear_nodoVar(&indiceCodigo, 4),
					crear_nodoVar(offset, 4),
					crear_nodoVar(&instruccionesSize, 4),
					crear_nodoVar(metadata->instrucciones_serializado,
							instruccionesSize), 0));
	enviarConRazon(socketUMV, logKernel, ESCRIBIR_EN_UMV,
			serializar2(crear_nodoVar(&indiceEtiquetas, 4),
					crear_nodoVar(offset, 4), crear_nodoVar(&etiquetasSize, 4),
					crear_nodoVar(metadata->etiquetas, etiquetasSize), 0));
}

void agregar_En_Diccionario(int pid, int sd) {
	char* pidC=malloc(5);
	int* sdNodo=malloc(sizeof(int));
	*sdNodo=sd;
	snprintf(pidC, 5, "%d", pid);
	sem_wait(&PidSD_Mutex);
	log_debug(logKernel, "Agregando pid en diccionario, con pid: %d y sd :%d",
			pid, sd);

	dictionary_put(pidYSockets, pidC, sdNodo);
	sem_post(&PidSD_Mutex);

}

void crear_Nuevo_Proceso(int progid) {
	printf("estoy en crear proceso nuevo\n");
	enviarConRazon(socketUMV, logKernel, CREAR_PROCESO_NUEVO,
			serializar2(crear_nodoVar(&progid, 4), 0));
	printf("pid:%d \n", progid);	//Prendanme una vela pls
}

void gestionarProgramaNuevo(char* literal, int sd, int tamanioLiteral) {
	t_PCB* pcb = malloc(sizeof(t_PCB));
	t_metadata_program* metadata;
	metadata = metadata_desde_literal(literal);
	pcb->program_id = generarProgramID();
	int peso = calcularPeso(metadata);
	sem_wait(&mutexProcesoActivo);
	crear_Nuevo_Proceso(pcb->program_id);
	cambiar_Proceso_Activo(pcb->program_id);
	if (crearSegmentos_Memoria(metadata, pcb, literal, tamanioLiteral)) {
		escribir_en_Memoria(metadata, pcb, literal, tamanioLiteral);
		asignaciones_desde_metada(metadata, pcb);
		sem_post(&mutexProcesoActivo);
		encolar_New(pcb, peso);
		agregar_En_Diccionario(pcb->program_id, sd);
	} else {
		sem_post(&mutexProcesoActivo);
		notificar_Memoria_Llena(sd);
		close(sd);
		liberar_numero(pcb->program_id);
	}
	metadata_destruir(metadata); //OJO QUIZAS SOLO SEA EN EL ELSE REVISAR!
	printf("PESO:%d\n", peso);
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
		t_PCB* pcb_Ready = malloc(sizeof(t_PCB));
		memcpy(pcb_Ready, elementoSacado->pcb, sizeof(t_PCB));
		encolar_en_Ready(pcb_Ready);
		free(elementoSacado);
	}
	return NULL ;
}

void encolar_en_Ready(t_PCB* pcb) {
	sem_wait(&colaReadyMutex);
	log_info(logKernel, "Encolando Programa en Cola Ready, pid: %d",
			pcb->program_id);
	queue_push(colaReady, pcb);
	sem_post(&colaReadyMutex);
	mostrar_todas_Las_Listas();
	sem_post(&vacioReady);
}

void imprimirNodosNew(t_new* nodo) {
	printf("Program id:%i     Peso:%i\n", nodo->pcb->program_id, nodo->peso);
}

void mostrarListaNew() {
	printf("El estado de la Cola New es el siguiente:\n");
	sem_wait(&colaNuevosMutex);
	list_iterate(colaNew, (void*) (void*) imprimirNodosNew);
	sem_post(&colaNuevosMutex);
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
		sem_post(&grado_Multiprogramacion);
		sem_wait(&colaExitMutex);
		t_PCB* pcb = queue_pop(colaExit);
		sem_post(&colaExitMutex);
		log_info(logKernel, "Sacando programa de cola exit,con pid: %d",
				pcb->program_id);
		mostrar_todas_Las_Listas();
		sem_wait(&mutexProcesoActivo);                //   FIXME
		cambiar_Proceso_Activo(pcb->program_id);   // FIXME
	//	solicitar_Destruccion_Segmentos();	//FIXME
		sem_post(&mutexProcesoActivo); //FIXME
	//	enviar_Mensaje_Final(pcb->program_id);
		cerrar_conexion(pcb->program_id);
		liberar_nodo_Diccionario_PIDySD(pcb->program_id);
		liberar_numero(pcb->program_id);
		free(pcb);
	}
	return NULL ;
}

void liberar_nodo_Diccionario_PIDySD(int pid){
	char* pidC=malloc(5);
	snprintf(pidC, 5, "%d", pid);
	sem_wait(&PidSD_Mutex);
	dictionary_remove_and_destroy(pidYSockets, pidC,(void*)free);
	sem_post(&PidSD_Mutex);
	log_debug(logKernel, "Borrando pid:%d del diccionario",	pid);
}

void solicitar_Destruccion_Segmentos() {

	printf("algo %d\n", enviarConRazon(socketUMV, logKernel, DESTRUIR_SEGMENTOS,
				serializar2(crear_nodoVar(&socketUMV, 4), 0)));

//	codigos_Mensajes razon;
//	razon = DESTRUIR_SEGMENTOS;
//	int *tamanoMensaje = malloc(sizeof(int));
//	*tamanoMensaje = 4;
//	t_paquete * aSerializarHeader = (t_paquete *) serializar2(
//			crear_nodoVar(&razon, 4), crear_nodoVar(tamanoMensaje, 4), 0);
//	send(socketUMV, aSerializarHeader->msj, 16, 0);
//	send(socketUMV, (void*) tamanoMensaje, *tamanoMensaje, 0); //ES BASURA ES PARA QUE ANDE NOMAS
//	free(tamanoMensaje);
//	free(aSerializarHeader);
}

void cambiar_Proceso_Activo(int progid) {
	enviarConRazon(socketUMV, logKernel, CAMBIAR_PROCESO_ACTIVO,
			serializar2(crear_nodoVar(&progid, 4), 0));
}

void enviar_Mensaje_Final(int pid) {
	int sd = obtener_sd_Programa(pid);
	notificar_Programa(sd,
			"El kernel conluyo con la Ejecucion del Programa\r\n");
}

int obtener_sd_Programa(int pid) {
	char* pidC=malloc(5);
	snprintf(pidC, 5, "%d", pid);
	sem_wait(&PidSD_Mutex);
	int* sd = dictionary_get(pidYSockets,pidC);
	sem_post(&PidSD_Mutex);
	free (pidC);
	return *sd;
}

void notificar_Programa(int sd, char* message) {
/*	int* tamano = malloc(4);
	*tamano = strlen(message);
	if (send(sd, tamano, 4, 0) == 0) {
		perror("send");
	} else {
		if (send(sd, message, *tamano, 0) != *tamano) {
			perror("send");
		}
	}

	free(tamano);
*/

}

void cerrar_conexion(int pid) {
	int sd = obtener_sd_Programa(pid);
	log_info(logKernel,"Cerrando Conexion con Programa, sd:%d",sd);
	close(sd);

}

void mostrar_todas_Las_Listas() {
	sem_wait(&mostarColasMutex);
	printf("El estado actual de todas las colas es el siguiente:\n");
	mostrarListaNew();
	mostrarColaDeProcesosListos();
	mostrarColaDeProcesosEnEjecucion();
//mostrarColaDeProcesosBloqueados();
	mostrarColaDeProcesosFinalizados();
	sem_post(&mostarColasMutex);

}

int obtener_pid_de_un_sd(int sd){
	int tamanoDiccionario;
	int i;
	sem_wait(&PidSD_Mutex);
	tamanoDiccionario=dictionary_size(pidYSockets);
	for(i=0;i<tamanoDiccionario;i++){

	}
	sem_post(&PidSD_Mutex);
	return 0;
}
