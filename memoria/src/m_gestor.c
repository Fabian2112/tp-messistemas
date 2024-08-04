#include "../include/m_gestor.h"

t_log *memoria_logger;
t_config *memoria_config;
pthread_t hilo_memoria_cpu;
pthread_t hilo_memoria_kernel;
pthread_t hilo_memoria_entsal;

int fd_memoria;
int fd_entradaSalida;
int fd_cpu;
int fd_kernel;

char *PUERTO_ESCUCHA;
int TAM_MEMORIA;
int TAM_PAGINA;
char *PATH_INSTRUCCIONES;
int RETARDO_RESPUESTA;

char *PATH_INSTRUCCIONES;

t_list *listaProcesosMemoria;
void *memoria_usuario;
char *marcos_libres;
int pagina_buscada;
int pidBuscado;

bool matchesPid(kernel_memoria_process *contenido)
{
    return contenido->pid == pidBuscado;
}

kernel_memoria_process *encontrar_proceso_memoria(int pid)
{
    kernel_memoria_process *contenido = list_find(listaProcesosMemoria, (void *)matchesPid);

    return contenido;
}
