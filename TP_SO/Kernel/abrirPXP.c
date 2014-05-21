/*
 * abrirPXP.c
 *
 *  Created on: 28/04/2014
 *      Author: utnso
 */
#include <pthread.h>
#include <stdio.h>
int main(){
	pthread_t thr1, thr2;
	char *m1 = "thr1";
	char *m2 = "thr2";
	int r1, r2;

	r1 = pthread_create(&thr1, NULL, printf_fnc, (void*) m1);
	r2 = pthread_create(&thr2, NULL, printf_fnc, (void*) m2);

	pthread_join(thr2, NULL);
	pthread_join(thr1, NULL);

	printf("thread 1 devolvio: %d y el Thread 2: %d\n", r1, r2);
}

