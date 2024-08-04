#include <../include/cpu_kerneld.h>

void atender_cpu_kernel_dispatch(){
    bool control_key=1;

    while (control_key) {
        //t_pcb* contexto = recibir_cpu_kernel(fd_kernel_dispatch);
		int code_op=recibir_operacion(fd_kernel_dispatch);
        recibir_int(fd_kernel_dispatch);

        //esperar a que me manden un proceso
        switch (code_op) {
            
            case EXECUTE_CPU:
				//llenar registros con (pid, contextoEjecucion)
                //existe_interrupcion = false <- este va tambien
                //t_pcb* pcb = (t_pcb*) recibirPCB(fd_kernel_dispatch);
                pid = recibir_int(fd_kernel_dispatch);
                registrosCpu = recibir_registros2(fd_kernel_dispatch);
                continua_ejecucion = true;
                ejecutar_ciclo_de_instruccion();
                
				//liberarPcb(contexto);
				break;
            case -1:
                log_error(cpu_logger, "El KERNEL-DISPATCH se desconecto. Terminando servidor");
                control_key=0;
                // return EXIT_FAILURE;
                break;
            default:
                log_warning(cpu_logger,"Operacion desconocida de KERNEL-DISPATCH");
                break;
            }
	}
}