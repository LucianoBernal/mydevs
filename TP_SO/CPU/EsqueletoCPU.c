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

#include <commons/config.h>
#include "EsqueletoCPU.h"

#define HANDSHAKE_SIZE 16
char *IP_UMV, *IP_KERNEL;
char* PUERTO_UMV, *PUERTO_PCP;

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
	proceso_terminado = 0;
	proceso_bloqueado = 0;
	log = log_create("logCPU", "CPU", true, LOG_LEVEL_INFO);
	t_config *config = config_create(argv[1]);
	obtenerDatosConfig(config);

	if (!(socketUMV = conectarCliente(IP_UMV, PUERTO_UMV, log))) {
		log_error(log, "No se pudo conectar a la UMV.");
		cerrarCPU(log);
	}
	if (!(socketKernel = conectarCliente(IP_KERNEL, PUERTO_PCP, log))) {
		log_error(log, "No se pudo conectar al Kernel.");
		cerrarCPU(log);
	}
	char *saludo = "CPU";
	char* respuestaUMV = malloc(8);
	send(socketUMV, saludo, 4, 0);
	recv(socketUMV, respuestaUMV, 4, MSG_WAITALL);

	if (!strncmp(respuestaUMV, "UMV", 3)) {
		log_info(log, "El handshake con la UMV salió bien.");
	} else {
		log_error(log, "El handshake con la UMV salió mal.");
	}
	free(respuestaUMV);

	int* header = malloc(4);
	char* saludoKernel = malloc(7);

	send(socketKernel, saludo, 4, 0); //TODO el saludo tiene tamaño 4?
	recv(socketKernel, (void *) header, 4, MSG_WAITALL);
	char *respuestaKernel = malloc(*header);
	recv(socketKernel, respuestaKernel, *header, MSG_WAITALL);
	desempaquetar2(respuestaKernel, saludoKernel, &quantumDeKernel, &retardo,
			0);
	if (!strncmp(saludoKernel, "Kernel", 6)) {
		log_info(log, "El handshake con el Kernel salió bien.");
	} else {
		log_error(log, "El handshake con el Kernel salió mal.");
	}

	diccionarioDeVariables = dictionary_create();

	//signal(SIGINT, manejar_seniales);
	signal(SIGUSR1, manejar_seniales);

	while (sigusr1_activado == 0) {
		//Recibo un PCB
		char* pcbEmpaquetado = malloc(sizeof(t_PCB));
		t_PCB *pcb;
		recv(socketKernel, pcbEmpaquetado, *pcbEmpaquetado, MSG_WAITALL);
		desempaquetar2(pcbEmpaquetado, &pcb, 0);

		etiquetas = solicitarBytesAUMV(*(pcb->indice_de_Etiquetas), 0,
				pcb->tamanio_Indice_de_Etiquetas);
		int lineasAnalizadas = 0;
		int programaBloqueado = 1;
		int ubInstruccion, largoInstruccion;
		while (lineasAnalizadas < quantumDeKernel || programaBloqueado) {
			//Si empieza en la instruccion 0 deberia ser progra_Counter-1 ???
			char *msjInstruccion = solicitarBytesAUMV(*(pcb->indice_de_Codigo),
					pcb->program_Counter * 8, 8);
			desempaquetar2(msjInstruccion, &ubInstruccion, &largoInstruccion,
					0);
			char *literalInstruccion = solicitarBytesAUMV(*(pcb->segmento_Codigo), ubInstruccion, largoInstruccion);
			analizarLinea(literalInstruccion, funcionesAnSISOP,
					funcionesKernel);
			pcb->program_Counter++;
			actualizar(pcb);
			lineasAnalizadas++;
		}
		t_paquete* paquete = serializarPCB(pcb);
		send(socketKernel, &paquete, sizeof(t_paquete), 0);
		//enviar de nuevo el pcb al kernel, con la razon en el header

	}
	return 0;
}

