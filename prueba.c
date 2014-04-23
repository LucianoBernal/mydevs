#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int sum;
void *runner (void *param);
/*void meterVariable(void **direccion_sp, char nom, int valor);
void sacarVariable(void **direccion_sp, char *p_nom, int *p_val);*/

int main(int argc, char *argv[])
{
	/*void *stack_base = malloc(100);
	void *stack_pointer = stack_base;
	int valAntes, valDespues;
	char nomAntes, nomDespues;
	//printf("Hola, soy belen\n");
	//printf("Hola, soy Leandro\n");
	//printf("Hola, soy Luciano\n");
	//printf("primer commit desde github de Luciano\n");
	//No se los queria borrar <3
	//meto los valores
	valAntes = 15;
	nomAntes = 'a';
	//El unsigned int solamente hace que no subraye printf
	//me molesta demasiado
	printf("a ver cuanto vale el sp: %x\n", (unsigned int)stack_pointer);
	meterVariable(&stack_pointer, nomAntes, valAntes);
	printf("a ver cuanto vale el sp: %x\n", (unsigned int)stack_pointer);
	//saco los valores
	sacarVariable(&stack_pointer, &nomDespues, &valDespues);
	printf("a ver cuanto vale el sp: %x\n", (unsigned int)stack_pointer);
	printf("las variables afuera son: %c y %d\n", nomDespues, valDespues);
	return 0;*/

	pthread_t tid;
	pthread_attr_t attr;

	if (argc !=2) {
		fprintf (stderr, "uso: a.out <valor entero>\n");
		return -1;
	}
	if (atoi (argv[1]) < 0) {
		fprintf (stderr, "%d debe ser >= 0 \n", atoi(argv[1]));
		return -1;
	}
	pthread_attr_init (&attr);
	pthread_create (&tid, &attr, runner, argv[1]);
	pthread_join (tid,NULL);

	printf("sum = %d\n", sum);
	funcion();
	return 0;
}

void *runner (void *param){
	int i,upper = atoi(param);
	sum=0;

	for(i=1; i<=upper;i++)
		sum += i;

	pthread_exit(0);
}
