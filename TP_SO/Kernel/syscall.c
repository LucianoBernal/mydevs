#include "syscall.h"

void sc_obtener_valor(char* id, int idCpu) {
	sem_wait(&mutexVG);
	int* a = dictionary_get(variables_globales, id);
	int *aProp=malloc(sizeof(int));
	*aProp= *a;
	sem_post(&mutexVG);
	enviarConRazon(idCpu, logKernel, OBTENER_VALOR, serializar2(crear_nodoVar(aProp,4),0));
	free(aProp);
} //VISTA

void sc_grabar_valor(char* id, int valor, int idCpu) {
	sem_wait(&mutexVG);
	dictionary_remove_and_destroy(variables_globales, id, (void*) free);
	int *valorProp=malloc(sizeof(int));
	*valorProp=valor;
	dictionary_put(variables_globales, id, &valorProp);
	sem_post(&mutexVG);
	enviarConRazon(idCpu, logKernel, GRABAR_VALOR, serializar2(crear_nodoVar(valorProp,4),0));
	free(valorProp);
}//VISTA

void sc_signal(char* idSem, int idCPU) {
	sem_wait(&diccionarioSemaforosMutex);
	t_estructuraSemaforo* semaforo = dictionary_get(diccionarioSemaforos,
			idSem);
	semaforo->valor = (semaforo->valor) + 1;
	sem_post(&diccionarioSemaforosMutex);
	if (!queue_is_empty(semaforo->procesosBloqueados)) {
		sem_wait(&(semaforo->mutexCola));
		t_PCB* pcbADesbloquear = queue_pop(semaforo->procesosBloqueados);
		sem_post(&(semaforo->mutexCola));
		sem_wait(&colaReadyMutex);
		queue_push(colaReady, pcbADesbloquear);
		sem_post(&colaReadyMutex);
	}
} //VISTA


void sc_wait(char* idSem, t_PCB* pcb, int idCPU) {
	int * estado_semaforo=malloc(sizeof(int));
	sem_wait(&diccionarioSemaforosMutex);
	t_estructuraSemaforo* semaforo = dictionary_get(diccionarioSemaforos,
			idSem);
	int a = semaforo->valor;
	semaforo->valor = (semaforo->valor) - 1;
	sem_post(&diccionarioSemaforosMutex);
	if (a > 0) {
		*estado_semaforo=1;
		enviarConRazon(idCPU, logKernel, WAIT, serializar2(crear_nodoVar(estado_semaforo,4),0));
		//(puede seguir)
	} else {
		*estado_semaforo=0;
		enviarConRazon(idCPU, logKernel, WAIT, serializar2(crear_nodoVar(estado_semaforo,4),0));
		//(se libera)
		seLiberoUnaCPU(idCPU);
		sem_wait(&(semaforo->mutexCola));
		queue_push(semaforo->procesosBloqueados, pcb);
		sem_post(&(semaforo->mutexCola));
	}

} //VISTA

void sc_imprimir(int valorAMostrar, int idCpu) {
	int programID = programIdDeCpu(idCpu);
		int sd = obtener_sd_Programa(programID);
		int* valorAMostrarBuffer=malloc(sizeof(int));
		*valorAMostrarBuffer=valorAMostrar;
		send(sd, valorAMostrarBuffer, 4, 0);
		int cantDigitos = strlen(string_from_format("%d",valorAMostrar));
		enviarConRazon(idCpu, logKernel, IMPRIMIR_TEXTO, serializar2(crear_nodoVar(&cantDigitos,4),0));
} //VISTA

void sc_imprimirTexto(char* texto, int idCpu) {
	int programID = programIdDeCpu(idCpu);
	int sd = obtener_sd_Programa(programID);
//	notificar_Programa(sd, texto);
//	int cantDigitos = strlen(texto);
	enviarConRazon(sd, logKernel, IMPRIMIR_TEXTO, serializar2(crear_nodoVar(texto, strlen(texto)), 0));
	enviarConRazon(idCpu, logKernel, CONFIRMACION, NULL);
//	enviarConRazon(idCpu, logKernel, IMPRIMIR_TEXTO, serializar2(crear_nodoVar(&cantDigitos,4),0));
} //VISTA


int programIdDeCpu(int idCPU) {
	sem_wait(&CPUsMutex);
	int posicion = posicionEnLaLista(CPUs, idCPU);
	t_estructuraCPU* CPU = list_get(CPUs, posicion);
	sem_post(&CPUsMutex);
	return (CPU->idProceso);

}
