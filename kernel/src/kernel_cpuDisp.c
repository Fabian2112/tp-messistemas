#include "../include/kernel_cpuDisp.h"

void atender_kernel_cpuD(){

   bool control_key=1;

    while (control_key) {
		int cod_op = recibir_operacion(fd_cpu_dispatch);
        // int size_paq = recibir_int(fd_cpu_dispatch);
		switch (cod_op) {
            case IO_GEN_SLEEP:
                int pid = recibir_int(fd_cpu_dispatch);
                 nombreInterfaz = recibir_string(fd_cpu_dispatch);
                log_error(kernel_logger,"NOmbre INterfaz de cpu dispatch:%s", nombreInterfaz);
                int tiempoADormir = recibir_int(fd_cpu_dispatch);
                /*t_paquete* paquete = crear_paquete(GENERICA);
                agregar_int_a_paquete(paquete, pid);
                agregar_int_a_paquete(paquete, tiempoADormir);
                //buscar interfaz correspondiente a partir de nombreInterfaz, y usar ese socket en particular
                enviar_paquete(paquete, fd_entsal);
                se debe bloquear al proceso
                */
                
                interfaz_fd *fd= obtenerInterfazPorNombre(nombreInterfaz);
                t_paquete* paquete= crear_paquete(GENERICA);
                agregar_int_a_paquete(paquete, pid);
                agregar_int_a_paquete(paquete, tiempoADormir);
                enviar_paquete(paquete,fd->fd_socket);
                eliminar_paquete(paquete);

                t_paquete* paquete2= crear_paquete(DESALOJAR_PCB);
               enviar_paquete(paquete2, fd_cpu_interrupt);
               eliminar_paquete(paquete2);
                break;

            case EXIT_INSTRUCCTION:
                recibir_int(fd_cpu_dispatch);
                recibir_registros2(fd_cpu_dispatch);
                list_add(listaProcesosTerminados, proceso_ejecutando);
                proceso_ejecutando = NULL;
                sem_post(&cantProcesosConcurrentes);
                //sem_post(&existeProcesoRunning);
                sem_post(&runningLibre);
                break;
            case -1:
                log_error(kernel_logger, "El CPU Dispatch se desconecto. Terminando servidor");
                control_key=0;

                break;
            default:
                log_warning(kernel_logger,"Operacion desconocida de CPU Dispatch.");
                break;
            }
	}
}