#include <../include/memoria_entsal.h>

void atender_memoria_entsal(int* socket){

    bool control_key=1;

    while (control_key) {
        log_error(memoria_logger, "te cache");
		int cod_op = recibir_operacion(*socket);
        log_error(memoria_logger, "no me tuve que haber metido aca");
        recibir_int(*socket);
		switch (cod_op) {
            case MENSAJE:
                // recibir_mensaje(cliente_fd);
                break;
            case PAQUETE:
                // lista = recibir_paquete(fd_kernel_dispatch);
                // log_info(cpu_logger, "Me llegaron los siguientes valores:\n");
                // list_iterate(lista, (void*) iterator);
                break;
            case -1:
                log_error(memoria_logger, "E/S desconectada. Terminando servidor");
                control_key=0;
                // return EXIT_FAILURE;
                break;
            default:
                log_warning(memoria_logger,"Operacion desconocida de E/S");
                break;
            }
	}

}