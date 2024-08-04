#include <../include/entsal_memoria.h>

void atender_entsal_memoria(){
    log_error(entradaSalida_logger, "Entrando a atender_entsal_memoria");
    fd_memoria=-1;//para que se quede esperando la conexion
    while(fd_memoria<=0)
    {    
        log_error(entradaSalida_logger, "Conectando a memoria...");
        fd_memoria = crear_conexion(ioconfig->IP_MEMORIA, ioconfig->PUERTO_MEMORIA);
        if((fd_memoria)<=0) log_error(entradaSalida_logger, "Intentando conectar a memoria");
        
        sleep(1);
    }
    int codOp = GENERICA;
    send(fd_memoria, &codOp, sizeof(int), 0);
    bool control_key=1;

    while (control_key) {
		int cod_op = recibir_operacion(fd_memoria);
		switch (cod_op) {
            case MONITOR:

            	break;
            case -1:
                log_error(entradaSalida_logger, "La MEMORIA se desconecto. Terminando servidor");
                control_key=0;
                // return EXIT_FAILURE;
                break;
            default:
                log_warning(entradaSalida_logger,"Operacion desconocida de MEMORIA");
                break;
            }
	}
}
