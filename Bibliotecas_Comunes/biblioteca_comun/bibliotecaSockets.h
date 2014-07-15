/*
 * bibliotecaSockets.h
 *
 *  Created on: 15/07/2014
 *      Author: utnso
 */

#ifndef BIBLIOTECASOCKETS_H_
#define BIBLIOTECASOCKETS_H_

int crearServidor(char* puerto, t_log* logs);
int aceptarConexion(int socket, t_log* logs);
int conectarCliente (char* ip, char* puerto, int logs);

#endif /* BIBLIOTECASOCKETS_H_ */
