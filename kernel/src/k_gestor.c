

#include  <../include/k_gestor.h>

 t_log* kernel_logger;
 t_log* kernel_debug; 
 t_config* kernel_config;

 pthread_t hilo_kernel_es;
 pthread_t hilo_kernel_memoria;
 pthread_t hilo_kernel_cpu_dispatch;
 pthread_t hilo_kernel_cpu_interrupt;
 pthread_t hilo_consola;

int identificador_PID = 0;
pthread_mutex_t mutex_pid = PTHREAD_MUTEX_INITIALIZER;


 int fd_kernel;
 int fd_cpu_dispatch;
 int fd_cpu_interrupt;
 int fd_memoria;
 int fd_entsal;

 char* PUERTO_ESCUCHA;
 char* IP_MEMORIA;
 char* PUERTO_MEMORIA;
 char* IP_CPU;
 char* PUERTO_CPU_DISPATCH;
 char* PUERTO_CPU_INTERRUPT;
 char* ALGORITMO_PLANIFICACION;
int QUANTUM;
char** RECURSOS;
char** INSTANCIAS_RECURSOS;
 
 int* RECURSOS_INICIALES;
 int GRADO_MULTIPROGRAMACION;
int dif_multi;
bool subir;    
 int contador_pcbs;
 bool finProcesoEjecutando;

 pthread_mutex_t mutexNuevosProcesos, mutexListaReady, mlog, mutexNew, mutexLoggerKernel,cambio_multi_mutex;


 bool deadlock;
 bool ejecucion_pausada;
 t_pcb* proceso_ejecutando;
 sem_t existenProcesosNuevos, cantProcesosConcurrentes, existeProcesoReady, runningLibre, existeProcesoRunning, 
      new_disponible, ready_disponible, procesos_en_ready,  nivel_multiprogramacion, procesos_en_exit, bloqueado,planiCorto,planiLargo, cambio_multi;
 t_queue* cola_new;
 t_queue* cola_ready;
 t_queue* cola_blocked;
 t_queue* cola_exit;
 t_list* listaNuevosProcesos;
 t_list* listaProcesosReady;
 t_list* listaProcesosTerminados;
 t_list* blocked;
int socket_stdin;
int socket_stdout;
int socket_generico;
int socket_dialFS;


char* stringReady;
int numeroproceso;
char* nombreInterfaz;

t_list *lista_punteros_archivos;
t_list* list_interfaz; 
t_list* procesosBloqueados; 
int pidActual;
bool cpu_ejecutando;
t_pcb *procesoEjecutandoActual;

interfaz_fd* obtenerInterfazPorNombre(char* nombre){
    log_error(kernel_logger,"Tamano de la lista es:%d", list_size(list_interfaz));
    interfaz_fd *fd=list_find(list_interfaz,(void *)coincideNombre);
    log_error(kernel_logger,"EL fd del obtener interfaz: %s", fd->nombre);
    return fd;
}
interfaz_fd *obtenerInterfazSinAtender(){
    
    
    interfaz_fd *fd=list_find(list_interfaz,(void *)sinAtender);
    /*int marco = -1;
    TLB* tlbPtr = list_find(listaTLB, (void*) matchesPidAndPage);
    if(tlbPtr != NULL){
        marco = tlbPtr->marco;
    }
    return marco;*/
    
    return fd;
}
bool sinAtender(interfaz_fd* fd){
    return !fd->atendido;
}
bool coincideNombre(interfaz_fd *fd){
   
    return strcmp(fd->nombre,nombreInterfaz)==0; 
    //return tlb->pid == pid && tlb->pagina == paginaBuscada;
}

