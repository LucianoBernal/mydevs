/*
 * consola.c
 *
 *  Created on: 03/05/2014
 *      Author: utnso
 */
#include "consola.h"

int consola() {
	const int COMANDO_SIZE = 30;
	char* comando = malloc(COMANDO_SIZE);
	do {
		gets(comando);
		analizarYEjecutar(comando);

	} while (strncmp("exit", comando, 4));
	free(comando);
	return 0;
}

void analizarYEjecutar(char *comando) {

	if (!strncmp("operacion", comando, 9)) {
		t_operacion operacion;
		printf(
				"Qué operación desea ralizar?-para saber como usarlo enviar 'h'- \n");
		scanf("%c", &operacion.accion);
		switch (operacion.accion) {

		case 'h':
			printf(
					"para escribir bytes en memoria, ingrese 'e' \n para solicitar bytes en memoria, ingrese 's' \n para crear un segmento, ingrese 'c' \n para destruir un segmento, ingrese 'd' \n");

			break;

		case 'e':
			//printf("ingrese pid: ");
			//scanf("%d", &operacion.proceso);

			printf("\ningrese base: ");
			scanf("%d", &operacion.base);

			printf("\ningrese offset: ");
			scanf("%d", &operacion.offset);

			printf("\ningrese tamaño: ");
			scanf("%d", &operacion.tamano);

			char mensaje[operacion.tamano];
			printf("\n Ingrese bloque de mensaje: \n");
			scanf("%s", mensaje);

			enviarUnosBytes(operacion.base, operacion.offset, operacion.tamano,
					mensaje);

			break;

		case 's':
			printf("\ningrese base: ");
			scanf("%d", &operacion.base);

			printf("\ningrese offset: ");
			scanf("%d", &operacion.offset);

			printf("\ningrese tamaño: ");
			scanf("%d", &operacion.tamano);

			char mensaje[operacion.tamano];
			mensaje = solicitarBytes(operacion.base, operacion.offset,
					operacion.tamano);
			printf("%s", mensaje);

			break;

		case 'c':
			printf("\ningrese tamaño: ");
			scanf("%d", &operacion.tamano);

			crearSegmento(operacion.tamano);

			break;

		case 'd':
			printf("\ningrese base: ");
			scanf("%d", &operacion.base);

			destruirSegmento(operacion.base);

			break;

		default:
			printf("No ingresó ninguna opción válida.");
		}			//Termina Switch para ingreso de parámetros de operacion.

	}			//Termina if de operacion.

	else if (!strncmp("retardo", comando, 7)) {
		int ret;
		printf("\n ingrese el retardo: ");
		scanf("%d", ret);
		retardo = ret;

	}			//Termina if de retardo.

	else if (!strncmp("algoritmo", comando, 9)) {
		char alg;
		printf(
				"Si quiere cambiar a Worst-Fit, ingrese w\n Si quiere cambiar a First-Fit, ingrese f:\n");
		scanf("%c", alg);
		if (algoritmo) {
			switch (alg) {
			case 'w':
				algoritmo = 1;
				break;
			case 'f':
				algoritmo = 0;
				printf("Ya se encontraba en modo First-Fit.");
				break;
			default:
				printf("error al ingresar algoritmo, es f o w.\n");
				break;
			}
		} else {
			switch (alg) {
			case 'w':
				algoritmo = 1;
				printf("Ya se encontraba en modo Worst-Fit.");
				break;
			case 'f':
				algoritmo = 0;
				break;
			default:
				printf("error al ingresar algoritmo, es f o w.\n");
				break;
			}

		}
	}	//Termina if de algoritmo.

	else if (!strncmp("compactacion", comando, 12)) {
		copactarMemoria();
		printf("Compactación de memoria finalizad correctamente.");

	}	//Termina if de compactación.

	else if (!strncmp("dump", comando, 4)) {

	}	//Termina if de dump.
	else if (!strncmp("exit", comando, 4)) {
		return;
	} else {
		printf("Error al ingresar el comando. Verifique sintaxis.\n");
		return;
	}
}
