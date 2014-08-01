///*
// * funcionesParser.c
// *
// *  Created on: 09/06/2014
// *      Author: utnso
// */
////#include <biblioteca_comun/definiciones.h>
#include <parser/parser/parser.h>
#include <parser/parser/metadata_program.h>
#include "funcionesParser.h"
#include <biblioteca_comun/bibliotecaSockets.h>
#include <biblioteca_comun/Serializacion.h>
#include <semaphore.h>
#include <pthread.h>
//#include <stdio.h>
//#include <string.h>
//#include <stdlib.h>
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <netdb.h>
//#include <unistd.h>
//#include "EsqueletoCPU.h"
//
//#define TAMANO_CABECERA 16
//
extern sem_t mutexSigu;
extern int socketUMV, socketKernel, sigusr1_desactivado;
extern t_log *logs;
extern t_PCB *pcbEnUso;
extern t_dictionary *diccionarioDeVariables;
extern pthread_mutex_t mutexCroto;
extern char *etiquetas;
extern int programaFinalizado, programaBloqueado, programaAbortado;
//static unsigned char nivelContexto;
//int programCounter;
//t_puntero stackBase;
//t_puntero desplazamiento;
//t_dictionary *diccionario;
//t_puntero cursorCtxto;
//extern char *etiquetas;
////
//typedef enum {
//	CONFIRMACION,
//	SEGMENTATION_FAULT,
//	MEMORY_OVERLOAD,
//	MOSTRAR_VALOR,
//	MOSTRAR_TEXTO,
//	SALIDA_POR_BLOQUEO,
//	OBTENER_VALOR,
//	GRABAR_VALOR,
//	CREAR_SEGMENTO,
//	CREAR_SEGMENTOS_PROGRAMA,
//	DESTRUIR_SEGMENTOS,
//	ESCRIBIR_EN_UMV,
//	ESCRIBIR_EN_UMV_OFFSET_CERO,
//	ENVIAR_PCB,
//	SOLICITAR_A_UMV,
//	WAIT,
//	SIGNAL,
//	BLOQUEATE,
//	PEDIR_ETIQUETAS,
//	PEDIR_INSTRUCCION
//} codigos_mensajes;
//
//typedef struct {
//	t_nombre_variable identificador_variable;
//	t_valor_variable valor_variable;
//} t_variable;
//
//typedef struct {
//	int program_id;	//Identificador único del Programa en el sistema
//	int* segmento_Codigo;// Dirección del primer byte en la UMV del segmento de código
//	int* segmento_Stack;// Dirección del primer byte en la UMV del segmento de stack
//
//	int* cursor_Stack;// Dirección del primer byte en la UMV del Contexto de Ejecución Actual
//	int* indice_de_Codigo;// Dirección del primer byte en la UMV del Índice de Código
//	int* indice_de_Etiquetas;//Dirección del primer byte en la UMV del Índice de Etiquetas
//	int program_Counter;	//Número de la próxima instrucción a ejecutar
//	int tamanio_Contexto_Actual;//Cantidad de variables (locales y parámetros) del Contexto de Ejecución Actual
//	int tamanio_Indice_de_Etiquetas;//Cantidad de bytes que ocupa el Índice de etiquetas
//} t_PCB;
//
//typedef struct {
//	char *msj;
//	int tamano; //un tamano de 255 PUEDE ser poco
//	char cantVar; //pero es mejor que nada
//} t_paquete;
//
//typedef struct {
//	t_nombre_variable identificador_variable;
//	t_puntero desplazamiento;
//} t_variable_diccionario;
//typedef struct {
//	int tamano;
//	char *p_var;
//} t_tamYDir;
//
//t_tamYDir *crear_nodoVar(void *, int);
//t_paquete *serializar2(t_tamYDir *uno, ...);
//void desempaquetar2(char *, void *, ...);
//t_PCB *pcb;
//t_paquete *serializar2(t_tamYDir *uno, ...) {
//	va_list (p);
//	va_start(p, uno);
//	int acum = 0;
//	t_tamYDir *arg = uno;
//	t_paquete *paquete = malloc(sizeof(t_paquete));
//	paquete->msj = malloc(50);
//	do {
//		memcpy(acum + (paquete->msj), &(arg->tamano), 4);
//		memcpy(acum + 4 + (paquete->msj), arg->p_var, arg->tamano);
//		acum += ((arg->tamano) + 4);
//	} while ((arg = va_arg(p, t_tamYDir *) ));
//	va_end(p);
//	paquete->tamano = acum;
//	return paquete;
//}
//void desempaquetar2(char *msjRecibido, void *uno, ...) {
//	va_list (p);
//	va_start(p, uno);
//	void * arg = uno;
//	int tamano;
//	int acum = 0;
//	do {
//		memcpy(&tamano, msjRecibido + acum, 4);
//		memcpy(arg, msjRecibido + acum + 4, tamano);
//		acum += (tamano + 4);
//	} while ((arg = va_arg(p, void*) ));
//
//}
//t_tamYDir *crear_nodoVar(void *p_var, int tamano) {
//	t_tamYDir *nuevo = malloc(sizeof(t_tamYDir));
//	nuevo->p_var = p_var;
//	nuevo->tamano = tamano;
//	return nuevo;
//}
//

typedef struct {
	char identificador_variable;
	int desplazamiento;
} t_variable_diccionario;

t_variable_diccionario *crear_nodoDiccionario(t_nombre_variable identificador,
		int desplazamiento) {
	t_variable_diccionario *nuevo = malloc(sizeof(t_variable_diccionario));
	nuevo->identificador_variable = identificador;
	nuevo->desplazamiento = desplazamiento;
	return nuevo;
}

///*
// static void destruir_variable(t_variable *self) {
// free(self);
// }
// */
////claramente a este socket hace falta crearlo (y incluir las librerias)

void vincPrimitivas() {
	funciones_Ansisop.AnSISOP_asignar = asignar;
	funciones_Ansisop.AnSISOP_asignarValorCompartida = asignarValorCompartida;
	funciones_Ansisop.AnSISOP_definirVariable = definirVariable;
	funciones_Ansisop.AnSISOP_dereferenciar = dereferenciar;
	funciones_Ansisop.AnSISOP_entradaSalida = entradaSalida;
	funciones_Ansisop.AnSISOP_finalizar = finalizar;
	funciones_Ansisop.AnSISOP_imprimir = imprimir;
	funciones_Ansisop.AnSISOP_imprimirTexto = imprimirTexto;
	funciones_Ansisop.AnSISOP_irAlLabel = irAlLabel;
	funciones_Ansisop.AnSISOP_llamarConRetorno = llamarConRetorno;
	funciones_Ansisop.AnSISOP_llamarSinRetorno = llamarSinRetorno;
	funciones_Ansisop.AnSISOP_obtenerPosicionVariable = obtenerPosicionVariable;
	funciones_Ansisop.AnSISOP_obtenerValorCompartida = obtenerValorCompartida;
	funciones_Ansisop.AnSISOP_retornar = retornar;
	funciones_kernel.AnSISOP_signal = signalPropia;
	funciones_kernel.AnSISOP_wait = wait;
}

char* _depurar_sentencia(char* sentencia) {
//	log_debug(logs, "Depurando sentencia...");
	int i = strlen(sentencia);
	while (string_ends_with(sentencia, "\n")) {
		i--;
		sentencia = string_substring_until(sentencia, i);
	}
//	log_info(logs, "Sentencia depurada: %s", sentencia);
	return sentencia;
}

void recuperarDiccionario() {
	log_debug(logs, "Recuperando diccionario...");
	int i = 0, *desplazamiento;

//	pthread_mutex_lock(&mutexCroto);
	char *nombre, *stack = solicitarBytesAUMV(pcbEnUso->segmento_Stack,
			pcbEnUso->cursor_Stack - pcbEnUso->segmento_Stack,
			pcbEnUso->tamanio_Contexto_Actual * 5);
//	pthread_mutex_unlock(&mutexCroto);
	while (i < pcbEnUso->tamanio_Contexto_Actual) {
		nombre = malloc(2);
		desplazamiento = malloc(4);
		memcpy(nombre, stack + i * 5, 1);
		nombre[1] = 0;
		*desplazamiento = pcbEnUso->cursor_Stack - pcbEnUso->segmento_Stack
				+ i * 5;
		dictionary_put(diccionarioDeVariables, nombre, desplazamiento);
		i++;
	}
	i == 0 ? printf("jamas entre al loop\n") : printf("regenere algo\n");
}
void enviarBytesAUMV(t_puntero base, t_puntero desplazamiento, int tamano,
		void *datos) {
	int razon;
	enviarConRazon(socketUMV, logs, ESCRIBIR_EN_UMV,
			serializar2(crear_nodoVar(&base, sizeof(t_puntero)),
					crear_nodoVar(&desplazamiento, sizeof(t_puntero)),
					crear_nodoVar(&tamano, 4), crear_nodoVar(datos, tamano),
					0));
	recibirConRazon(socketUMV, &razon, logs);
	if (razon == SEGMENTATION_FAULT) {
		log_error(logs, "Hubo segmentation fault");
		if (!programaAbortado) programaAbortado = 1;
	}
}
void enviarVariableAUMV(t_puntero base, t_puntero desplazamiento, int tamano,
		void *datos) {
	int razon;
	enviarConRazon(socketUMV, logs, AGREGAR_VARIABLE,
			serializar2(crear_nodoVar(&base, sizeof(t_puntero)),
					crear_nodoVar(&desplazamiento, sizeof(t_puntero)),
					crear_nodoVar(&tamano, 4), crear_nodoVar(datos, tamano),
					0));
	recibirConRazon(socketUMV, &razon, logs);
	if (razon == SEGMENTATION_FAULT) {
		log_error(logs, "Hubo stack overflow");
		if (!programaAbortado) programaAbortado = 2;
	}
}
char *solicitarBytesAUMV(t_puntero base, t_puntero desplazamiento, int tamano) {
//	pthread_mutex_lock(&mutexCroto);
	int razon;
	enviarConRazon(socketUMV, logs, SOLICITAR_A_UMV,
			serializar2(crear_nodoVar(&base, sizeof(t_puntero)),
					crear_nodoVar(&desplazamiento, sizeof(t_puntero)),
					crear_nodoVar(&tamano, 4), 0));


	char *mensaje = recibirConRazon(socketUMV, &razon, logs);
//	pthread_mutex_unlock(&mutexCroto);
	if (razon == SEGMENTATION_FAULT) {
		log_error(logs, "Hubo segmentation fault");
		programaAbortado = 1;
		return NULL ;
	}
	char *aux = malloc(tamano + 1);
	if (mensaje != NULL)
			desempaquetar2(mensaje, aux, 0);
	free(mensaje);
	return aux;
}
t_puntero definirVariable(t_nombre_variable identificador_variable) { //Chequeada
	log_debug(logs, "Definiendo variable...");
//	pthread_mutex_lock(&mutexCroto);
	enviarVariableAUMV(pcbEnUso->segmento_Stack,
			pcbEnUso->cursor_Stack - pcbEnUso->segmento_Stack
					+ pcbEnUso->tamanio_Contexto_Actual * 5, 1,
			&identificador_variable);
//	pthread_mutex_unlock(&mutexCroto);
	char *identificadorCopiado = malloc(2);
	identificadorCopiado[0] = identificador_variable;
	identificadorCopiado[1] = 0;
	int *desplazamiento = malloc(4);
	*desplazamiento = pcbEnUso->cursor_Stack - pcbEnUso->segmento_Stack
			+ pcbEnUso->tamanio_Contexto_Actual * 5;
	log_debug(logs, "Puse una variable de nombre= %c y de offset= %d",
			identificador_variable, *desplazamiento);
	dictionary_put(diccionarioDeVariables, identificadorCopiado,
			desplazamiento);
	pcbEnUso->tamanio_Contexto_Actual++;
	return (pcbEnUso->tamanio_Contexto_Actual - 1) * 5;
}

t_puntero obtenerPosicionVariable(t_nombre_variable identificador_variable) { //Chequeada
	log_debug(logs, "Obteniendo posición con identif= %c",
			identificador_variable);
	char *identificadorPosta = malloc(2);
	identificadorPosta[0] = identificador_variable;
	identificadorPosta[1] = 0;
	int *aux = dictionary_get(diccionarioDeVariables, identificadorPosta);
	log_debug(logs, "La dirección obtenida es %d", *aux);
	return (aux == NULL ) ? -1 : *aux;
}
//
t_valor_variable dereferenciar(t_puntero direccion_variable) { //Chequeada
	log_debug(logs, "Dereferenciado con dirreción = %d", direccion_variable);
//	pthread_mutex_lock(&mutexCroto);
	char *aux =
			solicitarBytesAUMV(pcbEnUso->segmento_Stack,
					direccion_variable
							+ 1, 4);
//	pthread_mutex_unlock(&mutexCroto);
	t_valor_variable valor;
	if (aux==NULL)
		return -736;
	memcpy(&valor, aux, 4);
	log_debug(logs, "El valor obtenido es= %d", valor);
	free(aux);
	return valor;
}
void asignar(t_puntero direccion_variable, t_valor_variable valor) { //Chequeada
	log_debug(logs, "Llamé asignar con dirección= %d y valor= %d",
			direccion_variable, valor);
	log_error(logs, "DIRECCION VARIABLE ES: %d, LA DIFERENCIA ES %d",
			direccion_variable,
			pcbEnUso->cursor_Stack - pcbEnUso->segmento_Stack);
	if (direccion_variable
			< pcbEnUso->cursor_Stack - pcbEnUso->segmento_Stack) {
		direccion_variable += pcbEnUso->cursor_Stack - pcbEnUso->segmento_Stack;
	}
	log_error(logs, "LA NUEVA DIRECCION ES %d", direccion_variable);
//	pthread_mutex_lock(&mutexCroto);
	enviarBytesAUMV(pcbEnUso->segmento_Stack,direccion_variable + 1, 4, &valor);
//	pthread_mutex_unlock(&mutexCroto);
	log_debug(logs, "El valor asignado es %d", valor);
}

t_valor_variable obtenerValorCompartida(t_nombre_compartida variable) {
	log_info(logs, "Ejecute obtenerValorCompartida con %s", variable);
	int aux;
	int razon;
	enviarConRazon(socketKernel, logs, OBTENER_VALOR,
			serializar2(crear_nodoVar(variable, strlen(variable)), 0));
	char *respuesta = recibirConRazon(socketKernel, &razon, logs);
	desempaquetar2(respuesta, &aux, 0);
	free(respuesta);
	return aux;
}

t_valor_variable asignarValorCompartida(t_nombre_compartida variable,
		t_valor_variable valor) {
	log_info(logs, "Ejecute asignarValorCompartida con %s", variable);
	int razon;
	enviarConRazon(socketKernel, logs, GRABAR_VALOR,
			serializar2(crear_nodoVar(variable, strlen(variable)),
					crear_nodoVar(&valor, sizeof(t_valor_variable)), 0));
	recibirConRazon(socketKernel, &razon, logs);
	return valor;
}
//
void irAlLabel(t_nombre_etiqueta etiqueta) {
	printf("el valor del program counter viejo es %d\n",
			pcbEnUso->program_Counter);
	pcbEnUso->program_Counter = metadata_buscar_etiqueta(
			_depurar_sentencia(etiqueta), etiquetas,
			pcbEnUso->tamanio_Indice_de_Etiquetas) - 1;
	printf("el valor del program counter nuevo es %d\n",
			pcbEnUso->program_Counter);
}
//
void llamarSinRetorno(t_nombre_etiqueta etiqueta) {
	dictionary_clean_and_destroy_elements(diccionarioDeVariables, (void*) free);
//	pthread_mutex_lock(&mutexCroto);
	enviarBytesAUMV(pcbEnUso->segmento_Stack,
			pcbEnUso->cursor_Stack - pcbEnUso->segmento_Stack
					+ pcbEnUso->tamanio_Contexto_Actual * 5, 4,
			&(pcbEnUso->cursor_Stack));
	enviarBytesAUMV(pcbEnUso->segmento_Stack,
			pcbEnUso->cursor_Stack - pcbEnUso->segmento_Stack
					+ pcbEnUso->tamanio_Contexto_Actual * 5 + 4, 4,
			&(pcbEnUso->program_Counter));
//	pthread_mutex_unlock(&mutexCroto);
	printf("El viejo cursor stack es: %d\n", pcbEnUso->cursor_Stack);
	pcbEnUso->cursor_Stack = pcbEnUso->cursor_Stack
			+ pcbEnUso->tamanio_Contexto_Actual * 5 + 8;
	printf("El nuevo cursor stack es: %d\n", pcbEnUso->cursor_Stack);
	pcbEnUso->tamanio_Contexto_Actual = 0;
	irAlLabel(etiqueta);
}
void llamarConRetorno(t_nombre_etiqueta etiqueta, t_puntero donde_retornar) {
	dictionary_clean_and_destroy_elements(diccionarioDeVariables, (void*) free);
//	pthread_mutex_lock(&mutexCroto);
	enviarBytesAUMV(pcbEnUso->segmento_Stack,
			pcbEnUso->cursor_Stack - pcbEnUso->segmento_Stack
					+ pcbEnUso->tamanio_Contexto_Actual * 5, 4,
			&(pcbEnUso->cursor_Stack));
	enviarBytesAUMV(pcbEnUso->segmento_Stack,
			pcbEnUso->cursor_Stack - pcbEnUso->segmento_Stack
					+ pcbEnUso->tamanio_Contexto_Actual * 5 + 4, 4,
			&(pcbEnUso->program_Counter));
	enviarBytesAUMV(pcbEnUso->segmento_Stack,
			pcbEnUso->cursor_Stack - pcbEnUso->segmento_Stack
					+ pcbEnUso->tamanio_Contexto_Actual * 5 + 8, 4,
			&(donde_retornar));
//	pthread_mutex_unlock(&mutexCroto);
	printf("El viejo cursor stack es: %d\n", pcbEnUso->cursor_Stack);
	pcbEnUso->cursor_Stack = pcbEnUso->cursor_Stack
			+ pcbEnUso->tamanio_Contexto_Actual * 5 + 12;
	printf("El nuevo cursor stack es: %d\n", pcbEnUso->cursor_Stack);
	pcbEnUso->tamanio_Contexto_Actual = 0;
	irAlLabel(etiqueta);
}
char *generarListadoVariables() {
	int i = 0, acum = 0;
	char *aux = NULL, *axu;
	while (i < pcbEnUso->tamanio_Contexto_Actual) {
	//	pthread_mutex_lock(&mutexCroto);
		char identif = *(solicitarBytesAUMV(pcbEnUso->segmento_Stack, i * 5, 1));
	//	pthread_mutex_unlock(&mutexCroto);
		int valor = dereferenciar(i * 5);
		char *cadena = string_from_format("VARIABLE %c: %d\n", identif, valor);
		acum += strlen(cadena) + 1;
		aux = realloc(aux, acum);
		if (!i)
			aux[0] = 0;
		string_append(&aux, cadena);
		free(cadena);
		i++;
	}
	if (aux != NULL )
		axu = string_from_format("El estado final de las variables es:\n%s",
				aux);
	else
		axu = string_from_format("El programa no tenia variables ");
	return axu;
}
void finalizar(void) {
	log_info(logs, "Ejecuté finalizar");
	if (pcbEnUso->cursor_Stack == pcbEnUso->segmento_Stack) {
		char *mensajeFinal = generarListadoVariables();
		if (mensajeFinal != NULL ) {
			int razon;
			enviarConRazon(socketKernel, logs, IMPRIMIR_TEXTO,
					serializar2(
							crear_nodoVar(mensajeFinal, strlen(mensajeFinal)),
							0));
			recibirConRazon(socketKernel, &razon, logs);//FIXME confirmacion
		}
		programaFinalizado = 1;
		sem_wait(&mutexSigu);
		if (!sigusr1_desactivado) {
			enviarConRazon(socketKernel, logs, SIGUSR_1, serializar2(crear_nodoVar(&programaBloqueado,4),0) );
		}
		sem_post(&mutexSigu);
		enviarConRazon(socketKernel, logs, SALIDA_NORMAL,
				serializarPCB(pcbEnUso));
	} else {
//		pthread_mutex_lock(&mutexCroto);
		char *p_programCounter = solicitarBytesAUMV(pcbEnUso->segmento_Stack,
				pcbEnUso->cursor_Stack - pcbEnUso->segmento_Stack - 4, 4);
		char *p_cursorCtxto = solicitarBytesAUMV(pcbEnUso->segmento_Stack,
				pcbEnUso->cursor_Stack - pcbEnUso->segmento_Stack - 8, 4);
//		pthread_mutex_unlock(&mutexCroto);
		if (programaAbortado)
			goto falloSolicitar;
		int proximo_cursor_stack;
		printf("el viejo program counter es %d\n", pcbEnUso->program_Counter);
		memcpy(&(pcbEnUso->program_Counter), p_programCounter, 4);
		printf("el nuevo program counter es %d\n", pcbEnUso->program_Counter);
		memcpy(&proximo_cursor_stack, p_cursorCtxto, 4);
		pcbEnUso->tamanio_Contexto_Actual = (pcbEnUso->cursor_Stack - 8
				- proximo_cursor_stack) / 5;
		pcbEnUso->cursor_Stack = proximo_cursor_stack;
		dictionary_clean_and_destroy_elements(diccionarioDeVariables,
				(void*) free);
		recuperarDiccionario();
		falloSolicitar: ;
		free(p_programCounter);
		free(p_cursorCtxto);
	}
}
void retornar(t_valor_variable retorno) {
	log_info(logs, "Ejecute retornar con %d", retorno);
//	pthread_mutex_lock(&mutexCroto);
	char *p_retorno = solicitarBytesAUMV(pcbEnUso->segmento_Stack,
			pcbEnUso->cursor_Stack - pcbEnUso->segmento_Stack - 4, 4);
//	pthread_mutex_unlock(&mutexCroto);
	if (programaAbortado)
		goto falloSolicitar;
	int dirRetorno;
	memcpy(&dirRetorno, p_retorno, 4);
//	pthread_mutex_lock(&mutexCroto);
	enviarBytesAUMV(pcbEnUso->segmento_Stack, dirRetorno + 1, 4, &retorno);
	char *p_programCounter = solicitarBytesAUMV(pcbEnUso->segmento_Stack,
			pcbEnUso->cursor_Stack - pcbEnUso->segmento_Stack - 8, 4);
	char *p_cursorCtxto = solicitarBytesAUMV(pcbEnUso->segmento_Stack,
			pcbEnUso->cursor_Stack - pcbEnUso->segmento_Stack - 12, 4);
//	pthread_mutex_unlock(&mutexCroto);
	int proximo_cursor_stack;
	if (programaAbortado)
		goto falloSolicitar;
	printf("el viejo program counter es %d\n", pcbEnUso->program_Counter);
	memcpy(&pcbEnUso->program_Counter, p_programCounter, 4);
	printf("el nuevo program counter es %d\n", pcbEnUso->program_Counter);
	memcpy(&proximo_cursor_stack, p_cursorCtxto, 4);
	pcbEnUso->tamanio_Contexto_Actual = (pcbEnUso->cursor_Stack - 12
			- proximo_cursor_stack) / 5;
	pcbEnUso->cursor_Stack = proximo_cursor_stack;
	printf("el nuevo contexto actual es %d\n",
			pcbEnUso->tamanio_Contexto_Actual);

	dictionary_clean_and_destroy_elements(diccionarioDeVariables, (void*) free);
	recuperarDiccionario();
	falloSolicitar:;
	free(p_retorno);
	free(p_programCounter);
	free(p_cursorCtxto);
}
void imprimir(t_valor_variable valor_mostrar) {
	log_info(logs, "Ejecute imprimir con %d", valor_mostrar);
	if (valor_mostrar!=-736)
	imprimirTexto(string_from_format("%d ", valor_mostrar));
}
void imprimirTexto(char* texto) {
	int razon;
	log_info(logs, "Ejecute imprimirTexto con %s", texto);
	enviarConRazon(socketKernel, logs, IMPRIMIR_TEXTO,
			serializar2(crear_nodoVar(texto, strlen(texto)), 0)); //TODO
	recibirConRazon(socketKernel, &razon, logs);
}
void entradaSalida(t_nombre_dispositivo dispositivo, int tiempo) {
	log_info(logs, "Ejecute entradaSalida con %s", dispositivo);
	pcbEnUso->program_Counter++;
	t_paquete * paquetePCB = serializarPCB(pcbEnUso);
	int tamano = paquetePCB->tamano;
	printf("El tamano del pcb empaquetado es %d", tamano);
	mostrarPCB(pcbEnUso);
	sem_wait(&mutexSigu);
	if (!sigusr1_desactivado) {
		enviarConRazon(socketKernel, logs, SIGUSR_1, serializar2(crear_nodoVar(&programaBloqueado,4),0) );
	}
	sem_post(&mutexSigu);
	printf("El dispositvo es: %s\n",dispositivo);
	enviarConRazon(socketKernel, logs, SALIO_POR_IO, serializarPCB(pcbEnUso));
	enviarConRazon(socketKernel, logs, SALIO_POR_IO,serializar2(crear_nodoVar(dispositivo, strlen(dispositivo)),
					crear_nodoVar(&tiempo, 4), 0)); //TODO
	programaBloqueado = 1;
}
void wait(t_nombre_semaforo identificador_semaforo) {
	log_info(logs, "Ejecute wait con %s", identificador_semaforo);
	int razon;
	enviarConRazon(socketKernel, logs, WAIT,
			serializar2(
					crear_nodoVar(identificador_semaforo,
							strlen(identificador_semaforo)), 0));
	recibirConRazon(socketKernel, &razon, logs);
	if (razon == DESALOJAR_PROGRAMA) {
		pcbEnUso->program_Counter++;
		sem_wait(&mutexSigu);
		if (!sigusr1_desactivado) {
			enviarConRazon(socketKernel, logs,SIGUSR_1, serializar2(crear_nodoVar(&programaBloqueado,4),0) );
		}
		sem_post(&mutexSigu);
		enviarConRazon(socketKernel, logs, 4, serializarPCB(pcbEnUso));
		programaBloqueado = 1;
	} else {
		//NARANJA;
	}
}

void signalPropia(t_nombre_semaforo identificador_semaforo) {
	log_info(logs, "Ejecute signalPropia %s", identificador_semaforo);
	enviarConRazon(socketKernel, logs, SIGNAL,
			serializar2(
					crear_nodoVar(identificador_semaforo,
							strlen(identificador_semaforo)), 0)); //TODO
}
