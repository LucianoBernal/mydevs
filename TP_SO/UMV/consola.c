/*
 * consola.c
 *
 *  Created on: 03/05/2014
 *      Author: utnso
 */
/*
 * #include <stdio.h>
 #include <string.h>
 #include <stdlib.h>
 #include "funcionesaux.h"
 */
#include "consola.h"

int consola() {
	const int COMANDO_SIZE = 70; //TODO preguntar tamaño max de comando.
	char* comando = malloc(COMANDO_SIZE);
	do {
		gets(comando);
//		analizarYEjecutar(comando);

	} while (strncmp("exit", comando, 4));
	free(comando);
	return 0;
}
