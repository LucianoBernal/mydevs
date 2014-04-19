#include <stdio.h>
#include <stdlib.h>

void meterVariable(void **direccion_sp, char nom, int valor);
void sacarVariable(void **direccion_sp, char *p_nom, int *p_val);

int main()
{
	void *stack_base = malloc(100);
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
	return 0;
}
