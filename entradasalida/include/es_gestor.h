#ifndef ES_GESTOR_
#define ES_GESTOR_

#include <stdlib.h>
#include <stdio.h>
#include <../../utils/include/utils.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <pthread.h>
#include <readline/readline.h>
#include "inicializar_entsal.h"
#include "entsal_memoria.h"
#include "entsal_kernel.h"
#include <commons/bitarray.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <commons/bitarray.h>
#include <sys/mman.h>
#include <commons/collections/list.h>
#include <dirent.h>
#include <stdbool.h>

// varieables GLOBALES

extern t_log *entradaSalida_logger;
extern t_config *entradaSalida_config;
extern pthread_t hilo_entsal_memoria;
extern pthread_t hilo_entsal_kernel;

extern int fd_memoria;
extern int fd_kernel;
extern int puerto_entradasalida;
extern struct t_ioconfig* ioconfig;

extern char* buffer_bloques;
extern t_bitarray* buffer_bitmap;

extern pthread_mutex_t mutex_memoria;

void atender_entsal_memoria();
void atender_entsal_kernel();

#endif /* ES_GESTOR_ */
