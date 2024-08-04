#ifndef ENTRADASALIDA_H_
#define ENTRADASALIDA_H_

#include "es_gestor.h"

// VARIABLES GLOBALES
t_log *entradaSalida_logger;
t_config *entradaSalida_config;
pthread_t hilo_entsal_kernel;
pthread_t hilo_entsal_memoria;

int fd_memoria;
int fd_kernel;
int puerto_entradasalida;

t_ioconfig* ioconfig;

char* buffer_bloques;
t_bitarray* buffer_bitmap;

pthread_mutex_t mutex_memoria;

#endif /* ENTRADASALIDA_H_ */
