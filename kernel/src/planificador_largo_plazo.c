#include "../include/planificador_largo_plazo.h"

//char* stringReady = NULL;
//int numeroproceso = 0;

//new = list_create(); //no sé cuál es esto porque lo más parecido es queue_new() pero no es list_create
//new=list_create(); //creó una nueva lista porque se utiliza en algún lado



void manejadorPlanificadorLargoPlazo(){	
	while(true){
		pthread_mutex_lock(&cambio_multi_mutex);
		sem_wait(&existenProcesosNuevos);
		sem_wait(&cantProcesosConcurrentes);
		sem_wait(&planiLargo);
		pthread_mutex_lock(&mutexNuevosProcesos);
		pthread_mutex_lock(&mutexListaReady);
		moveProcessFIFO(listaNuevosProcesos, listaProcesosReady);
		sem_post(&existeProcesoReady);
		logColaReady();
		//IF(CONFIRMACION==TRUE) for (int i = 0; i < count; i++)
            /*{
                sem_wait(&cantProcesosConcurrentes);
            }*/
		pthread_mutex_unlock(&mutexNuevosProcesos);
		pthread_mutex_unlock(&mutexListaReady);
		sem_post(&planiLargo);
		pthread_mutex_unlock(&cambio_multi_mutex);
	}
}

void iteratorReady(t_pcb* pcbActual){
	int length = snprintf( NULL, 0, "%d", pcbActual->pid);
	char* str = malloc( length + 1 );
	snprintf( str, length + 1, "%d", pcbActual->pid );
	if(string_length(stringReady) > 1){
		string_append(&stringReady, ",");
	}
	string_append(&stringReady, str);
	free(str);
}

void moveProcessFIFO(t_list* listaDesde, t_list* listaHasta){
	t_pcb* pcbToMove = list_remove(listaDesde, 0);
	list_add(listaHasta, pcbToMove);
	log_info(kernel_logger, "PID: %d - Estado Anterior: %s - Estado Actual: %s", pcbToMove->pid, "NEW", "READY");
}

void logColaReady(){
	stringReady = string_new();
	string_append(&stringReady, "[");
	list_iterate(listaProcesosReady, (void*) iteratorReady);//agrupa todos los procesos en ready y los separa por coma 
	string_append(&stringReady, "]");
	log_info(kernel_logger, "Cola Ready %s: %s", ALGORITMO_PLANIFICACION, stringReady);
	free(stringReady);
}


/*void nuevoProceso(char *path)
{
	t_pcb* aux = (t_pcb*)crear_pcb();
    

	//int size = sizeof(aux);

	//crear_memoria_pcb(aux->pid, path, size);
    
    pthread_mutex_lock(&mutexNew);
    list_add(listaNuevosProcesos, aux); //antes recibía el new pero tendría que agregarlo en listaNUevosPRocesos
    pthread_mutex_unlock(&mutexNew);
    
}
*/
