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
	char accion;
	char archivo;
}t_operacion;

void analizarYEjecutar(char *comando){
	//char* nomComando; //no lo usamos más...

	if(!strncmp("operacion ",comando,10)){
		leerComoOperacion(comando);
	}
	/*if(!strcmp(strncpy(nomComando,comando,8), "retardo ")){
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
	}*/
	else {
		printf("Error al ingresar el comando. Verifique sintaxis.");
	}
}

//funciones usadas para obtener y guardar parámetros de un comando
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
//Termina funciones usadas para obtener y guardar parámetros de un comando

void leerComoOperacion(char *comando){
	const int PARAM_SIZE = 20;
	char parametro[6][PARAM_SIZE];
	int i;
	t_operacion operacion;

	for(i=0;i<7;i++){
		obtenerParametroI(i+1, parametro[i], comando);//busca espacios en la gran cadena para separar los parámetros y los guarda en vector
	}
	operacion.proceso=atoi(parametro[1]);
	operacion.base=atoi(parametro[2]);
	operacion.offset=atoi(parametro[3]);
	operacion.tamano=atoi(parametro[4]);
	operacion.accion=(char)*parametro[5];
	operacion.archivo=(char)*parametro[6];

	for(i=1;i<7;i++){
		printf("el parametro %d es: %s\n", i, parametro[i]);
	}

/*	<>
	if(operacion.accion=='s')
	{
		buscarEnMemoriaYEntregar(operacion.proceso, operacion.base, operacion.offset, operacion.tamano);
	}
	else if (operacion.accion=='e')
	{
		escribirBufferEnMemoria(operacion.proceso, operacion.base, operacion.offset, operacion.tamano,operacion.archivo);
	}
*/
}

