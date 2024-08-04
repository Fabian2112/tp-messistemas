#include <../include/cpu_kerneli.h>

void atender_cpu_kernel_interrupt(){
    bool control_key=1;

    while (control_key) {
		//kernel_cpu_dato cpu_data = recibir_interrupt_cpu_kernel(fd_kernel_interrupt);//no está definido
        
        //t_list* miLista = recibir_paquete(fd_kernel_interrupt);
        //int cod_op = list.get(miLista, 0);
        int code_op=recibir_operacion(fd_kernel_interrupt);//el primer elemento siempre será un code_op del paquete que envíe el socket kernel_interrupt
        recibir_int(fd_kernel_interrupt);
        //int size= sizeof(kernel_cpu_dato) //chequear esto
        //t_buffer* unBUffer= recibir_buffer(&size, fd_kernel_interrupt); //lo segundo será un buffer 
        //tendrías que desempaquetar lo que está dentro de buffer y castearlo a kernel_cpu_data
		switch (code_op) {

            case DESALOJAR_PCB:
              	pthread_mutex_lock(&mutex_interrupt);
                continua_ejecucion = false;
                pthread_mutex_unlock(&mutex_interrupt);

                t_paquete* paquete = crear_paquete(DESALOJAR_PCB);
                agregar_int_a_paquete(paquete, pid);
                enviar_registros(paquete, registrosCpu, fd_kernel_interrupt);
			    break;

            case -1:
                log_error(cpu_logger, "El KERNEL-INTERRUPT se desconecto. Terminando servidor");
                control_key=0;
                // return EXIT_FAILURE;
                break;

            default:
                log_warning(cpu_logger,"Operacion desconocida de KERNEL-INTERRUPT");
                break;
        }
	}
}