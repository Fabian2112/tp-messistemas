#include <../include/kernel.h>
int conexiones= 0; 
int main(int argc, char* argv[]) {
       
    //iniciar kernel
    inicializar_kernel();

    //iniciar servidor kernel
    fd_kernel = iniciar_servidor(PUERTO_ESCUCHA, kernel_logger, "KERNEL LEVANTADO");
    pthread_t hilo_multi;
     pthread_create(&hilo_multi, NULL, (void*)manejo_multi, NULL);
    pthread_t hilo_kernel_cpu_dispatch;
    pthread_create(&hilo_kernel_cpu_dispatch, NULL, (void*)conectar_a_cpu_dispatch, NULL);
    //Atender mensajes del CPU int
    pthread_t hilo_kernel_cpu_interrupt;
    pthread_create(&hilo_kernel_cpu_interrupt, NULL, (void*)conectar_a_cpu_interrupt, NULL);
    //Atender mensajes de la MEMORIA
    pthread_t hilo_kernel_memoria;
    pthread_create(&hilo_kernel_memoria, NULL, (void*)conectar_a_memoria, NULL);
    pthread_t hilo_kernel_es;
    pthread_create(&hilo_kernel_es, NULL, (void*)conectar_a_io, NULL);
   

    pthread_t hilo_planificador_largo;
    pthread_create(&hilo_planificador_largo, NULL, (void*)manejadorPlanificadorLargoPlazo, NULL);
  
    pthread_t hilo_planificador_corto;
    pthread_create(&hilo_planificador_corto, NULL, (void*)manejadorPlanificadorCortoPlazo, NULL);
    pthread_detach(hilo_planificador_corto);
    pthread_detach(hilo_kernel_cpu_dispatch);
    pthread_detach(hilo_planificador_largo);
    pthread_detach(hilo_kernel_memoria);
    pthread_detach(hilo_kernel_cpu_interrupt);
    pthread_detach(hilo_kernel_es);
    pthread_detach(hilo_multi);
/*
    //hilo de ejecucion 
    pthread_t th_execute_pcb;
    pthread_create(&th_execute_pcb, NULL, (void*) execute, NULL);
    pthread_detach(th_execute_pcb);
*/

/*
    if(strcmp(ALGORITMO_PLANIFICACION, "RR") == 0){
            //comparar tiempo_llegada_running y tiempo_actual vs quantum 
            //si supera, se lo manda al final de cola
			pthread_t round_robin;
			pthread_create(&round_robin, NULL, (void*) desalojador_round_robin, NULL);
			
			pthread_detach(round_robin);
    }
*/

    //Inicio de la Consola para Ejecucion de Funciones

    //iniciar_consola_funciones();
    pthread_t hilo_consola;
    pthread_create(&hilo_consola, NULL, (void*)iniciar_consola_funciones, NULL);
    pthread_join(hilo_consola,NULL);

    log_debug(kernel_debug,"Se ha desconectado de kernel");

    return EXIT_SUCCESS;
}


int conectar_a_memoria(){
    log_info(kernel_logger, "Iniciando conexión a memoria...");
    fd_memoria=-1;//para que se quede esperando la conexion
    while((fd_memoria)<=0)
    {    log_info(kernel_logger, "Conectando a memoria...");
        fd_memoria = crear_conexion(IP_MEMORIA, PUERTO_MEMORIA);
        if((fd_memoria)<=0) log_error(kernel_logger, "Intentando conectar a memoria");
        
        sleep(1);
    }
    //Acá voy verificando el protocolo de comunicación 
    int handshake_kernel=KERNEL;
    bool result;
    send(fd_memoria, &handshake_kernel, sizeof(int), 0);
    recv(fd_memoria, &result, sizeof(bool), MSG_WAITALL); 
    if(result)
    {
        log_info(kernel_logger, "Conexión a memoria éxitosa");
        conexiones+=1;
    }else log_error(kernel_logger, "ERROR: HANDSHAKE CON MEMORIA FALLIDO");
    
    atender_kernel_memoria();
    return 0;
}

int conectar_a_cpu_dispatch(){
    log_info(kernel_logger, "Iniciando conexión a cpu_dispatch...");
    fd_cpu_dispatch=-1;//para que se quede esperando la conexion
    while((fd_cpu_dispatch)<=0)
    {    log_info(kernel_logger, "Conectando a cpu_dispatch...");
        fd_cpu_dispatch = crear_conexion(IP_CPU, PUERTO_CPU_DISPATCH);
        if((fd_cpu_dispatch)<=0) log_error(kernel_logger, "Intentando conectar a cpu_dispatch");
        
        sleep(1);
    }
    //Acá voy verificando el protocolo de comunicación 
    int handshake_kernel=KERNEL;
    bool result;
    send(fd_cpu_dispatch, &handshake_kernel, sizeof(int), 0);
    recv(fd_cpu_dispatch, &result, sizeof(bool), MSG_WAITALL); 
    if(result)
    {
        log_info(kernel_logger, "Conexión a cpu_dispatch éxitosa");
        conexiones+=1;
    }else  log_error(kernel_logger, "ERROR: HANDSHAKE CON CPU_DISPATCH FALLIDO");
    conexiones+=1;
    atender_kernel_cpuD();
    return 0;
}

int conectar_a_cpu_interrupt(){
    log_info(kernel_logger, "Iniciando conexión a cpu_interrupt...");
    fd_cpu_interrupt=-1;//para que se quede esperando la conexion
    while((fd_cpu_interrupt)<=0)
    {    log_info(kernel_logger, "Conectando a cpu_interrupt...");
        fd_cpu_interrupt = crear_conexion(IP_CPU, PUERTO_CPU_INTERRUPT);
        if((fd_cpu_interrupt)<=0) log_error(kernel_logger, "Intentando conectar a cpu_interrupt");
        
        sleep(1);
    }
    //Acá voy verificando el protocolo de comunicación 
    int handshake_kernel=KERNEL;
    bool result;
    send(fd_cpu_interrupt, &handshake_kernel, sizeof(int), 0);
    recv(fd_cpu_interrupt, &result, sizeof(bool), MSG_WAITALL); 
    if(result)
    {
        log_info(kernel_logger, "Conexión a cpu_interrupt éxitosa");
        conexiones+=1;
    }else log_error(kernel_logger, "ERROR: HANDSHAKE CON CPU_INTERRUPT FALLIDO");
    conexiones+=1;
    atender_kernel_cpuI();
    return 0;
}

int conectar_a_io(){
    pthread_t hilo_generica, hilo_stdin, hilo_stdout, hilo_dial_fs;
    for (int i = 0; i < 1; i++){
        fd_entsal = esperar_cliente(fd_kernel,kernel_logger, "ENTRADA/SALIDA");

        int cod_op = recibir_operacion(fd_entsal);
        
        recibir_int(fd_entsal);
        conexiones+=1;
        switch (cod_op){
            case GENERICA:
                socket_generico = fd_entsal;
                interfaz_fd *fd = malloc(sizeof(interfaz_fd));
                char* nombre= recibir_string(socket_generico);
                log_error(kernel_logger,"nombre recibido: %s" , nombre);
                recibir_string(socket_generico);
                fd->nombre=nombre;
                fd->fd_socket=socket_generico;
                fd->atendido=false;
                list_add(list_interfaz,fd);
                pthread_create(&hilo_generica, NULL, (void *) atender_kernel_es, &socket_generico);

                break;
            case STDIN:
                socket_stdin = fd_entsal;
                pthread_create(&hilo_stdin, NULL, (void *) atender_kernel_es, &socket_stdin);
                break;
            case STDOUT:
                socket_stdout = fd_entsal;
                pthread_create(&hilo_stdout, NULL, (void *) atender_kernel_es, &socket_stdout);
                break;
            case DIAL_FS:
                socket_dialFS = fd_entsal;
                pthread_create(&hilo_dial_fs, NULL, (void *) atender_kernel_es, &socket_dialFS);
                break;    
            default:
                log_error(kernel_logger, "En conectar_IO no se recibió el cod_op correspondiente");
                break;
        }   
    }
    return 0;
}

void manejo_multi(){
    sem_wait(&cambio_multi);
    pthread_mutex_lock(&cambio_multi_mutex);
    for (int i = 0; i < dif_multi; i++)
    {   log_error(kernel_logger,"he entrado al for_manejo_multo %d %d", subir, dif_multi);
        if(subir) sem_post(&cantProcesosConcurrentes);
        else sem_wait(&cantProcesosConcurrentes);
    }
    
    pthread_mutex_unlock(&cambio_multi_mutex);
}