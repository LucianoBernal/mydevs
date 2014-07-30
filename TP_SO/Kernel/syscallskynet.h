#ifndef SYSCALLSKYNET_H_
#define SYSCALLSKYNET_H_

#include "Kernel.h"
#include <string.h>
#include "PCP.h"
#include <biblioteca_comun/Serializacion.h>


extern sem_t mutexVG;
extern t_dictionary* variables_globales;
extern sem_t diccionarioSemaforosMutex;
extern t_dictionary* diccionarioSemaforos;
extern sem_t colaReadyMutex;
void sc_obtener_valor(char*, int);

void sc_grabar_valor(char*, int, int);
void sc_signal(char*, int);
void sc_wait(char*, int);

void sc_imprimir();

void sc_imprimirTexto(char*, int );

int programIdDeCpu(int);


#endif /* SYSCALLS_H_ */
