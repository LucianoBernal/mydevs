/*
 * funcionesaux.c
 *
 *  Created on: 03/05/2014
 *      Author: utnso
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void leerComoOperacion(char*);
void leerComoRetardo(char*);
void leerComoAlgoritmo(char*);
void leerComoCompactacion(char*);
void leerComoDump(char*);
void obtenerParametroI(int, char**);
int interpretadoComoInt(char*);
void *interpretadoComoPuntero(char*);
void buscarEnMemoriaYEntregar(int, void*, int, int);
void escribirBufferEnMemoria(int, void*, int, int, char);

typedef struct{
	int proceso;
	void *base;
	int offset;
	int tamano;
	char accion;
	char archivo;
}t_operacion;

void analizarYEjecutar(char *comando){
	char* nomComando;
	if(!strcmp(strncpy(nomComando,comando,10), "operacion ")){
		leerComoOperacion(comando);
	}
	if(!strcmp(strncpy(nomComando,comando,8), "retardo ")){
		leerComoRetardo(comando);
	}
	if(!strcmp(strncpy(nomComando,comando,10), "algoritmo ")){
		leerComoAlgoritmo(comando);
	}
	if(!strcmp(strncpy(nomComando,comando,13), "compactacion ")){
		leerComoCompactacion(comando);
	}
	if(!strcmp(strncpy(nomComando,comando,5), "dump ")){
		leerComoDump(comando);
	}
}

void leerComoOperacion(char*){
	char* parametro[6];
	int i;
	t_operacion operacion;
	for(i=0;i<6;i++){
		obtenerParametroI(i, &parametro[i]);//busca espacios en la gran cadena para separar los parámetros y los guarda en vector
	}
	operacion.proceso=interpretadoComoInt(parametro[0]);
	operacion.base=interpretadoComoPuntero(parametro[1]);
	operacion.offset=interpretadoComoInt(parametro[2]);
	operacion.tamano=interpretadoComoInt(parametro[3]);
	operacion.accion=(char)parametro[4];
	operacion.archivo=(char)parametro[5];

	if(operacion.accion=='s')
	{
		buscarEnMemoriaYEntregar(operacion.proceso, operacion.base, operacion.offset, operacion.tamano);
	}
	else if (operacion.accion=='e')
	{
		escribirBufferEnMemoria(operacion.proceso, operacion.base, operacion.offset, operacion.tamano,operacion.archivo);
	}
}

