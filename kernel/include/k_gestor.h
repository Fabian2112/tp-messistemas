#ifndef K_GESTOR_
#define K_GESTOR_

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <readline/readline.h>

#include <stdint.h>
#include <stdbool.h>

#include <semaphore.h>

#include <../../utils/include/utils.h>

#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>


// VARIABLES GLOBALES

extern t_log* kernel_logger;
extern t_log* kernel_debug;
extern t_config* kernel_config;

extern pthread_t hilo_kernel_es;
extern pthread_t hilo_kernel_memoria;
extern pthread_t hilo_kernel_cpu_dispatch;
extern pthread_t hilo_kernel_cpu_interrupt;
extern pthread_t hilo_consola;


extern int fd_kernel;
extern int fd_cpu_dispatch;
extern int fd_cpu_interrupt;
extern int fd_memoria;
extern int fd_entsal;

extern char* PUERTO_ESCUCHA;
extern char* IP_MEMORIA;
extern char* PUERTO_MEMORIA;
extern char* IP_CPU;
extern char* PUERTO_CPU_DISPATCH;
extern char* PUERTO_CPU_INTERRUPT;
extern char* ALGORITMO_PLANIFICACION;
extern int QUANTUM;
extern char **RECURSOS;
extern char** INSTANCIAS_RECURSOS;
extern int* RECURSOS_INICIALES;
extern int GRADO_MULTIPROGRAMACION;
extern t_list* blocked;
extern bool finProcesoEjecutando;
extern char* nombreInterfaz;
extern pthread_mutex_t mutex_pid;
extern int identificador_PID;
extern int socket_stdin;
extern int socket_stdout;
extern int socket_generico;
extern int socket_dialFS;
extern int contador_pcbs;
extern t_pcb* proceso_ejecutando;

extern pthread_mutex_t mutexNuevosProcesos, mutexListaReady, mlog,mutexLoggerKernel, mutexNew, cambio_multi_mutex;

extern int numeroproceso;
extern char* stringReady;
extern t_list* new;


extern t_list *lista_punteros_archivos;
extern int pidActual;
extern bool cpu_ejecutando;
extern t_pcb *procesoEjecutandoActual;


extern bool deadlock;
extern bool ejecucion_pausada;
extern t_pcb* proceso_ejecutando;
extern sem_t existenProcesosNuevos, cantProcesosConcurrentes, existeProcesoReady, runningLibre, existeProcesoRunning, 
new_disponible, ready_disponible, procesos_en_ready,  nivel_multiprogramacion, procesos_en_exit, bloqueado,planiCorto,planiLargo, cambio_multi;

extern t_queue* cola_new;
extern t_queue* cola_ready;
extern t_queue* cola_blocked;
extern t_queue* cola_exit;
extern t_list* listaNuevosProcesos;
extern t_list* listaProcesosReady;
extern t_list* listaProcesosTerminados;
extern t_list* list_interfaz; 
extern t_list* procesosBloqueados; 
bool coincideNombre(interfaz_fd *fd);
bool sinAtender(interfaz_fd *fd);
interfaz_fd* obtenerInterfazPorNombre(char* nombre);
interfaz_fd* obtenerInterfazSinAtender();
extern int conexiones; 
extern int dif_multi;
extern bool subir;
#endif  /* K_GESTOR_ */