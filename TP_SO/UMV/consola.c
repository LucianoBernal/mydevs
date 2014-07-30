/*
 * consola.c
 *
 *  Created on: 03/05/2014
 *      Author: utnso
 */
#include "consola.h"
#include "consola_interfaz.h"

bool validarPid(int pid) {
	t_tablaProceso *pidFound = (list_get(listaProcesos, buscarPid(pid)));
	if (pidFound == NULL ) {
		printf("El pid no fue encontrado\n");
		return 0;
	}
	if (pidFound->pid != pid) {/*Es muy raro que llegue a este error pero por las dudas lo valido.*/
		printf("Error al encontrar el pid. Intente nuevamente.");
		return 0;
	}
	return 1;
}

void crearProcesoArtificial() {
	listaProcesos = list_create();
	agregarProceso(10000, 'c');
	cambiarProcesoActivo(10000);
}
void* consola(void* baseUMV) {
	const int COMANDO_SIZE = 30;
	char* comando = malloc(COMANDO_SIZE);
	crearProcesoArtificial();

	dumpFile = fopen("archivoDeDump.txt", "w+");
	if (dumpFile == NULL ) {
		fputs("File error", stderr);
		exit(1);
	} else {
		log_info(logger, "Se creó el archivo de dump.");
	}

	do {
		printf("\nIngrese comando: \n");
		fflush(stdin);
		scanf("%s", comando);
		analizarYEjecutar(comando);

	} while (strncmp("exit", comando, 4));
	log_info(logger, "Se cerró la consola");
	fclose(dumpFile);
	free(comando);
	return NULL ;
}

void analizarYEjecutar(char *comando) {

	if (!strncmp("operacion", comando, 9)) {
		t_operacion operacion;

//		printf(
//				"¿Qué operación desea ralizar?-para saber como usarlo enviar 'h'. \n");
		int flag;
		do {
			printf(
					"¿Qué operación desea ralizar?-para saber como usarlo enviar 'h'. \n");
			flag = 0;
			int processID;
			fflush(stdin);
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
				fflush(stdin);
				scanf("%s", msj);
				pthread_mutex_lock(&mutexOperacion);
				//cambiarProcesoActivo(10000);
				enviarUnosBytes(operacion.base, operacion.offset,
						operacion.tamano, msj);
				pthread_mutex_unlock(&mutexOperacion);
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
				pthread_mutex_lock(&mutexOperacion);
				//cambiarProcesoActivo(10000); //TODO me parece que va eh...
				char *respuesta = solicitarBytes(operacion.base,
						operacion.offset, operacion.tamano);
				pthread_mutex_unlock(&mutexOperacion);
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
				printf("\ningrese pid: ");
				fflush(stdin);
				scanf("%d", &processID);
				if (!validarPid(processID)) {
					break;
				}

				printf("\ningrese tamaño: ");
				fflush(stdin);
				scanf("%d", &operacion.tamano);

				pthread_mutex_lock(&mutexOperacion);
				cambiarProcesoActivo(processID); //todo ojo fijarse!!
				printf("La base logica del segmento creado es: %d\n",
						crearSegmento(operacion.tamano));
				pthread_mutex_unlock(&mutexOperacion);

				break;

			case 'd':
				printf("\ningrese pid: ");
				fflush(stdin);
				scanf("%d", &processID);
				if (!validarPid(processID)) {
					break;
				}

				printf("\ningrese base: ");
				fflush(stdin);
				scanf("%d", &operacion.base);

				pthread_mutex_lock(&mutexOperacion);
				cambiarProcesoActivo(processID);
				destruirSegmento(operacion.base);
				pthread_mutex_unlock(&mutexOperacion);
				break;

			default:
				printf("No ingresó ninguna opción válida.\n");
				flag = 1;
			}		//Termina Switch para ingreso de parámetros de operacion.
		} while (flag);
	}			//Termina if de operacion.

	else if (!strncmp("retardo", comando, 7)) {
		int ret;
		printf("\n ingrese el retardo: ");
		fflush(stdin);
		scanf("%d", &ret);
		retardo = ret;

	}			//Termina if de retardo.

	else if (!strncmp("algoritmo", comando, 9)) {
		char alg;
		printf("El algoritmo actual es %s\n",
				!algoritmo ? "First fit" : "Worst fit");
		printf(
				"Si quiere cambiar a Worst-Fit, ingrese 'w'\nSi quiere cambiar a First-Fit, ingrese 'f':\n");
		fflush(stdin);
		scanf("%s", &alg);

		if (algoritmo) {
			switch (alg) {
			case 'w':
				printf("Ya se encontraba en modo Worst-Fit.\n");
				//algoritmo = 1;
				break;
			case 'f':
				cambiarAlgoritmo();
				//algoritmo = 0;
				//printf("Ya se encontraba en modo First-Fit.\n");
				break;
			default:
				printf("Error al ingresar algoritmo, es 'f' o 'w'.\n");
				break;
			}
		} else {
			switch (alg) {
			case 'w':
				cambiarAlgoritmo();
				//algoritmo = 1;
				//printf("Ya se encontraba en modo Worst-Fit.\n");
				break;
			case 'f':
				printf("Ya se encontraba en modo First-Fit.\n");
				//algoritmo = 0;
				break;
			default:
				printf("Error al ingresar algoritmo, es 'f' o 'w'.\n");
				break;
			}

		}
	}	//Termina if de algoritmo.

	else if (!strncmp("compactacion", comando, 12)) {
		pthread_mutex_lock(&mutexOperacion);
		compactarMemoria();
		pthread_mutex_unlock(&mutexOperacion);
		printf("Compactación de memoria finalizada correctamente. \n");

	}	//Termina if de compactación.

	else if (!strncmp("dump", comando, 4)) {
		int pidPedido;
		int off;
		int tam;
		int confirmaMemo;
		bool arch = 0;

		printf("Estructuras de memoria.\n");
		fprintf(dumpFile, "Estructuras de memoria.\n");
		printf(
				"Si quiere las tablas de segmentos de un proceso ingrese su pid, sino ingrese -1: \n");
		scanf("%d", &pidPedido);
		dumpTablaSegmentos(arch, pidPedido);	//todo

		printf("\nMemoria principal:\n");
		fprintf(dumpFile, "\nMemoria principal:\n");
		dumpMemoriaLibreYSegmentos(arch);

		printf(
				"\nSi desea saber el contenido de la memoria principal, ingrese '1', sino, '0'. \n");
		scanf("%d", &confirmaMemo);
		if (confirmaMemo) {
			printf("\n Contenido de memoria principal.\n");
			fprintf(dumpFile, "\n Contenido de memoria principal.\n");
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

