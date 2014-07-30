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
extern int socketUMV, socketKernel;
extern t_log *logs;
extern t_PCB *pcbEnUso;
extern t_dictionary *diccionarioDeVariables;
extern char *etiquetas;
extern int programaFinalizado, programaBloqueado;
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
	log_debug(logs, "Depurando sentencia...");
	int i = strlen(sentencia);
	while (string_ends_with(sentencia, "\n")) {
		i--;
		sentencia = string_substring_until(sentencia, i);
	}
	log_info(logs, "Sentencia depurada: %s", sentencia);
	return sentencia;
}

void recuperarDiccionario() {
	log_debug(logs, "Recuperando diccionario...");
	int i = 0, *desplazamiento;
	char *nombre, *stack = solicitarBytesAUMV(pcbEnUso->segmento_Stack,
			pcbEnUso->cursor_Stack - pcbEnUso->segmento_Stack,
			pcbEnUso->tamanio_Contexto_Actual * 5);
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
		programaFinalizado = 1;
		//SACARPROCESODECPUPORABORTO();
	}
}
char *solicitarBytesAUMV(t_puntero base, t_puntero desplazamiento, int tamano) {
	int razon;
	enviarConRazon(socketUMV, logs, SOLICITAR_A_UMV,
			serializar2(crear_nodoVar(&base, sizeof(t_puntero)),
					crear_nodoVar(&desplazamiento, sizeof(t_puntero)),
					crear_nodoVar(&tamano, 4), 0));
	char *mensaje = recibirConRazon(socketUMV, &razon, logs);
	if (razon == SEGMENTATION_FAULT) {
		log_error(logs, "Hubo segmentation fault");
		//SACARPROCESODECPUPORABORTO(); Declaratividad op
		return NULL ;
	}
	char *aux = malloc(tamano + 1);
	mensaje != NULL ?
			desempaquetar2(mensaje, aux, 0) : printf("Esta estaba vacia\n");
	free(mensaje);
	return aux;
}
//	return mensaje;
////	char *msjCabecera = malloc(TAMANO_CABECERA);
////	recv(socketUMV, msjCabecera, TAMANO_CABECERA, MSG_WAITALL);
////	int tamanoMensaje;
////	desempaquetar2(msjCabecera, &tamanoMensaje, razon, 0);
////	if (razon == CONFIRMACION) {
////		char *mensaje = malloc(tamanoMensaje);
////		recv(socketUMV, mensaje, tamanoMensaje, MSG_WAITALL);
////		char *aux = malloc(tamano);
////		desempaquetar2(mensaje, aux, 0);
////		return aux;
////	} else {
////		//notificar error
////		return "";
////	}
//} //UFF SOY TAN HOMBRE
//
t_puntero definirVariable(t_nombre_variable identificador_variable) { //Chequeada
	log_debug(logs, "Definiendo variable...");
	enviarBytesAUMV(pcbEnUso->segmento_Stack,
			pcbEnUso->cursor_Stack - pcbEnUso->segmento_Stack
					+ pcbEnUso->tamanio_Contexto_Actual * 5, 1,
			&identificador_variable);
	char *identificadorCopiado = malloc(2);
	identificadorCopiado[0] = identificador_variable;
	identificadorCopiado[1] = 0;
	int *desplazamiento = malloc(4);
	*desplazamiento = pcbEnUso->cursor_Stack - pcbEnUso->segmento_Stack
			+ pcbEnUso->tamanio_Contexto_Actual * 5;
	log_debug(logs, "Puse una variable de nombre= %c y de offset= %d",
			identificador_variable, *desplazamiento);
//	printf("Puse una variable de nombre = %c y de offset = %d\n",
//			identificador_variable, *desplazamiento);
	dictionary_put(diccionarioDeVariables, identificadorCopiado,
			desplazamiento);
	pcbEnUso->tamanio_Contexto_Actual++;
	return (pcbEnUso->tamanio_Contexto_Actual - 1) * 5;
}

t_puntero obtenerPosicionVariable(t_nombre_variable identificador_variable) { //Chequeada
	log_debug(logs, "Obteniendo posición con identif= %c",
			identificador_variable);
//	printf("llame obtenerPosicion con identif = %c\n", identificador_variable);
	char *identificadorPosta = malloc(2);
	identificadorPosta[0] = identificador_variable;
	identificadorPosta[1] = 0;
	int *aux = dictionary_get(diccionarioDeVariables, identificadorPosta);
	log_debug(logs, "La dirección obtenida es %d", *aux);
//	printf("Y la direccion obtenida es %d\n", *aux);
	return (aux == NULL ) ? -1 : *aux;
}
//
t_valor_variable dereferenciar(t_puntero direccion_variable) { //Chequeada
	log_debug(logs, "Dereferenciado con dirreción = %d", direccion_variable);
	//printf("llame dereferenciar con direccion = %d\n", direccion_variable);

	char *aux =
			solicitarBytesAUMV(pcbEnUso->segmento_Stack,
					/*pcbEnUso->cursor_Stack - pcbEnUso->segmento_Stack +*/direccion_variable
							+ 1, 4);
	t_valor_variable valor;
	memcpy(&valor, aux, 4);
	log_debug(logs, "El valor obtenido es= %d", valor);
	//printf("y el valor obtenido es = %d\n", valor);
	free(aux);
	return valor;
}
////
void asignar(t_puntero direccion_variable, t_valor_variable valor) { //Chequeada
	log_debug(logs, "Llamé asignar con dirección= %d y valor= %d",
			direccion_variable, valor);
//	printf("llame asignar con direccion = %d y valor = %d\n",
//			direccion_variable, valor);
	log_error(logs, "DIRECCION VARIABLE ES: %d, LA DIFERENCIA ES %d",
			direccion_variable,
			pcbEnUso->cursor_Stack - pcbEnUso->segmento_Stack);
	if (direccion_variable
			< pcbEnUso->cursor_Stack - pcbEnUso->segmento_Stack) {
		direccion_variable += pcbEnUso->cursor_Stack - pcbEnUso->segmento_Stack;
	}
	log_error(logs, "LA NUEVA DIRECCION ES %d", direccion_variable);
	enviarBytesAUMV(pcbEnUso->segmento_Stack,
	/*pcbEnUso->cursor_Stack - pcbEnUso->segmento_Stack
	 +*/direccion_variable + 1, 4, &valor);
	log_debug(logs, "El valor asignado es %d", valor);
//	printf("el valor asignado es %d\n", valor);
}
//
t_valor_variable obtenerValorCompartida(t_nombre_compartida variable) {
	log_info(logs, "Ejecute obtenerValorCompartida con %s", variable);
//	int razon = OBTENER_VALOR;
//	t_paquete *paquete = serializar2(crear_nodoVar(variable, strlen(variable)),
//			0);
//	t_paquete *header = serializar2(crear_nodoVar(&(paquete->tamano), 4),
//			crear_nodoVar(&razon, 4), 0);
//	send(socketKernel, header->msj, TAMANO_CABECERA, 0);
//	send(socketKernel, paquete->msj, paquete->tamano, 0);
//	char *msjCabecera = malloc(TAMANO_CABECERA);
//	//bind y listen mediante
//	recv(socketKernel, msjCabecera, TAMANO_CABECERA, MSG_WAITALL);
//	int tamanoMensaje;
//	desempaquetar2(msjCabecera, &tamanoMensaje, &razon, 0);
//	//quizas deberia preguntar por la razon, pero ni da la verdad.
//	char *mensaje = malloc(tamanoMensaje);
//	recv(socketKernel, mensaje, tamanoMensaje, MSG_WAITALL);
//	t_valor_variable *aux = malloc(sizeof(t_valor_variable));
//	desempaquetar2(mensaje, aux, 0);
//	return *aux;
	int aux;
	int razon;
	enviarConRazon(socketKernel, logs, OBTENER_VALOR,
			serializar2(crear_nodoVar(variable, strlen(variable)), 0));
	char *respuesta = recibirConRazon(socketKernel, &razon, logs);
	desempaquetar2(respuesta, &aux, 0);
	free(respuesta);
	return aux;
}
//
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
//	log_info(logs, "Ejecute irAlLabel con %s", etiqueta);
//	Segun yo a las 12, al comienzo del programa en el cpu, debemos traer el segmento de etiquetas hacia el.
//	etiquetas es una global en la que copiamos enterito el indice de etiquetas
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
	//log_info(logs, "Ejecute llamarSinRetorno con %s", etiqueta);
	//Yo crearia un nuevo diccionario, pero no se como es esta cosa.
	dictionary_clean_and_destroy_elements(diccionarioDeVariables, (void*) free);
	enviarBytesAUMV(pcbEnUso->segmento_Stack,
			pcbEnUso->cursor_Stack - pcbEnUso->segmento_Stack
					+ pcbEnUso->tamanio_Contexto_Actual * 5, 4,
			&(pcbEnUso->cursor_Stack));
	enviarBytesAUMV(pcbEnUso->segmento_Stack,
			pcbEnUso->cursor_Stack - pcbEnUso->segmento_Stack
					+ pcbEnUso->tamanio_Contexto_Actual * 5 + 4, 4,
			&(pcbEnUso->program_Counter));
	printf("El viejo cursor stack es: %d\n", pcbEnUso->cursor_Stack);
	pcbEnUso->cursor_Stack = pcbEnUso->cursor_Stack
			+ pcbEnUso->tamanio_Contexto_Actual * 5 + 8;
	printf("El nuevo cursor stack es: %d\n", pcbEnUso->cursor_Stack);
	pcbEnUso->tamanio_Contexto_Actual = 0;
	irAlLabel(etiqueta);
}
void llamarConRetorno(t_nombre_etiqueta etiqueta, t_puntero donde_retornar) {
	//log_info(logs, "Ejecute llamarConRetorno con %s", etiqueta);
	dictionary_clean_and_destroy_elements(diccionarioDeVariables, (void*) free);
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
//	if (pcbEnUso->tamanio_Contexto_Actual)
//		chota="El estado final de las variables es: \n";
//	 else
//		chota="El programa no poseia variables";
//	aux=strdup(chota);
//	acum+=strlen(aux);
	while (i < pcbEnUso->tamanio_Contexto_Actual) {
		char identif = *(solicitarBytesAUMV(pcbEnUso->segmento_Stack, i * 5, 1));
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
		axu = string_from_format("El programa no tenia variables");
//	puts(aux); //Esto es para ver como esta de este lado
	return axu;
}
void finalizar(void) {
	log_info(logs, "Ejecuté finalizar");
	if (pcbEnUso->cursor_Stack == pcbEnUso->segmento_Stack) {
//		char *tituloFinal = "El estado final de las variables es:\n";
//		char *tituloFinal2 =strdup(tituloFinal);
//		enviarConRazon(socketKernel, logs, IMPRIMIR_TEXTO, serializar2(crear_nodoVar(tituloFinal2, strlen(tituloFinal2)), 0));
		char *mensajeFinal = generarListadoVariables();
		if (mensajeFinal != NULL ) {
			int razon;
			enviarConRazon(socketKernel, logs, IMPRIMIR_TEXTO,
					serializar2(
							crear_nodoVar(mensajeFinal, strlen(mensajeFinal)),
							0));
			recibirConRazon(socketKernel, &razon, logs);//Deberia preguntar si es confirmacion pero bue
		}
		programaFinalizado = 1;
		enviarConRazon(socketKernel, logs, SALIDA_NORMAL,
				serializarPCB(pcbEnUso));
	} else {
		char *p_programCounter = solicitarBytesAUMV(pcbEnUso->segmento_Stack,
				pcbEnUso->cursor_Stack - pcbEnUso->segmento_Stack - 4, 4);
		char *p_cursorCtxto = solicitarBytesAUMV(pcbEnUso->segmento_Stack,
				pcbEnUso->cursor_Stack - pcbEnUso->segmento_Stack - 8, 4);
		int proximo_cursor_stack;
		printf("el viejo program counter es %d\n", pcbEnUso->program_Counter);
		memcpy(&(pcbEnUso->program_Counter), p_programCounter, 4);
		printf("el nuevo program counter es %d\n", pcbEnUso->program_Counter);
		memcpy(&proximo_cursor_stack, p_cursorCtxto, 4);
		pcbEnUso->tamanio_Contexto_Actual = (pcbEnUso->cursor_Stack - 8
				- proximo_cursor_stack) / 5;
		pcbEnUso->cursor_Stack = proximo_cursor_stack;
//		free(diccionarioDeVariables);
//		diccionarioDeVariables=dictionary_create();
		dictionary_clean_and_destroy_elements(diccionarioDeVariables,
				(void*) free);
		recuperarDiccionario();
//		memcpy(&(pcbEnUso->program_Counter), p_programCounter, 4);
//		memcpy(&(pcbEnUso->cursor_Stack), p_cursorCtxto, 4);
		free(p_programCounter);
		free(p_cursorCtxto);
	}
} //Esa division es bastante discutible
void retornar(t_valor_variable retorno) {
	log_info(logs, "Ejecute retornar con %d", retorno);
	char *p_retorno = solicitarBytesAUMV(pcbEnUso->segmento_Stack,
			pcbEnUso->cursor_Stack - pcbEnUso->segmento_Stack - 4, 4);
	int dirRetorno;
	memcpy(&dirRetorno, p_retorno, 4);
	free(p_retorno);
	enviarBytesAUMV(pcbEnUso->segmento_Stack, dirRetorno + 1, 4, &retorno);
	char *p_programCounter = solicitarBytesAUMV(pcbEnUso->segmento_Stack,
			pcbEnUso->cursor_Stack - pcbEnUso->segmento_Stack - 8, 4);
	char *p_cursorCtxto = solicitarBytesAUMV(pcbEnUso->segmento_Stack,
			pcbEnUso->cursor_Stack - pcbEnUso->segmento_Stack - 12, 4);
	int proximo_cursor_stack;
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
	free(p_programCounter);
	free(p_cursorCtxto);
}
void imprimir(t_valor_variable valor_mostrar) {
	log_info(logs, "Ejecute imprimir con %d", valor_mostrar);
//	char *cadena=string_from_format("%d\n", valor_mostrar);
//	enviarConRazon(socketKernel, logs, IMPRIMIR_TEXTO, serializar2(crear_nodoVar(cadena, strlen(cadena)), 0));
//	enviarConRazon(socketKernel, logs, IMPRIMIR,
//	 serializar2(crear_nodoVar(&valor_mostrar, sizeof(t_valor_variable)),
//	 0)); //TODO
	imprimirTexto(string_from_format("%d ", valor_mostrar));
}
void imprimirTexto(char* texto) {
	int razon;
	log_info(logs, "Ejecute imprimirTexto con %s", texto);
//	enviarConRazon(socketKernel, logs, IMPRIMIR_TEXTO, serializar2(crear_nodoVar(texto, strlen(texto)), 0));
//	int *razon = malloc(sizeof(int));
//	*razon = MOSTRAR_TEXTO;
//	t_paquete *paquete = serializar2(crear_nodoVar(texto, strlen(texto) + 1),
//			0);
//	t_paquete *header = serializar2(crear_nodoVar(&(paquete->tamano), 4),
//			crear_nodoVar(razon, 4), 0);
//	send(socketKernel, header->msj, TAMANO_CABECERA, 0);
//	send(socketKernel, paquete->msj, paquete->tamano, 0);
//	//Quizas deberiamos esperar la respuesta
	enviarConRazon(socketKernel, logs, IMPRIMIR_TEXTO,
			serializar2(crear_nodoVar(texto, strlen(texto)), 0)); //TODO
	recibirConRazon(socketKernel, &razon, logs);
}
void entradaSalida(t_nombre_dispositivo dispositivo, int tiempo) {
	log_info(logs, "Ejecute entradaSalida con %s", dispositivo);
//	int *razon = malloc(sizeof(int));
//	*razon = SALIDA_POR_BLOQUEO;
//	t_paquete *paquete = serializar2(crear_nodoVar(&pcb, sizeof(pcb)),
//			crear_nodoVar(&tiempo, 4),
//			crear_nodoVar(dispositivo, strlen(dispositivo)), 0);
//	t_paquete *header = serializar2(crear_nodoVar(&(paquete->tamano), 4),
//			crear_nodoVar(razon, 4), 0);
//	send(socketKernel, header->msj, TAMANO_CABECERA, 0);
//	send(socketKernel, paquete->msj, paquete->tamano, 0);
//	log_info(log, "Se desalojó un programa de esta CPU");
	/*	t_paquete * paquetePCB = serializarPCB(pcbEnUso);
	 int tamano = paquetePCB->tamano;
	 enviarConRazon(socketKernel, logs, SALIO_POR_IO,
	 serializar2(crear_nodoVar(paquetePCB->msj, tamano),
	 crear_nodoVar(dispositivo, strlen(dispositivo) + 1),
	 crear_nodoVar(&tiempo, 4), 0));*///TODO
}
void wait(t_nombre_semaforo identificador_semaforo) {
	log_info(logs, "Ejecute wait con %s", identificador_semaforo);
//	//PASO
//	int razon = WAIT, tamano;
//	t_paquete *paquete = serializar2(
//			crear_nodoVar(identificador_semaforo,
//					strlen(identificador_semaforo)),
//			crear_nodoVar(&pcb, sizeof(pcb)), 0);
//	t_paquete *header = serializar2(crear_nodoVar(&(paquete->tamano), 4),
//			crear_nodoVar(&razon, 4), 0);
//	send(socketKernel, (void*) header->msj, TAMANO_CABECERA, 0);
//	send(socketKernel, (void*) paquete->msj, paquete->tamano, 0);
//	char *cabecera = malloc(TAMANO_CABECERA);
//	//lo que haga falta
//	recv(socketKernel, (void*) cabecera, TAMANO_CABECERA, MSG_WAITALL);
//	desempaquetar2(cabecera, &tamano, &razon, 0);
//	if (!razon) {
//		log_info(log, "Se desalojó un programa de esta CPU");
//	}
	int razon;
	enviarConRazon(socketKernel, logs, WAIT,
			serializar2(
					crear_nodoVar(identificador_semaforo,
							strlen(identificador_semaforo) + 1), 0));
	recibirConRazon(socketKernel, &razon, logs);
	//TODO tiene razón el hijo de puta del warning.
	if (razon == DESALOJAR_PROGRAMA) {
		pcbEnUso->program_Counter++;
		enviarConRazon(socketKernel, logs, 4, serializarPCB(pcbEnUso));
		programaBloqueado = 1;
	} else {
		//NARANJA;
	}
}

void signalPropia(t_nombre_semaforo identificador_semaforo) {
	log_info(logs, "Ejecute signalPropia %s", identificador_semaforo);
//	int razon = SIGNAL;
//	t_paquete *paquete = serializrar2(
//			crear_nodoVar(identificador_semaforo,
//					strlen(identificador_semaforo)), 0);
//	t_paquete *header = serializar2(crear_nodoVar(&(paquete->tamano), 4),
//			crear_nodoVar(&razon, 4), 0);
//	send(socketKernel, (void*) header->msj, TAMANO_CABECERA, 0);
//	send(socketKernel, (void*) paquete->msj, paquete->tamano, 0);
	/*Creo que no hace falta un recv, porque no le afecta directamente a el,
	 * el PCP deberia sacar de la cola de bloqueados a quien corresponda y listo.
	 */
	enviarConRazon(socketKernel, logs, SIGNAL,
			serializar2(
					crear_nodoVar(identificador_semaforo,
							strlen(identificador_semaforo) + 1), 0)); //TODO
}
