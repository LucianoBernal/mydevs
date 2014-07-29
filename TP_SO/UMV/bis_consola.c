/*
 * bis_consola.c
 *
 *  Created on: 29/07/2014
 *      Author: utnso
 */


//void* consola(void* baseUMV) {
//	const int COMANDO_SIZE = 30;
//	char* comando = malloc(COMANDO_SIZE);
//
//	dumpFile = fopen("archivoDeDump.txt","w+");
//	if (dumpFile==NULL) {
//		fputs ("File error",stderr);
//		exit (1);
//	}
//
//	do {
//		printf("\nIngrese comando: \noperacion\ncompactacion\ndump\nretardo\nalgoritmo\nexit\n");
//		fflush(stdin);
//		scanf("%s", comando);
//		analizarYEjecutar(comando);
//
//	} while (strncmp("exit", comando, 4));
//	fclose(dumpFile);
//	free(comando);
//	log_info(logger, "Se cerró la consola");
//	return NULL ;
//}
//
//void analizarYEjecutar(char* com){
//
//}
