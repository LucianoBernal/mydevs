#ifndef SYSCALLS_H_
#define SYSCALLS_H_

#include "Kernel.h"
#include <string.h>
#include "PCP.h"
#include <biblioteca_comun/Serializacion.h>


extern sem_t mutexVG;
extern t_dictionary* variables_globales;
extern sem_t diccionarioSemaforosMutex;
extern t_dictionary* diccionarioSemaforos;
extern sem_t colaReadyMutex;
int sc_obtener_valor(char*, int);

int sc_grabar_valor(char*, int);
void sc_signal(char*, int);
void sc_wait(char*, t_PCB* , int);

int sc_imprimir();

int sc_imprimirTexto(char*, int );

int programIdDeCpu(int);


#endif /* SYSCALLS_H_ */
