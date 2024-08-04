#ifndef INICIALIZAR_ENTSAL_
#define INICIALIZAR_ENTSAL_

#include "es_gestor.h"

typedef struct t_ioconfig{
	char *TIPO_INTERFAZ;
	int TIEMPO_UNIDAD_TRABAJO;
	char *IP_KERNEL;
	char *PUERTO_KERNEL;
	char *IP_MEMORIA;
	char *PUERTO_MEMORIA;
	char *PATH_BASE_DIALFS;
	int BLOCK_SIZE;
	int BLOCK_COUNT;
	int RETRASO_COMPACTACION;
}t_ioconfig;

t_ioconfig *inicializar_entsal(char *nombre, char *config_path);
t_ioconfig *inicializar_configs(char *config_path);
void inicializar_logs();
void imprimir_configs();

#endif /* INICIALIZAR_ENTSAL_ */
