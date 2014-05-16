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

void leerComoOperacion(char *comando){
	char parametro[5][20];
	int i;
//	t_operacion operacion;
	for(i=0;i<6;i++){
		obtenerParametroI(i+1, parametro[i], comando);//busca espacios en la gran cadena para separar los parámetros y los guarda en vector
	}
//	operacion.proceso=atoi(parametro[0]);
//	operacion.base=atoi(parametro[1]);
//	operacion.offset=atoi(parametro[2]);
//	operacion.tamano=atoi(parametro[3]);
//	operacion.accion=(char)*parametro[4];
//	operacion.archivo=(char)*parametro[5];
	for(i=0;i<6;i++){
		printf("el %d parametro es %s:\n", i, parametro[i]);
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

