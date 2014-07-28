/*
 * bibliotecaSockets.h
 *
 *  Created on: 15/07/2014
 *      Author: utnso
 */

#ifndef BIBLIOTECASOCKETS_H_
#define BIBLIOTECASOCKETS_H_

#include <commons/log.h>
#include "Serializacion.h"

int crearServidor(char* puerto, t_log* logs);
int aceptarConexion(int socket, t_log* logs);
int conectarCliente (char* ip, char* puerto, t_log* logs);
int enviarConRazon(int socket, t_log* logs, int razon, t_paquete *pack);
char *recibirConRazon(int socket, int *p_razon, t_log *logs);

#endif /* BIBLIOTECASOCKETS_H_ */
