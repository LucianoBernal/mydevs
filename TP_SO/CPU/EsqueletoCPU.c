#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <biblioteca_comun/bibliotecaSockets.h>
#include <biblioteca_comun/Serializacion.h>
#include <biblioteca_comun/definiciones.h>
#include <commons/collections/dictionary.h>
#include "funcionesParser.h"
#include <commons/config.h>
#include "EsqueletoCPU.h"

#define HANDSHAKE_SIZE 16
char *IP_UMV, *IP_KERNEL;
char* PUERTO_UMV, *PUERTO_PCP;
char *etiquetas;
t_PCB *pcbEnUso;
t_dictionary *diccionarioDeVariables;
t_log *logs;
int programaFinalizado;
int programaBloqueado;
int programaAbortado;
int cpu_ocupada = 0;

void obtenerDatosConfig(t_config *config) {
	IP_UMV = config_get_string_value(config, "IP_UMV");
	IP_KERNEL = config_get_string_value(config, "IP_KERNEL");
	PUERTO_UMV = config_get_string_value(config, "PUERTO_UMV");
	PUERTO_PCP = config_get_string_value(config, "PUERTO_PCP");
	printf(
			"datos levantados;\nIP_UMV=%s\nIP KERNEL=%s\nPUERTO UMV=%s\nPUERTO PCP=%s\n",
			IP_UMV, IP_KERNEL, PUERTO_UMV, PUERTO_PCP);
}
void cerrarCPU(t_log *log) {
	log_error(log, "Deberia estar cerrando el CPU");
}
int main(int arc, char **argv) {
	vincPrimitivas();
	diccionarioDeVariables = dictionary_create();
	pcbEnUso = malloc(sizeof(t_PCB));
	proceso_terminado = 0;
	proceso_bloqueado = 0;
	logs = log_create("logCPU", "CPU", true, LOG_LEVEL_TRACE);
	t_config *config = config_create(argv[1]);
	obtenerDatosConfig(config);

	if (!(socketUMV = conectarCliente(IP_UMV, PUERTO_UMV, logs))) {
		log_error(logs, "No se pudo conectar a la UMV.");
		cerrarCPU(logs);
	}
	if (!(socketKernel = conectarCliente(IP_KERNEL, PUERTO_PCP, logs))) {
		log_error(logs, "No se pudo conectar al Kernel.");
		cerrarCPU(logs);
	}
	char *saludo = "CPU";
	char* respuestaUMV = malloc(8);
	send(socketUMV, saludo, 4, 0);
	recv(socketUMV, respuestaUMV, 4, MSG_WAITALL);

	if (!strncmp(respuestaUMV, "UMV", 3)) {
		log_info(logs, "El handshake con la UMV salió bien.");
	} else {
		log_error(logs, "El handshake con la UMV salió mal.");
	}
	free(respuestaUMV);
	send(socketKernel, saludo, 4, 0); //TODO el saludo tiene tamaño 4?
	char *respuestaKernel = malloc(7);
	int *razon = malloc(sizeof(int));
	char * paquete = recibirConRazon(socketKernel, razon, logs);
	desempaquetar2(paquete, respuestaKernel, &quantumDeKernel, &retardo, 0);
	if (!strncmp(respuestaKernel, "Kernel", 6)) {
		log_info(logs, "El handshake con el Kernel salió bien.");
	} else {
		log_error(logs, "El handshake con el Kernel salió mal.");
	}

	sigusr1_desactivado = 1;
	if (signal(SIGUSR1, sig_handler) == SIG_ERR )
		log_error(logs, "Error al atrapar señal SIGUSR1");
	if (signal(SIGINT, sig_handler) == SIG_ERR )
		log_error(logs, "Error al atrapar señal SIGINT");

	while (sigusr1_desactivado) {
		//Recibo un PCB
		int razon;
		cpu_ocupada = 0;
		char* mensaje = recibirConRazon(socketKernel, &razon, logs);
		if(mensaje==NULL&&razon!=CONFIRMACION/*&&razon!=SIGUSR_1*/){
			log_error(logs, "Se recibió mal.");
			exit(EXIT_FAILURE);
		}
		cpu_ocupada = 1;
		char* pcbEmpaquetado = malloc(sizeof(t_PCB) + 4 * 9 + 32);
//		t_PCB *pcb = malloc(sizeof(t_PCB));
		desempaquetar2(mensaje, pcbEmpaquetado, 0);
		desempaquetarPCB(pcbEnUso, pcbEmpaquetado);
		free(mensaje);
		free(pcbEmpaquetado);
		int pid = pcbEnUso->program_id;
		enviarConRazon(socketUMV, logs, CAMBIAR_PROCESO_ACTIVO,
				serializar2(crear_nodoVar(&pid, 4), 0));
		etiquetas = solicitarBytesAUMV(pcbEnUso->indice_de_Etiquetas, 0,
				pcbEnUso->tamanio_Indice_de_Etiquetas);
//		actualizarDiccionarioDeVariables(pcb);
		recuperarDiccionario();
		int lineasAnalizadas = 0;
		programaBloqueado = 0;
		programaFinalizado = 0;
		programaAbortado = 0;
		int ubInstruccion, largoInstruccion;
		while (lineasAnalizadas < quantumDeKernel&& !programaBloqueado
				&& !programaFinalizado && !programaAbortado) {
			char *msjInstruccion = solicitarBytesAUMV(
					pcbEnUso->indice_de_Codigo, pcbEnUso->program_Counter * 8,
					8);
			memcpy(&ubInstruccion, msjInstruccion, 4);
			memcpy(&largoInstruccion, msjInstruccion + 4, 4);
			char *literalInstruccion = solicitarBytesAUMV(
					pcbEnUso->segmento_Codigo, ubInstruccion, largoInstruccion);
			literalInstruccion[largoInstruccion] = 0;
			log_debug(logs, "El literal es juancito: %s", literalInstruccion);
			analizadorLinea(_depurar_sentencia(strdup(literalInstruccion)), &funciones_Ansisop,
					&funciones_kernel);
			pcbEnUso->program_Counter++;
			lineasAnalizadas++;
			log_info(logs, "ESTA ES LA INSTRUCCION %d", lineasAnalizadas);
			log_info(logs, "EL PID DEL BASTARDO ES %d", pcbEnUso->program_id);
			//printf("ESTA ES LA INSTRUCCION %d\n", lineasAnalizadas);
		}
		dictionary_clean_and_destroy_elements(diccionarioDeVariables, (void*)free);
		/*
		enviarConRazon(socketKernel, logs, razon, serializarPCB(pcbEnUso));
		free(pcbEnUso);
		*/
		if (programaBloqueado){
			log_debug(logs, "El programa salió por bloqueo");
		}
		if (programaAbortado){
			log_debug(logs, "El programa aborto");
		}
		if (programaFinalizado){
			log_debug(logs, "El programa finalizo");
		}
		if (lineasAnalizadas==quantumDeKernel&&!programaFinalizado&&!programaBloqueado&&!programaAbortado){
			enviarConRazon(socketKernel, logs, SALIDA_POR_QUANTUM, serializarPCB(pcbEnUso));
		}
		printf("Sali del while, lineasAnalizadas=%d y quantumDeKernel=%d\n", lineasAnalizadas, quantumDeKernel);
	}
	enviarConRazon(socketKernel, logs, SIGUSR_1, NULL);

	return 0;
}

//void actualizarDiccionarioDeVariables (t_PCB* pcb){
//	if (pcb->tamanio_Contexto_Actual){
//		char *datosContexto =solicitarBytesAUMV(pcb->cursor_Stack, 0, pcb->tamanio_Contexto_Actual);
//		int cantVariables=0;
//		while (cantVariables*5 < pcb->tamanio_Contexto_Actual){
//			char *nombreTemporal=malloc(1);
//			int *valorTemporal=malloc(sizeof(int));
//			memcpy(nombreTemporal, datosContexto+cantVariables*5, 1);
//			memcpy(valorTemporal, datosContexto+cantVariables*5+1, 4);
//			dictionary_put(diccionarioDeVariables, nombreTemporal, valorTemporal);
//		}
//	}
//}
//Difiere en detalles con la de arriba...

void sig_handler(int signo) {
	if (signo == SIGUSR1) {
		sigusr1_desactivado = 0;
		log_info(logs,
				"Se recibió la señal SIGUSR_1, la CPU se cerrara al finalizar la ejecucion actual");
		if (!cpu_ocupada) {
			exit(EXIT_SUCCESS);
		}
	} else if (signo == SIGINT) {
		log_info(logs,
				"Se recibió la señal SIGNIT, la CPU se cerrara abruptamente");
		exit(EXIT_FAILURE);
	}
}
