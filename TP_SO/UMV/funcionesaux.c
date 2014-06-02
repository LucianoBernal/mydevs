/*
 * funcionesaux.c
 *
 *  Created on: 03/05/2014
 *      Author: utnso
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "funcionesaux.h"

typedef struct{
	int proceso;
	int base;
	int offset;
	int tamano;
	char accion; //Las acciones posibles son: Crear/Destruir segmentos y solicitar/enviar bytes
	char *archivo;
}t_operacion;

void analizarYEjecutar(char *comando){
	if(!strncmp("operacion",comando,9)){
		leerComoOperacion(comando);
	}
	else if(!strncmp("retardo",comando,7)){
		leerComoRetardo(comando);
	}
	else if(!strncmp("algoritmo",comando,9)){
		leerComoAlgoritmo(comando);
	}
	else if(!strncmp("compactacion",comando,12)){
		leerComoCompactacion(comando);
	}
	else if(!strncmp("dump",comando,4)){
		leerComoDump(comando);
	}
	else if(!strncmp("exit",comando,4)){
		return;
	}
	else {
		printf("Error al ingresar el comando. Verifique sintaxis.\n");
		return;
	}
}

/*//Empieza- funciones usadas para obtener y guardar parámetros de un comando
int buscarEspacio(int numero, char *cadena){
	if (numero==0) return 0;
	int espacios=0;
	int i=1;
 	while((espacios<numero)&&(i<strlen(cadena))){
		if (cadena[i]==' ') espacios+=1;
		i+=1;
	};
 	if (i==strlen(cadena)&&(cadena[i-1]!=' ')) i+=1;
	return i;
}

void obtenerParametroI(int numero, char* destino, char* origen){
	int primerEspacio;
	int segundoEspacio;
	int i;
	primerEspacio=buscarEspacio(numero-1, origen);
	segundoEspacio=buscarEspacio(numero, origen);
	for (i=primerEspacio;i<(segundoEspacio-1);i++){
		destino[i-primerEspacio]=origen[i];
	}
	destino[i-primerEspacio]=0;
	return;
}
//Termina- funciones usadas para obtener y guardar parámetros de un comando */

void leerComoOperacion(char *comando){
/*	const int PARAM_SIZE = 20;
	char parametro[6][PARAM_SIZE];
	int i;
	t_operacion operacion;
	char mensaje[48];
	for(i=0;i<8;i++){
		obtenerParametroI(i+1, parametro[i], comando);//busca espacios en la gran cadena para separar los parámetros y los guarda en vector
	}

	operacion.proceso=atoi(parametro[1]);
	operacion.base=atoi(parametro[2]);
	operacion.offset=atoi(parametro[3]);
	operacion.tamano=atoi(parametro[4]);
	operacion.accion=(char)parametro[5];
	operacion.archivo=(char)*parametro[6];

	switch (operacion.accion){
		case 'e':
			printf("que queres mandar? tipea pls:");
			scanf("%s", mensaje);
			enviarUnosBytes(operacion.proceso, operacion.base, operacion.offset, operacion.tamano, mensaje);
			//averiguar que mostrar por pantalla
			break;
		case 's':
			printf("%s", solicitarBytes(operacion.proceso, operacion.base, operacion.offset, operacion.tamano));
			break;
		case 'c':
			if (crearSegmento(operacion.proceso)){
				printf("Creeme que el segmento se creo correctamente");
			}else {
				printf("Hubo un error, anda a saber cual");
			}
			break;
		case 'd':
			if (destruirSegmento(operacion.proceso)){
			printf("Creeme que el segmento se creo correctamente");
		}else {
			printf("Hubo un error, anda a saber cual");
		}
			break;
		default:
			//lo mandamos al log
			break;
}*/
	t_operacion operacion;
	printf("decime que queres hacer:\n");
	scanf("%d",operacion.proceso);

}

void enviarUnosBytes(int, pid, int base, int offset, int tamano, void* mensaje){
	memcpy(obtenerDirFisica(base, offset, pid), mensaje, tamano);
}

char *solicitarBytes(int pid, int base, int offset, int tamano){
	char *aux;
	memcpy(aux, obtenerDirFisica(base, offset, pid), tamano);
	*(aux+tamano+1)=0;
	return aux;
}

typedef struct {
	int identificador;
	int inicioLogico;
	int tamano;
	void* memPpal;
}t_tablaSegmento;

typedef struct{
	int pid;
	char *tipo;
	t_tablaSegmento *tabla;
	int activo=0;
}t_tablaProceso;

t_tablaProceso vectorProcesos[];
int cantProcesos;

void crearSegmento(int pid, int tamano){

	t_tablaSegmento auxTablaSegmento;
	auxTablaSegmento.identificador = crearIdentificador();
	auxTablaSegmento.inicioLogico = obtenerInicioLogico(pid);
	auxTablaSegmento.tamano = tamano;
	auxTablaSegmento.memPpal = mallocConAlgoritmo(tamano);
	list_add(vectorProcesos[buscarPid(pid)].tabla, auxTablaSegmento);
}

void destruirSegmento(int pid){
	list_destroy_and_destroy_element(vectorProcesos[buscarPid(pid)].tabla, element_destroyer);
}
//element_destroyer entendemos que es una función que permite destruir algo... no sabemos si la definimos nosotros o qionda :P

void* element_destroyer (void* p){
	free(p);
	return p;
}

int buscarPid(int pid){
	for(int i = 0; i < cantProcesos; i++){
		if (vectorProcesos[i].pid==pid) return i;
	}
	printf("algo malo paso");
	return 0;
}

void leerComoRetardo(char *comando){
}

void leerComoAlgoritmo(char *comando){
}

void leerComoCompactacion(char *comando){
}

void leerComoDump(char *comando){
}

