
#include <parser/parser/parser.h>
#include <parser/parser/metadata_program.h>
#include "funcionesParser.h"
#include <biblioteca_comun/bibliotecaSockets.h>
#include <biblioteca_comun/Serializacion.h>
#include <semaphore.h>

extern sem_t mutexSigu;
extern int socketUMV, socketKernel, sigusr1_desactivado;
extern t_log *logs;
extern t_PCB *pcbEnUso;
extern t_dictionary *diccionarioDeVariables;
extern char *etiquetas;
extern int programaFinalizado, programaBloqueado, programaAbortado;

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
	int i = strlen(sentencia);
	while (string_ends_with(sentencia, "\n")) {
		i--;
		sentencia = string_substring_until(sentencia, i);
	}
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
	int razon;
	enviarConRazon(socketUMV, logs, SOLICITAR_A_UMV,
			serializar2(crear_nodoVar(&base, sizeof(t_puntero)),
					crear_nodoVar(&desplazamiento, sizeof(t_puntero)),
					crear_nodoVar(&tamano, 4), 0));
	char *mensaje = recibirConRazon(socketUMV, &razon, logs);
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
t_puntero definirVariable(t_nombre_variable identificador_variable) {
	log_debug(logs, "Definiendo variable...");
	enviarVariableAUMV(pcbEnUso->segmento_Stack,
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
	dictionary_put(diccionarioDeVariables, identificadorCopiado,
			desplazamiento);
	pcbEnUso->tamanio_Contexto_Actual++;
	return (pcbEnUso->tamanio_Contexto_Actual - 1) * 5;
}

t_puntero obtenerPosicionVariable(t_nombre_variable identificador_variable) {
	log_debug(logs, "Obteniendo posición con identif= %c",
			identificador_variable);
	char *identificadorPosta = malloc(2);
	identificadorPosta[0] = identificador_variable;
	identificadorPosta[1] = 0;
	int *aux = dictionary_get(diccionarioDeVariables, identificadorPosta);
	log_debug(logs, "La dirección obtenida es %d", *aux);
	return (aux == NULL ) ? -1 : *aux;
}

t_valor_variable dereferenciar(t_puntero direccion_variable) {
	log_debug(logs, "Dereferenciado con dirreción = %d", direccion_variable);
	char *aux =
			solicitarBytesAUMV(pcbEnUso->segmento_Stack,
					direccion_variable
							+ 1, 4);
	t_valor_variable valor;
	if (aux==NULL)
		return -736;
	memcpy(&valor, aux, 4);
	log_debug(logs, "El valor obtenido es= %d", valor);
	free(aux);
	return valor;
}
void asignar(t_puntero direccion_variable, t_valor_variable valor) {
	if (direccion_variable
			< pcbEnUso->cursor_Stack - pcbEnUso->segmento_Stack) {
		direccion_variable += pcbEnUso->cursor_Stack - pcbEnUso->segmento_Stack;
	}
	log_debug(logs, "Llamé asignar con dirección= %d y valor= %d",
			direccion_variable, valor);
	enviarBytesAUMV(pcbEnUso->segmento_Stack,direccion_variable + 1, 4, &valor);
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
	pcbEnUso->program_Counter = metadata_buscar_etiqueta(
			_depurar_sentencia(etiqueta), etiquetas,
			pcbEnUso->tamanio_Indice_de_Etiquetas) - 1;
}
//
void llamarSinRetorno(t_nombre_etiqueta etiqueta) {
	dictionary_clean_and_destroy_elements(diccionarioDeVariables, (void*) free);
	enviarBytesAUMV(pcbEnUso->segmento_Stack,
			pcbEnUso->cursor_Stack - pcbEnUso->segmento_Stack
					+ pcbEnUso->tamanio_Contexto_Actual * 5, 4,
			&(pcbEnUso->cursor_Stack));
	enviarBytesAUMV(pcbEnUso->segmento_Stack,
			pcbEnUso->cursor_Stack - pcbEnUso->segmento_Stack
					+ pcbEnUso->tamanio_Contexto_Actual * 5 + 4, 4,
			&(pcbEnUso->program_Counter));
	pcbEnUso->cursor_Stack = pcbEnUso->cursor_Stack
			+ pcbEnUso->tamanio_Contexto_Actual * 5 + 8;
	pcbEnUso->tamanio_Contexto_Actual = 0;
	irAlLabel(etiqueta);
}
void llamarConRetorno(t_nombre_etiqueta etiqueta, t_puntero donde_retornar) {
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
	pcbEnUso->cursor_Stack = pcbEnUso->cursor_Stack
			+ pcbEnUso->tamanio_Contexto_Actual * 5 + 12;
	pcbEnUso->tamanio_Contexto_Actual = 0;
	irAlLabel(etiqueta);
}
char *generarListadoVariables() {
	int i = 0, acum = 0;
	char *aux = NULL, *axu;
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
		char *p_programCounter = solicitarBytesAUMV(pcbEnUso->segmento_Stack,
				pcbEnUso->cursor_Stack - pcbEnUso->segmento_Stack - 4, 4);
		char *p_cursorCtxto = solicitarBytesAUMV(pcbEnUso->segmento_Stack,
				pcbEnUso->cursor_Stack - pcbEnUso->segmento_Stack - 8, 4);
		if (programaAbortado)
			goto falloSolicitar;
		int proximo_cursor_stack;
		memcpy(&(pcbEnUso->program_Counter), p_programCounter, 4);
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
	char *p_retorno = solicitarBytesAUMV(pcbEnUso->segmento_Stack,
			pcbEnUso->cursor_Stack - pcbEnUso->segmento_Stack - 4, 4);
	if (programaAbortado)
		goto falloSolicitar;
	int dirRetorno;
	memcpy(&dirRetorno, p_retorno, 4);
	enviarBytesAUMV(pcbEnUso->segmento_Stack, dirRetorno + 1, 4, &retorno);
	char *p_programCounter = solicitarBytesAUMV(pcbEnUso->segmento_Stack,
			pcbEnUso->cursor_Stack - pcbEnUso->segmento_Stack - 8, 4);
	char *p_cursorCtxto = solicitarBytesAUMV(pcbEnUso->segmento_Stack,
			pcbEnUso->cursor_Stack - pcbEnUso->segmento_Stack - 12, 4);
	int proximo_cursor_stack;
	if (programaAbortado)
		goto falloSolicitar;
	memcpy(&pcbEnUso->program_Counter, p_programCounter, 4);
	memcpy(&proximo_cursor_stack, p_cursorCtxto, 4);
	pcbEnUso->tamanio_Contexto_Actual = (pcbEnUso->cursor_Stack - 12
			- proximo_cursor_stack) / 5;
	pcbEnUso->cursor_Stack = proximo_cursor_stack;
	dictionary_clean_and_destroy_elements(diccionarioDeVariables, (void*) free);
	recuperarDiccionario();
	falloSolicitar:;
	free(p_retorno);
	free(p_programCounter);
	free(p_cursorCtxto);
}
void imprimir(t_valor_variable valor_mostrar) {
	if (valor_mostrar!=-736)
	imprimirTexto(string_from_format("%d ", valor_mostrar));
}
void imprimirTexto(char* texto) {
	int razon;
	log_info(logs, "Ejecute imprimir con %s", texto);
	enviarConRazon(socketKernel, logs, IMPRIMIR_TEXTO,
			serializar2(crear_nodoVar(texto, strlen(texto)), 0)); //TODO
	recibirConRazon(socketKernel, &razon, logs);
}
void entradaSalida(t_nombre_dispositivo dispositivo, int tiempo) {
	log_info(logs, "Ejecute entradaSalida con %s", dispositivo);
	pcbEnUso->program_Counter++;
	sem_wait(&mutexSigu);
	if (!sigusr1_desactivado) {
		enviarConRazon(socketKernel, logs, SIGUSR_1, serializar2(crear_nodoVar(&programaBloqueado,4),0) );
	}
	sem_post(&mutexSigu);
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
	}
}

void signalPropia(t_nombre_semaforo identificador_semaforo) {
	log_info(logs, "Ejecute signalPropia %s", identificador_semaforo);
	enviarConRazon(socketKernel, logs, SIGNAL,
			serializar2(
					crear_nodoVar(identificador_semaforo,
							strlen(identificador_semaforo)), 0)); //TODO
}
