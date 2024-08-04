#include <../include/cpu.h>
//sudo ss -tuln | grep LISTEN sirve para ver los puertos en airea
//sudo lsof -i :8010 para mostrar quien está usando determinado puerto (8010=puerto para ver)
 
int main(int argc, char* argv[]) {
    
    inicializar_cpu();
    //levantar cpu dispatch
    //fd_cpu_dispatch = iniciar_servidor(PUERTO_ESCUCHA_DISPATCH, cpu_logger, "CPU-DISPATCH LEVANTADO");
    //levantar cpu interrupt
    
    //conectarse a memoria
    //log_info(cpu_logger, "Conectando a memoria...");
    //fd_memoria = crear_conexion(IP_MEMORIA, PUERTO_MEMORIA);
    //esperar conexion kernel al dispatch
    //log_info(cpu_logger, "Esperando conexion de KERNEL a DISPATCH...");
    //fd_kernel_dispatch = esperar_cliente(fd_cpu_dispatch, cpu_logger, "KERNEL_DISPATCH");
    //esperar conexion kernel al interrupt
    //log_info(cpu_logger, "Esperando conexion KERNEL a INTERRUPT...");
    //fd_kernel_interrupt = esperar_cliente(fd_cpu_interrupt, cpu_logger, "KERNEL_INTERRUPT");
    
    //Atender mensajes de la MEMORIA
    pthread_t hilo_cpu_memoria;
    pthread_create(&hilo_cpu_memoria, NULL, (void*)conectar_a_memoria, NULL);
    //pthread_detach(hilo_cpu_memoria);
   
    //Atender mensajes del KERNEL-DISPATCH   ---- HECHO
    pthread_t hilo_cpu_kernel_dispatch;
    pthread_create(&hilo_cpu_kernel_dispatch, NULL, (void*)conectar_a_kernel_dispatch, NULL);
    
    
    //Atender mensajes del KERNEL-INTERRUPT
    pthread_t hilo_cpu_kernel_interrupt;
    pthread_create(&hilo_cpu_kernel_interrupt, NULL, (void*)conectar_a_kernel_interrupt, NULL);
    pthread_join(hilo_cpu_kernel_interrupt, NULL);
    pthread_join(hilo_cpu_memoria, NULL);
    pthread_join(hilo_cpu_kernel_dispatch,NULL);

    return EXIT_SUCCESS; 

    
}

int conectar_a_kernel_interrupt(){
    log_info(cpu_logger, "Iniciando servidor KERNEL_INTERRUPT...");
    fd_cpu_interrupt=-1;
    while (fd_cpu_interrupt<=0)
    {   
        log_info(cpu_logger, "Levantando servidor KERNEL_INTERRUPT...");
        fd_cpu_interrupt = iniciar_servidor(PUERTO_ESCUCHA_INTERRUPT, cpu_logger, "CPU-INTERRUPT LEVANTADO");
        if((fd_cpu_interrupt)<=0) log_error(cpu_logger, "ERROR: No  se pudo levantar el servidor KERNEL_INTERRUPT");
        
        sleep(1);
    }
    log_info(cpu_logger, "Se ha levantado el servidor KERNEL_INTERRUPT...");
    fd_kernel_interrupt = esperar_cliente(fd_cpu_interrupt,cpu_logger,"KERNEL_INTERRUPT");
	int handshake = recibir_operacion(fd_kernel_interrupt);
    if(handshake==KERNEL){
        bool confirmacion = true;
        send(fd_kernel_interrupt, &confirmacion, sizeof(bool), 0);
        log_info(cpu_logger, "MENSAJE DE KERNEL INTERRUPT RECIBIDO");
    }else log_error(cpu_logger, "ERROR: Handshake equivocado en KERNEL_INTERRUPT");

    atender_cpu_kernel_interrupt();
    return 0;
}

int conectar_a_kernel_dispatch()
{
    log_info(cpu_logger, "Iniciando servidor KERNEL_DISPATCH...");
    fd_cpu_dispatch=-1;
    while (fd_cpu_dispatch<=0)
    {   
        log_info(cpu_logger, "Levantando servidor KERNEL_DISPATCH...");
        fd_cpu_dispatch = iniciar_servidor(PUERTO_ESCUCHA_DISPATCH, cpu_logger, "KERNEL-DISPATCH LEVANTADO");
        if((fd_cpu_dispatch)<=0) log_error(cpu_logger, "ERROR: No  se pudo levantar el servidor KERNEL_DISPATCH");
        
        sleep(3);
    }
    log_info(cpu_logger, "Se ha levantado el servidor KERNEL_DISPATCH...");
    fd_kernel_dispatch = esperar_cliente(fd_cpu_dispatch,cpu_logger,"KERNEL_DISPATCH");
	int handshake = recibir_operacion(fd_kernel_dispatch);
    if(handshake==KERNEL){
        bool confirmacion = true;
        send(fd_kernel_dispatch, &confirmacion, sizeof(bool), 0);
        log_info(cpu_logger, "MENSAJE DE KERNEL_DISPATCH RECIBIDO");
    }else log_error(cpu_logger, "ERROR: Handshake equivocado en KERNEL_DISPATCH");

    atender_cpu_kernel_dispatch();
    return 0;
}
void conectar_a_memoria(){
	log_info(cpu_logger, "Creando conexión con MEMORIA...");
    fd_memoria = -1;


    // log_protegido_cpu(string_from_format("CONEXION MEMORIA: Lei la IP %s y  puerto %s\n",ip,puerto));
    while (fd_memoria<=0)
    {   
        log_info(cpu_logger, "Conectandose a MEMORIA...");
        fd_memoria = crear_conexion(IP_MEMORIA, PUERTO_MEMORIA);
        if((fd_memoria)<=0) log_error(cpu_logger, "ERROR: No  se pudo conectar a MEMORIA");
        
        sleep(1);
    }
    
	if(fd_memoria>0){
        int identificador = CPU;

		send(fd_memoria, &identificador, sizeof(int), 0);
		//recv(fd_memoria, &tam_pag, sizeof(int), MSG_WAITALL);//tenemos que charlar como se envía el tam

		log_info(cpu_logger,"Conexion con memoria exitosa");
		
    }
    atender_cpu_memoria();	


}