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
	logs = log_create("logCPU", "CPU", true, LOG_LEVEL_INFO);
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
	int quantum;
	int retardo;
	int *razon = malloc(sizeof(int));
	char * paquete = recibirConRazon(socketKernel, razon,logs);
	desempaquetar2(paquete, respuestaKernel, &quantum, &retardo, 0);
	puts(respuestaKernel);
	if (!strncmp(respuestaKernel, "Kernel", 6)) {
		log_info(logs, "El handshake con el Kernel salió bien.");
	} else {
		log_error(logs, "El handshake con el Kernel salió mal.");
	}
//	int pidMancodeado=8331;
//	enviarConRazon(socketUMV, logs, CAMBIAR_PROCESO_ACTIVO, serializar2(crear_nodoVar(&pidMancodeado, 4), 0));
//	int base, offset, tamano;
//	char*mensaje=malloc(50);
//	while (strncmp(mensaje, "exit", 4)){
//		if (!strncmp(mensaje, "enviar", 6)){
//			printf("Ingrese base:");
//			scanf("%d", &base);
//			printf("\nIngrese offset:");
//			scanf("%d", &offset);
//			printf("\nIngrese tamano:");
//			scanf("%d", &tamano);
//			scanf("%s", mensaje);
//			enviarBytesAUMV(base, offset, tamano, mensaje);
//			printf("\nlo enviado fue: %s\n", mensaje);
//		}else if(!strncmp(mensaje, "solicitar", 9)){
//			printf("Ingrese base:");
//			scanf("%d", &base);
//			printf("Ingrese offset:");
//			scanf("%d", &offset);
//			printf("Ingrese tamano:");
//			scanf("%d", &tamano);
//			mensaje=solicitarBytesAUMV(base, offset, tamano);
//			printf("mensaje=%s\n", mensaje);
//			if (mensaje==NULL){
//				mensaje=malloc(40);
//			}
//		}else{
//			printf("\nLo que ingresaste no coincide, si queres salir manda exit m3n\n");
//		}
//		fflush(stdin);
//		gets(mensaje);
//	}
//
	signal(SIGINT, (sighandler_t) manejar_seniales);
	signal(SIGUSR1, (sighandler_t) manejar_seniales);

	while (sigusr1_activado == 0) {
		//Recibo un PCB
		int razon;
		char* mensaje = recibirConRazon(socketKernel, &razon, logs);
		char* pcbEmpaquetado = malloc(sizeof(t_PCB) + 4 * 9 + 32);
//		t_PCB *pcb = malloc(sizeof(t_PCB));
		desempaquetar2(mensaje, pcbEmpaquetado, 0);
		desempaquetarPCB(pcbEnUso, pcbEmpaquetado);
		free(mensaje);
		free(pcbEmpaquetado);

		etiquetas = solicitarBytesAUMV(pcbEnUso->indice_de_Etiquetas, 0,
				pcbEnUso->tamanio_Indice_de_Etiquetas);
//		actualizarDiccionarioDeVariables(pcb);
		recuperarDiccionario();
		int lineasAnalizadas = 0;
		int programaBloqueado = 0;
		int ubInstruccion, largoInstruccion;
		while (lineasAnalizadas < quantumDeKernel || programaBloqueado) {
			//Si empieza en la instruccion 0 deberia ser progra_Counter-1 ???
			char *msjInstruccion = solicitarBytesAUMV(pcbEnUso->indice_de_Codigo,
					pcbEnUso->program_Counter * 8, 8);
			desempaquetar2(msjInstruccion, &ubInstruccion, &largoInstruccion,
					0);
			char *literalInstruccion = solicitarBytesAUMV(pcbEnUso->segmento_Codigo,
					ubInstruccion, largoInstruccion);
			analizadorLinea(literalInstruccion, &funciones_Ansisop,
					&funciones_kernel);
			pcbEnUso->program_Counter++;
			lineasAnalizadas++;
		}
		enviarConRazon(socketKernel, logs, razon, serializarPCB(pcbEnUso));
		free(pcbEnUso);
	}
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
void recuperarDiccionario() {
	int i = 0, *desplazamiento;
	char *nombre, *stack = solicitarBytesAUMV(pcbEnUso->segmento_Stack,
			pcbEnUso->cursor_Stack, pcbEnUso->tamanio_Contexto_Actual * 5);
	while (i < pcbEnUso->tamanio_Contexto_Actual) {
		nombre = malloc(2);
		desplazamiento = malloc(4);
		memcpy(nombre, stack + i * 5, 1);
		nombre[1] = 0;
		*desplazamiento = i * 5;
		dictionary_put(diccionarioDeVariables, nombre, desplazamiento);
		i++;
	}
}

sighandler_t manejar_seniales(int senal) {
	switch (senal) {
	case SIGINT:
		log_info(logs, "Se recibió la señal SIGNIT");
		break;
	case SIGUSR1:
		log_info(logs, "Se recibió la señal SIGUSR_1");
	}
	return NULL ;
}

