#include "../include/kernel_es.h"

/*void crear_conexiones_fd()
{   log_error(kernel_logger,"ENtro a crear conexiones fs");
    int i=0;
    while(i<=4){
       
       fd_entsal = esperar_cliente(fd_kernel, kernel_logger, "ENTRADA/SALIDA");
       log_error(kernel_logger,"Valor de socket fd_entsal: %d" , fd_entsal);
       interfaz_fd *fd = malloc(sizeof(interfaz_fd));
       recibir_operacion(fd_entsal);
       recibir_int(fd_entsal);
       char* nombre= recibir_string(fd_entsal);
       log_error(kernel_logger,"nombre recibido: %s" , nombre);
       recibir_string(fd_entsal);
       fd->nombre=nombre;
       fd->fd_socket=fd_entsal;
       fd->atendido=false;
       list_add(list_interfaz,fd);
        i++;
    }
}*/



void atender_kernel_es(int* socket){
    
    int control_key=1;
    
    while (control_key) {
		// int cod_op = recibir_operacion(*socket);
        recibir_int(*socket);
		switch (*socket) {
            case GENERICA:
                /*interfaz_fd *fd = malloc(sizeof(interfaz_fd));
                char* nombre= recibir_string(socket);
                log_error(kernel_logger,"nombre recibido: %s" , nombre);
                recibir_string(socket);
                fd->nombre=nombre;
                fd->fd_socket=socket;
                fd->atendido=false;
                list_add(list_interfaz,fd);
                // recibir_mensaje(cliente_fd);
                */
               log_error(kernel_logger, "no deberia estar aca");
                break;
            case PAQUETE:
                // lista = recibir_paquete(fd_kernel_INTERRUPT);
                // log_info(cpu_logger, "Me llegaron los siguientes valores:\n");
                // list_iterate(lista, (void*) iterator);
                break;
            case -1:
                log_error(kernel_logger, "La MEMORIA se desconecto. Terminando servidor");
                control_key=0;
                // return EXIT_FAILURE;
                break;
            default:
                log_warning(kernel_logger,"Operacion desconocida de MEMORIA");
                break;
        }
	}
}