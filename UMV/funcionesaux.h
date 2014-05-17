/*
 * funcionesaux.h
 *
 *  Created on: 16/05/2014
 *      Author: utnso
 */

#ifndef FUNCIONESAUX_H_
#define FUNCIONESAUX_H_

void analizarYEjecutar(char *);
void leerComoOperacion(char*);
void leerComoRetardo(char*);
void leerComoAlgoritmo(char*);
void leerComoCompactacion(char*);
void leerComoDump(char*);
void obtenerParametroI(int, char*, char*);
void *interpretadoComoPuntero(char*);
void buscarEnMemoriaYEntregar(int, void*, int, int);
void escribirBufferEnMemoria(int, void*, int, int, char);
int buscarEspacio(int, char *);


#endif /* FUNCIONESAUX_H_ */
