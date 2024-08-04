#ifndef PLANIFICADOR_LARGO_PLAZO_H_
#define PLANIFICADOR_LARGO_PLAZO_H_

#include "k_gestor.h"
#include "desalojador.h"

/*
t_log* kernel_logger;
t_list* listaProcesosReady;
t_pcb* proceso_ejecutando;
sem_t existeProcesoReady;
sem_t runningLibre;
sem_t existeProcesoRunning;
pthread_mutex_t mutexListaReady;
int fd_cpu_dispatch;
*/
void takeProcessNext();
void manejadorPlanificadorCortoPlazo(void* arg);


#endif /* PLANIFICADOR_LARGO_PLAZO_H_ */