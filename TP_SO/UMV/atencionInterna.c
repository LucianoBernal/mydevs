/*
 * atencionInterna.c
 *
 *  Created on: 11/06/2014
 *      Author: utnso
 */
#include "atencionInterna.h"
#include "atencioninterna_interfaz.h"
#include <biblioteca_comun/Serializacion.h>
#include <biblioteca_comun/definiciones.h>

pthread_mutex_t mutexOperacion = PTHREAD_MUTEX_INITIALIZER;
//typedef enum {
//	Kernel = 1, CPU = 2
//} saludos_internos;

void* atencionInterna(void* sinParametro) {
	char* saludoKernel = malloc(7);
	char* saludoCpu = malloc(4);

	int socket = crearServidor(puertoUMV, logger);
	log_debug(logger, "Se creó servidor para atención interna");

	while (1) {
		int* socketKernel=malloc(sizeof(int));
		*socketKernel = aceptarConexion(socket, logger);
		recv(*socketKernel, (void*) saludoKernel, 30, 0);
		log_info(logger, "Se intenta conectar un posible Kernel. Comprobando...");

		if (!strncmp(saludoKernel, "Kernel", 6)) {
			log_info(logger, "El handshake con el Kernel salió bien.");
			send(*socketKernel, "UMV", 4, 0);
			//free(saludoKernel);FIXME va acá?
			//Creo hilo de atención al kernel.
			if (pthread_create(&hiloKernel, NULL, (void *) atencionKernel,
					(void*) socketKernel)) {
				log_error(logger,
						"El hilo del Kernel no se creó correctamente.");
			} else {
				log_debug(logger, "El hilo del Kernel se creó correctamente.");
			}
			break;
		} else {
			close(*socketKernel);
			free(socketKernel);
			log_error(logger, "No es un Kernel válido");
		}

	} //Cierra while de Kernel

	while (1) {
		int *socketCPU=malloc(sizeof(int));
		*socketCPU = aceptarConexion(socket, logger);
		recv(*socketCPU, (void*) saludoCpu, 30, 0);
		log_info(logger, "Se intenta conectar un posible CPU. Comprobando...");

		if (!strncmp(saludoCpu, "CPU", 3)) {
			log_info(logger, "El handshake con la CPU salió bien.");
			send(*socketCPU, "UMV", 4, 0);
			//Creo hilo de atención a CPU.
			if (pthread_create(&hiloCpu, NULL, (void *) atencionCpu,
					(void*) socketCPU)) {
				log_error(logger, "El hilo de CPU no se creó correctamente.");
			} else {
				log_info(logger, "El hilo de CPU se creó correctamente.");
				cantCpu++;
			}

//			//CODIGO PELIGROSO E
//			char buffer[1025];  //data buffer de 1K
//			//inicializo set
//			fd_set readfds;
//			struct sockaddr_in address;
//			int addrlen;
//			if (FD_ISSET( socketCPU , &readfds)) {
//				//Verifica si se cerro, y ademas lee el mensaje recibido
//				if ((valread = read(socketCPU, buffer, 1024)) == 0) {
//					//Algun programa se desconecto, obtengo la informacion
//					getpeername(socketCPU, (struct sockaddr*) &address,
//							(socklen_t*) &addrlen);
//					log_info(logger,"Una cpu se cerro: socket fd: %d , ip: %s , puerto: %d \n",socketCPU,
//							inet_ntoa(address.sin_addr),
//							ntohs(address.sin_port));
//					//Cierra el socket y marca 0 el bit de ocupado
//					close(socketCPU);
//				}
//			}
//			//CODIGO PELIGROSO T

		} else {
			close(*socketCPU);
			free(socketCPU);
			log_error(logger, "No es una CPU válida.");
		}
	}			//Cierra while de CPU

}			//Cierra atencionInterna

void atencionKernel(int* socketKernel) {
	char *header = malloc(16);
	int j = 0;
	int *razon = malloc(4);
	int *tamanoMensaje = malloc(4);
	int procesoActivo = 0, parametro[3], basesLogicas[4];
	log_info(logger, "Entró a atencioKernel.");
	int *pid = malloc(sizeof(int));
	while (1) {

		j++;
		char *mensaje = recibirConRazon(*socketKernel, razon, logger);
		if((mensaje==NULL)&&(*razon!=DESTRUIR_SEGMENTOS)&&(*razon!=CONFIRMACION)){
			log_error(logger,"Se desconecto Abruptamente el Kernel, sd:%d",*socketKernel);
			break;//el close lo hace recibirConRazon
		}
		//aplicarRetardo(retardo);
		switch (*razon) {

		case CONFIRMACION:
			pthread_mutex_lock(&mutexOperacion);
			enviarConRazon(*socketKernel,logger,CONFIRMACION, NULL);
			pthread_mutex_unlock(&mutexOperacion);
			break;
		case CREAR_SEGMENTOS_PROGRAMA:
			log_debug(logger, "Recibí crear segmentos programa, de parte del Kernel.");
			puts("recibi crear segmentos programa");
			pthread_mutex_lock(&mutexOperacion);
			int *tamano1 = malloc(sizeof(int)), *tamano2 = malloc(sizeof(int)),
					*tamano3 = malloc(sizeof(int)), *tamano4 = malloc(
							sizeof(int));
			desempaquetar2(mensaje, tamano1, tamano2, tamano3, tamano4, 0);
			int i;
			basesLogicas[0] = crearSegmento(*tamano1);
			basesLogicas[1] = crearSegmento(*tamano2);
			basesLogicas[2] = crearSegmento(*tamano3);
			basesLogicas[3] = crearSegmento(*tamano4);
			for (i = 0; i < 4; i++) {
				printf("la base logica %d es %d\n", i, basesLogicas[i]);
				if (basesLogicas[i] == -1) {
					destruirTodosLosSegmentos();
					*razon = MEMORY_OVERLOAD;
					*tamanoMensaje = 0;
					break;
				}
				*razon = BASES_LOGICAS;
				*tamanoMensaje = 32;
			}
			pthread_mutex_unlock(&mutexOperacion);
			enviarConRazon(*socketKernel, logger, *razon,
					serializar2(crear_nodoVar(&basesLogicas[0], 4),
							crear_nodoVar(&basesLogicas[1], 4),
							crear_nodoVar(&basesLogicas[2], 4),
							crear_nodoVar(&basesLogicas[3], 4), 0));
			log_debug(logger, "Terminé de crear segmentos programa, de parte del Kernel.");
			break;

		case DESTRUIR_SEGMENTOS:
			log_debug(logger, "Recibí destruir segmentos, de parte del Kernel.");
			pthread_mutex_lock(&mutexOperacion);
			cambiarProcesoActivo(procesoActivo);
			destruirTodosLosSegmentos();
			pthread_mutex_unlock(&mutexOperacion);
			log_debug(logger, "Terminé de destruir segmentos, de parte del Kernel.");
			break;

		case ESCRIBIR_EN_UMV:
			log_debug(logger, "Recibí escribir en umv, de parte del Kernel.");
			printf("recibi escribir en umv\n");
			pthread_mutex_lock(&mutexOperacion);
			cambiarProcesoActivo(procesoActivo);

			char *buffer = malloc(2048);
			desempaquetar2(mensaje, &parametro[0], &parametro[1], &parametro[2],
					buffer, 0);
			buffer=realloc(buffer, parametro[2]);
			printf("Los parametros que envio fueron:\n base= %d\n offset=%d\n tamano= %d\n buffer= %s\n", parametro[0], parametro[1], parametro[2], buffer);
			enviarUnosBytes(parametro[0], parametro[1], parametro[2], buffer);
			pthread_mutex_unlock(&mutexOperacion);
			free(buffer);
			log_debug(logger, "Terminé de escribir en umv, de parte del Kernel.");
			break;

		case CAMBIAR_PROCESO_ACTIVO:
			log_debug(logger, "Recibí cambiar proceso activo, de parte del Kernel.");
			printf("recibi cambiar proceso activo");
			pthread_mutex_lock(&mutexOperacion);
			desempaquetar2(mensaje, &procesoActivo, 0);
			printf("\nel valor del pid es: %d\n", procesoActivo);
			cambiarProcesoActivo(procesoActivo);
			pthread_mutex_unlock(&mutexOperacion);
			puts("termine de cambiar proceso activo");
			log_debug(logger, "Terminé de cambiar proceso activo, de parte del Kernel.");
			break;
		case CREAR_PROCESO_NUEVO:
			log_debug(logger, "Recibí crear proceso nuevo, de parte del Kernel.");
			puts("recibi crear proceso nuevo");
			pthread_mutex_lock(&mutexOperacion);
			printf("el pid que recibi es:%d\n", *pid);
			desempaquetar2(mensaje, pid, 0);
			printf("el pid que recibi es:%d\n", *pid);
			if (listaProcesos == NULL ) {
				printf("lista procesos era null");
				listaProcesos = list_create();
			}
			agregarProceso(*pid, 'c');
			pthread_mutex_unlock(&mutexOperacion);
			log_debug(logger, "Terminé de crear proceso nuevo, de parte del Kernel.");
			break;
		}

		free(mensaje);
	}
	free(header);
	free(tamanoMensaje);
	free(socketKernel);
}
void atencionCpu(int *socketCPU) {
	log_info(logger, "Entró a atencionCpu");

	int procesoActivo = 0, parametro[4];

	int *tamanoMensaje = malloc(4);
	int * razon = malloc(sizeof(int));

	while(1){
//	aplicarRetardo(retardo);
	char *mensaje=recibirConRazon(*socketCPU, razon, logger);
	if(mensaje==NULL){
			log_info(logger,"Se desconecto Abruptamente una CPU, sd%d",*socketCPU); //TODO hay que hacer algo??
				break;//el close al socket lo hace recibirConRazon
			}
	switch (*razon) {

	case SOLICITAR_A_UMV:
		log_debug(logger, "Recibí solicitar a umv, de la CPU: %d",*socketCPU);
		pthread_mutex_lock(&mutexOperacion);
		cambiarProcesoActivo(procesoActivo);
		desempaquetar2(mensaje, &parametro[0], &parametro[1], &parametro[2], 0);
		printf("\nRecibi solicitar bytes, base: %d, offset: %d, y tamano: %d\n", parametro[0], parametro[1], parametro[2]);
		char *respuesta = solicitarBytes(parametro[0], parametro[1],
				parametro[2]);
		printf("Y el resultado es buffer=%s", respuesta);
		pthread_mutex_unlock(&mutexOperacion);
		if (!strcmp(respuesta, "error")) {
//			*razon = SEGMENTATION_FAULT;
//			*tamanoMensaje = 0;
//			t_paquete *varALiberar = (t_paquete *) serializar2(
//					crear_nodoVar(razon, 4), crear_nodoVar(tamanoMensaje, 4),
//					0);
//			send(*socketCPU, varALiberar->msj, 16, 0);
//			free(varALiberar);
			enviarConRazon(*socketCPU, logger, SEGMENTATION_FAULT, NULL);
			break;
		}
		*razon = RESPUESTA_A_SOLICITAR_BUFFER;
		*tamanoMensaje = parametro[2];
		printf("Me solicitaron un buffer, base %d, offset %d, y tamano %d \n", parametro[0], parametro[1], parametro[2]);
		printf("Y la respuesta es %s\n", respuesta);
//		t_paquete *paquete = (t_paquete *) serializar2(
//				crear_nodoVar(respuesta, *tamanoMensaje), 0);
//		t_paquete *header = (t_paquete *) serializar2(crear_nodoVar(razon, 4),
//				crear_nodoVar(&paquete->tamano, 4), 0);
//		send(*socketCPU, header->msj, 16, 0);
//		send(*socketCPU, paquete->msj, paquete->tamano, 0);
		enviarConRazon(*socketCPU, logger, RESPUESTA_A_SOLICITAR_BUFFER, serializar2(
				crear_nodoVar(respuesta, *tamanoMensaje), 0));
		free(respuesta);
		log_debug(logger, "Terminé solicitar a umv, de la CPU: %d",*socketCPU);
		break;

	case ESCRIBIR_EN_UMV:
		log_debug(logger, "Recibí escribir en umv, de la CPU: %d",*socketCPU);
		pthread_mutex_lock(&mutexOperacion);
		cambiarProcesoActivo(procesoActivo);
		char *buffer = malloc(*tamanoMensaje+1);
		desempaquetar2(mensaje, &parametro[0], &parametro[1], &parametro[2],
				buffer, 0);
		buffer[parametro[2]]= 0;
		printf("\nRecibi escribir bytes, base: %d, offset: %d, tamano: %d y buffer: %s\n", parametro[0], parametro[1], parametro[2], buffer);
		int resultado = enviarUnosBytes(parametro[0], parametro[1], parametro[2], buffer);
		if (resultado){
			enviarConRazon(*socketCPU, logger, CONFIRMACION, NULL);
		}else{
			enviarConRazon(*socketCPU, logger, SEGMENTATION_FAULT, NULL);
		}
		pthread_mutex_unlock(&mutexOperacion);
		free(buffer);
		log_debug(logger, "Terminé escribir en umv, de la CPU: %d",*socketCPU);
		break;
	case CAMBIAR_PROCESO_ACTIVO:
		log_debug(logger, "Recibí cambiar proceso activo, de la CPU: %d",*socketCPU);
		printf("recibi cambiar proceso activo");
		pthread_mutex_lock(&mutexOperacion);
		int *pid2 = malloc(sizeof(int));
		desempaquetar2(mensaje, pid2, 0);
		procesoActivo = *pid2;
		printf("\nel valor del pid es: %d\n", *pid2);
		cambiarProcesoActivo(*pid2);
		free(pid2);
		pthread_mutex_unlock(&mutexOperacion);
		puts("termine de cambiar proceso activo");
//		free(pid2); //TODO lo puse arriba del mutex... no se en qué varía pero creo q es mas seguro asi.
		log_debug(logger, "Terminé cambiar proceso activo, de la CPU: %d",*socketCPU);
		break;
	}

	free(mensaje);
	}
	free(tamanoMensaje);
	free(razon);
	free(socketCPU);
}

