#include <../include/kernel_cpuInt.h>

void atender_kernel_cpuI()
{

    bool control_key = 1;

    while (control_key)
    {
        int cod_op = recibir_operacion(fd_cpu_interrupt);
        recibir_int(fd_cpu_interrupt);
        switch (cod_op)
        {
        case MENSAJE:

            break;
        case PAQUETE:

            break;
        case DESALOJAR_PCB:
        // int pid= recibir_int(fd_cpu_interrupt);
        proceso_ejecutando->registros_cpu= recibir_registros2(fd_cpu_interrupt);
        list_add(procesosBloqueados,proceso_ejecutando);
        sem_post(&runningLibre);
        break;
        case -1:
            log_error(kernel_logger, "El CPU Interrupt se desconecto. Terminando servidor");
            control_key = 0;

            break;
        default:
            log_warning(kernel_logger, "Operacion desconocida de CPU Interrupt.");
            break;
        }
    }
}