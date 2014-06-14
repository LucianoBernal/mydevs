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
#include "commons/collections/list.h"
/*
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
	int activo;
}t_tablaProceso;

typedef struct{
	int proceso;
	int base;
	int offset;
	int tamano;
	char accion; //Las acciones posibles son: Crear/Destruir segmentos y solicitar/enviar bytes
	char *archivo;
}t_operacion;

t_tablaProceso vectorProcesos[20];
int cantProcesos;

void* element_destroyer (void* p){
	free(p);
	return p;
}

int buscarPid(int pid){
	int i;
	for(i = 0; i < cantProcesos; i++){
		if (vectorProcesos[i].pid==pid) return i;
	}
	printf("algo malo paso");
	return 0;
}

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
COMENTADO 04/06
*/
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


//void leerComoOperacion(char *comando){
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
/* COMENTADO 04/06
	t_operacion operacion;
	printf("decime que queres hacer:(para saber como usarlo enviar 'h')\n");
	scanf("%c", &operacion.accion);
	switch (operacion.accion){
		case 'h':
			printf("para escribir bytes en memoria, ingrese 'e' \n para solicitar bytes en memoria, ingrese 's' \n para crear un segmento, ingrese 'c' \n para destruir un segmento, ingrese 'd' \n");
			break;
		case 'e':
			printf("ingrese pid: ");
			scanf("%d",&operacion.proceso);
			printf("\ningrese base: ");
			scanf("%d",&operacion.base);
			printf("\ningrese offset: ");
		    scanf("%d",&operacion.offset);
		    printf("\ningrese tamaño: ");
		    scanf("%d", &operacion.tamano);
		    char mensaje[operacion.tamano];
		    printf("\nahora si, mete lo que quieras:");
		    scanf("%s", mensaje);
		    //Recien despues de to_do esto mandamos
		    enviarUnosBytes(operacion.proceso, operacion.base, operacion.offset, operacion.tamano, mensaje);
		    break;
		case 's':
			printf("ingrese pid: ");
			scanf("%d",&operacion.proceso);
			printf("\ningrese base: ");
			scanf("%d",&operacion.base);
			printf("\ningrese offset: ");
		    scanf("%d",&operacion.offset);
		    printf("\ningrese tamaño: ");
		    scanf("%d", &operacion.tamano);
		    char mensaje[operacion.tamano];
		    mensaje = solicitarBytes(operacion.proceso, operacion.base, operacion.offset, operacion.tamano);
		    printf("%s", mensaje);
		    break;
		case 'c':
			printf("ingrese pid: ");
			scanf("%d",&operacion.proceso);
		    printf("\ningrese tamaño: ");
		    scanf("%d", &operacion.tamano);
		    crearSegmento(operacion.proceso,operacion.tamano);
		    break;
		case 'd':
			printf("ingrese pid: ");
			scanf("%d",&operacion.proceso);
			printf("\ningrese base: ");
			scanf("%d",&operacion.base);
			t_tablaSegmento *aux = vectorProcesos[buscarPid(operacion.proceso)].tabla;
			t_tablaSegmento *aux2=list_filter(aux, aux.inicioLogico!=operacion.base);
			list_destroy_and_destroy_elemnts(aux, element_destroyer);
			break;
		default:
			printf("No ingresó ninguna opción válida.");
	}

}
void * obtenerDirFisica(int base, int offset, int pid){
	return ((list_filter(vectorProcesos[buscarPid(pid)].tabla, vectorProcesos[buscarPid(pid)].tabla->inicioLogico==base))->memPpal)+offset;
}
void enviarUnosBytes(int pid, int base, int offset, int tamano, void* mensaje){
	memcpy(obtenerDirFisica(base, offset, pid), mensaje, tamano);
}

char *solicitarBytes(int pid, int base, int offset, int tamano){
	char *aux;
	memcpy(aux, obtenerDirFisica(base, offset, pid), tamano);
	*(aux+tamano+1)=0;
	return aux;
}

void crearSegmento(int pid, int tamano){

	t_tablaSegmento auxTablaSegmento;
	auxTablaSegmento.identificador = crearIdentificador();
	auxTablaSegmento.inicioLogico = obtenerInicioLogico(pid);
	auxTablaSegmento.tamano = tamano;
	auxTablaSegmento.memPpal = mallocConAlgoritmo(tamano);
	list_add(vectorProcesos[buscarPid(pid)].tabla, auxTablaSegmento);
}

void destruirSegmentos(int pid){
	list_destroy_and_destroy_element(vectorProcesos[buscarPid(pid)].tabla, element_destroyer);
}

//element_destroyer entendemos que es una función que permite destruir algo... no sabemos si la definimos nosotros o qionda :P

int handshake(int pid,char * tipo){
	//control de errores
	vectorProcesos[cantProcesos++].pid=pid;
	vectorProcesos[cantProcesos++].tipo=tipo;
	return 0;
}

void procesSwitch(int pid){
	int i;
	for(i=0; i<cantProcesos;i++){
		vectorProcesos[i].activo=0;
		if(vectorProcesos[i].pid==pid)
			vectorProcesos[i].activo=1;
	}
}


void leerComoRetardo(char *comando){
}

void leerComoAlgoritmo(char *comando){
}

void leerComoCompactacion(char *comando){
}

void leerComoDump(char *comando){
}
COMENTADO 04/06
*/
