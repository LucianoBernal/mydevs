/*
 * consola.c
 *
 *  Created on: 03/05/2014
 *      Author: utnso
 */
#include "consola.h"
#include "consola_interfaz.h"
void crearProcesoArtificial() {
	listaProcesos = list_create();
	agregarProceso(10000, 'c');
	cambiarProcesoActivo(10000);
}
void* consola(void* baseUMV) {
	const int COMANDO_SIZE = 30;
	char* comando = malloc(COMANDO_SIZE);
	crearProcesoArtificial();
	do {
		printf("\nIngrese comando: \n");
		scanf("%s", comando);
		analizarYEjecutar(comando);

	} while (strncmp("exit", comando, 4));
	log_info(logger, "Se cerró la consola");
	free(comando);
	return NULL ;
}

void analizarYEjecutar(char *comando) {

	if (!strncmp("operacion", comando, 9)) {
		t_operacion operacion;

		printf(
				"¿Qué operación desea ralizar?-para saber como usarlo enviar 'h'. \n");
		int flag;
		do {
			flag = 0;
			scanf("%s", &operacion.accion);

			switch (operacion.accion) {

			case 'h':
				printf(
						"Para escribir bytes en memoria, ingrese 'e' \nPara solicitar bytes en memoria, ingrese 's' \nPara crear un segmento, ingrese 'c' \nPara destruir un segmento, ingrese 'd' \n");
				flag = 1;
				break;

			case 'e':
				printf("\ningrese base: ");
				scanf("%d", &operacion.base);

				printf("\ningrese offset: ");
				scanf("%d", &operacion.offset);

				printf("\ningrese tamaño: ");
				scanf("%d", &operacion.tamano);

				char* msj = malloc(operacion.tamano);
				printf("\n Ingrese bloque de mensaje: \n");
				scanf("%s", msj);
				enviarUnosBytes(operacion.base, operacion.offset,
						operacion.tamano, msj);
				free(msj);
				break;

			case 's':

				printf("\ningrese base: ");
				scanf("%d", &operacion.base);

				printf("\ningrese offset: ");
				scanf("%d", &operacion.offset);

				printf("\ningrese tamaño: ");
				scanf("%d", &operacion.tamano);
				int i;
				//char *respuesta=malloc(operacion.tamano);
				char *respuesta = solicitarBytes(operacion.base,
						operacion.offset, operacion.tamano);
				//printf("\n%d\n", strlen(respuesta));
				if (strcmp(respuesta, "")) {
					for (i = 0; i < operacion.tamano; i++) {
						//para darme cuenta si no imprime o imprime un /0 TODO borrar!!
						printf("%c", respuesta[i] == 0 ? '0' : respuesta[i]);
					}
					free(respuesta);
				}
				printf("\n");
				break;

			case 'c':
				printf("\ningrese tamaño: ");
				scanf("%d", &operacion.tamano);

				printf("La base logica del segmento creado es: %d\n",
						crearSegmento(operacion.tamano));

				break;

			case 'd':
				printf("\ningrese base: ");
				scanf("%d", &operacion.base);

				destruirSegmento(operacion.base);

				break;

			default:
				printf("No ingresó ninguna opción válida.");
				flag = 1;
			}		//Termina Switch para ingreso de parámetros de operacion.
		} while (flag);
	}			//Termina if de operacion.

	else if (!strncmp("retardo", comando, 7)) {
		int ret;
		printf("\n ingrese el retardo: ");
		scanf("%d", &ret);
		retardo = ret;

	}			//Termina if de retardo.

	else if (!strncmp("algoritmo", comando, 9)) {
		char alg;
		printf(
				"Si quiere cambiar a Worst-Fit, ingrese w\n Si quiere cambiar a First-Fit, ingrese f:\n");
		fflush(stdin); //FIXME
		scanf("%s", &alg);
		if (algoritmo) {
			switch (alg) {
			case 'w':
				algoritmo = 1;
				break;
			case 'f':
				algoritmo = 0;
				printf("Ya se encontraba en modo First-Fit.\n");
				break;
			default:
				printf("error al ingresar algoritmo, es f o w.\n");
				break;
			}
		} else {
			switch (alg) {
			case 'w':
				algoritmo = 1;
				printf("Ya se encontraba en modo Worst-Fit.\n");
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
		compactarMemoria();
		printf("Compactación de memoria finalizada correctamente. \n");

	}	//Termina if de compactación.

	else if (!strncmp("dump", comando, 4)) {
		int pidPedido;
		int off;
		int tam;
		int confirmaMemo;
		bool arch = 0;	//TODO: archivo = log ?

		printf("Estructuras de memoria.\n");
		printf(
				"Si quiere las tablas de segmentos de un proceso ingrese su pid, sino ingrese -1: \n");
		scanf("%d", &pidPedido);
		dumpTablaSegmentos(arch, pidPedido);

		printf("\nMemoria principal:\n");
		dumpMemoriaLibreYSegmentos(arch);

		printf("\nSi desea saber el contenido de la memoria principal, ingrese '1', sino, '0'. \n");
		scanf("%d", &confirmaMemo);
		if(confirmaMemo){
		printf("\n Contenido de memoria principal.\n");
		printf("\nIngrese offset: ");
		scanf("%d", &off);
		printf("\nIngrese tamaño: ");
		scanf("%d", &tam);
		dumpMemoriaChata(off, tam, arch);
		}

	}	//Termina if de dump.

	else if (!strncmp("exit", comando, 4)) {
		return;
	} else {
		printf("Error al ingresar el comando. Verifique sintaxis.\n");
		return;
	}
}

