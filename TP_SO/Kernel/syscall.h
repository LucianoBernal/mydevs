#ifndef SYSCALLS_H_
#define SYSCALLS_H_

int sc_obtener_valor(char, int);

int sc_grabar_valor(char, int);
void sc_signal(char*, int);
void sc_wait(char*, t_PCB* , int);

int sc_imprimir();

int sc_imprimirTexto(char*, int );

void armarDiccionarioDeSemaforos();

int programIdDeCpu(int);


#endif /* SYSCALLS_H_ */
