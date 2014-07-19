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

//void procesarPCB(t_PCB* pcb)
//{
//		if(pcb->tamanio_Contexto_Actual > 0){
//			log_info(log, "Hay %d variables. Regenero el diccionario del PCB %d", pcb->tamanio_Contexto_Actual, pcb->program_id);
//			regenerar_diccionario();
//		}
//
//		int quantum_ejecutado;
//
//		for( quantum_ejecutado = 0; quantum_ejecutado < quantumDeKernel; quantum_ejecutado++ ){
//			if( proceso_bloqueado == 0 && proceso_terminado == 0)
//			{
//				ejecutar_linea(pcb);
//			} else {
//				break;
//			}
//		}
//
//		if(proceso_terminado == 1){
//			if(1 /*send(socketKernel, ) > 0 */ /*ENVIO EL PCB CON SALIDA_NORMAL EN EL HEADER */){
//				log_info(log, "Se envia a kernel el proceso terminado. PCB %d", pcb->program_id);
//			} else {
//				log_error(log, "No se pudo enviar a kernel el proceso terminado. PCB %d", pcb->program_id);
//			}
//		} else if(quantum_ejecutado == quantumDeKernel){
//			if( 1/*send(socketKernel, */ /* ENVIO EL PCB CON SALIDA_POR_QUANTUM EN EL HEADER)*/ > 0 ){
//				log_info(log, "Se envia a kernel la finalizacion del quantum del proceso. PCB %d", pcb->program_id);
//			} else {
//				log_error(log, "No se pudo enviar a kernel la finalizacion del quantum del proceso. PCB %d", pcb->program_id);
//			}
//		}
//	}
//
//void ejecutar_linea(t_PCB* pcb){
//	log_info(log, "Mando a ejecutar una linea. PCB %d", pcb->program_id);
//	t_socket_paquete *paquete = (t_socket_paquete *) malloc(sizeof(t_socket_paquete));
////
////	t_solicitar_bytes * solicitud_valores_linea_para_ejecutar = malloc(100);
////
////	solicitud_valores_linea_para_ejecutar->base    = pcb->indice_codigo;
////	solicitud_valores_linea_para_ejecutar->offset  = pcb->program_counter * 8;
////	solicitud_valores_linea_para_ejecutar->tamanio = 8;
////	solicitud_valores_linea_para_ejecutar->program_id	   = pcb->program_id;
//
//	if( send(socketUMV, 63, sizeof(t_solicitar_bytes), solicitud_valores_linea_para_ejecutar) > 0 ){ //SOLICITAR_BYTES
//		log_info(log, "Se envia la solicitud de cursor stack a UMV en ejecutar_una_linea. PCB %d", pcb->program_id);
//	} else {
//		log_error(log, "No se pudo enviar la solicitud de cursor stack a UMV en ejecutar_una_linea. PCB %d", pcb->program_id);
//	}
//
//	//Recibo los valores de la instruccion de UMV
//	if (socket_recvPaquete(socketDelUMV, paquete) >= 0) {
//		if(paquete->header.type == 63){
//			log_info(log, "Datos recibidos de la UMV (valores de la instruccion). PCB %d", pcb->program_id);
//
//			t_atributos_instr * atributos;
//			atributos = (t_atributos_instr* ) paquete->data;
//
//			/* YA TENIENDO EL DESPLAZAMIENTO Y EL PESO, program_idO LOS VALORES DEL CODIGO A LA UMV */
//			t_solicitar_bytes * solicitud_linea_a_ejecutar = malloc(100);
//
//			solicitud_linea_a_ejecutar->base    = pcb->segmento_codigo;
//			solicitud_linea_a_ejecutar->offset  = atributos->offset;
//			solicitud_linea_a_ejecutar->tamanio = atributos->tamanio;
//			solicitud_linea_a_ejecutar->program_id = pcb->program_id;
//
//			//Elimino el paquete anterior y "creo" uno nuevo
//			free(paquete);
//
//			t_socket_paquete *paquete = (t_socket_paquete *) malloc(sizeof(t_socket_paquete));
//
//			//Envio la solicitud
//			if( socket_sendPaquete(socketDelUMV, 63, sizeof(t_solicitar_bytes), solicitud_linea_a_ejecutar) > 0 ){ //SOLICITAR_BYTES
//				log_info(log, "Se envia la solicitud de codigo a UMV. PCB %d", pcb->program_id);
//			} else {
//				log_error(log, "No se pudo enviar la solicitud de codigo a UMV. PCB %d", pcb->program_id);
//			}
//
//			//Recibo la instruccion de UMV
//			if (socket_recvPaquete(socketDelUMV, paquete) >= 0) {
//				log_info(log, "Datos recibidos de la UMV (codigo). PCB %d", pcb->program_id);
//				//log_info(log, "Codigo recibido: %s", paquete->data);
//			} else {
//				log_error(log, "Error al recibir los datos del UMV! PCB %d", pcb->program_id);
//			}
//
//			char * linea = (char *) paquete->data;
//			char * instruccion = malloc(100);
//			int inicio_instruccion = 0, pos_en_instruccion = 0, cantidad_letras_instruccion = 0;
//
//			while(1){
//				if(linea[inicio_instruccion] == '\t' || linea[inicio_instruccion] == '\0'){
//					inicio_instruccion++;
//					break;
//				} else {
//					break;
//				}
//			}
//
//			pos_en_instruccion = inicio_instruccion;
//
//			while(1){
//				if(linea[pos_en_instruccion] == '\n'){
//					break;
//				} else {
//					cantidad_letras_instruccion++;
//					break;
//				}
//
//				pos_en_instruccion++;
//			}
//
//			memcpy(&instruccion[0], &linea[inicio_instruccion], cantidad_letras_instruccion);
//			instruccion[cantidad_letras_instruccion] = '\0';
//
//			log_info(log, "Linea a ejecutar: %s. PCB %d", instruccion, pcb->program_id);
//
//			usleep(retardo);
//			analizadorLinea(strdup(instruccion), &functions, &kernel_functions);
//			log_info(log, "Termino de ejecutarse la linea: %s. PCB %d", instruccion, pcb->program_id);
//
//			free(instruccion);
//
//			//Actualizo el program counter
//			pcb->program_Counter++;
//		} else {
//			log_error(log, "Se recibio un codigo inesperado de UMV en ejecutar_una_linea: %d. PCB %d", paquete->header.type, pcb->program_id);
//		}
//	} else {
//		log_error(log, "Error al recibir los datos del UMV en ejecutar_una_linea. PCB %d", pcb->program_id);
//	}
//}
//
//
//void regenerar_diccionario(){
//	t_socket_paquete *paquete = (t_socket_paquete *) malloc(sizeof(t_socket_paquete));
//
//	/* Solicitar las variables del contexto actual a la UMV */
//	char * variables_contexto_actual = malloc(200);
//	t_solicitar_bytes * solicitud_variables_contexto = malloc(100);
//
//	solicitud_variables_contexto->base    = pcb->segmento_stack;
//	solicitud_variables_contexto->offset  = pcb->cursor_stack - pcb->segmento_stack;
//	solicitud_variables_contexto->tamanio = pcb->tamanio_contexto * 5;
//
//	//Envio la solicitud
//	if( socket_sendPaquete(socketDelUMV, 63, sizeof(t_solicitar_bytes), solicitud_variables_contexto) > 0 ){ //SOLICITAR_BYTES
//		log_info(log, "Se envia la solicitud de variables de contexto al regenerar diccionario. PCB %d", pcb->program_id);
//	} else {
//		log_error(log, "No se pudo enviar la solicitud de variables de contexto al regenerar diccionario. PCB %d", pcb->program_id);
//	}
//
//	free(solicitud_variables_contexto);
//
//	//Recibo la parte del stack del contexto actual de UMV
//	if (socket_recvPaquete(socketDelUMV, paquete) >= 0) {
//		if(paquete->header.type == 68){
//			log_info(log, "Recibo las variables para regenerar el diccionario. PCB %d", pcb->program_id);
//
//			//Guardo en el pcb el program counter recuperado del stack
//			variables_contexto_actual = paquete->data;
//		} else {
//			log_error(log, "Se recibio un codigo inesperado de UMV en regenerar_diccionario: %d. PCB %d", paquete->header.type, pcb->program_id);
//		}
//	} else {
//		log_error(log, "Error al recibir los datos del UMV. PCB %d", pcb->program_id);
//	}
//
//	/* Recorro las variables obtenidas del stack para regenerar el diccionario */
//	int i;
//
//	log_info(log, "Comienzo a regenerar el diccionario");
//
//	for( i = 0; i < pcb->tamanio_contexto; i++ ){
//		t_variable_diccionario * variable_diccionario = (t_variable_diccionario *)malloc(sizeof(t_variable_diccionario));
//		variable_diccionario->identificador = variables_contexto_actual[i * 5];
//
//		//Esta es la direccion del identificador en el stack
//		variable_diccionario->direccion = pcb->cursor_stack + i * 5;
//
//		variable_diccionario->valor = variables_contexto_actual[i * 5 + 1];
//
//		list_add(variables_diccionario, variable_diccionario);
//
//		log_info(log, "Variable %c agregada al diccionario con valor %d con diccionario. PCB %d", variable_diccionario->identificador, variable_diccionario->valor, pcb->program_id);
//	}
//
//	log_info(log, "El diccionario se termino de regenerar. PCB %d", pcb->program_id);
//
//	free(variables_contexto_actual);
//}
//
//	void manejar_seniales(int n) {
//		switch (n) {
//		case SIGINT:
//			printf("Llego SIGINT\n");
//			break;
//
//		case SIGUSR1:
//			sigusr1_activado = 1;
//			printf("Llego SIGUSR1\n");
//			break;
//		}
//	}
//
//	return 0;
//}
