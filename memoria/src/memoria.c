#include <../include/m_gestor.h>
#include <math.h>

void atender_memoria_entsal2()
{
    log_error(memoria_logger, "no tengo error aca");
    bool control_key = 1;

    while (control_key)
    {
        log_error(memoria_logger, "te cache");
        int cod_op = recibir_operacion(fd_entradaSalida);
        log_error(memoria_logger, "no me tuve que haber metido aca");
        recibir_int(fd_entradaSalida);
        switch (cod_op)
        {
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
            control_key = 0;
            // return EXIT_FAILURE;
            break;
        default:
            log_warning(memoria_logger, "Operacion desconocida de E/S");
            break;
        }
    }
}

int main(int argc, char *argv[])
{

    inicializar_memoria();
    // Bytearray de marcos libres para el manejo de la memoria. 0: libre, 1: ocupado.
    marcos_libres = malloc(sizeof(char) * TAM_MEMORIA / TAM_PAGINA);
    memset(marcos_libres, 0, TAM_MEMORIA / TAM_PAGINA);
    memoria_usuario = malloc(TAM_MEMORIA);
    // iniciar el servidor de memoria
    fd_memoria = -1;
    fd_memoria = iniciar_servidor(PUERTO_ESCUCHA, memoria_logger, "MEMORIA INICIADA");
    if (fd_memoria == -1)
    {
        log_error(memoria_logger, "ERROR: Memoria no levanta servidor");
        return EXIT_FAILURE;
    }
    pthread_t hilo_generica, hilo_stdin, hilo_stdout, hilo_dial_fs;
    pthread_t hilo_memoria_cpu;
    pthread_t hilo_memoria_kernel;

    // esperar conexion de cpu
    /*log_info(memoria_logger, "Esperando a cpu...");
    fd_cpu = esperar_cliente(fd_memoria, memoria_logger, "CPU");
    // esperar conexion de kernel
    log_info(memoria_logger, "Esperando a Kernel...");
    fd_kernel = esperar_cliente(fd_memoria, memoria_logger, "KERNEL");
    */
    int fd_cliente;
    for (int i = 0; i < 3; i++)
    {
        log_info(memoria_logger, "Esperando Clientes...");
        fd_cliente = esperar_cliente(fd_memoria, memoria_logger, "ESPERAR_CLIENTE  FD MEMORIA FUNCIONA");

        int cod_op = recibir_operacion(fd_cliente);
        switch (cod_op)
        {
        case KERNEL:
            log_error(memoria_logger, "Se conecto el KERNEL");
            fd_kernel = fd_cliente;
            bool confirmacion = true;
            send(fd_kernel, &confirmacion, sizeof(bool), 0);
            pthread_create(&hilo_memoria_kernel, NULL, (void *)atender_memoria_kernel, NULL);
            break;
        case CPU:
            log_error(memoria_logger, "Se conecto la CPU");
            fd_cpu = fd_cliente;
            pthread_create(&hilo_memoria_cpu, NULL, (void *)atender_memoria_cpu, NULL);
            break;
        case GENERICA:
            log_error(memoria_logger, "Se conecto la ENTRADASALIDA");
            fd_entradaSalida = fd_cliente;
            // pthread_create(&hilo_generica, NULL, (void *) atender_memoria_entsal, &fd_entradaSalida);
            pthread_create(&hilo_generica, NULL, (void *)atender_memoria_entsal2, NULL);
            break;
        default:
            log_error(memoria_logger, "No reconozco ese codigo");
            break;
        }
    }
    // esperar conexion de entradasalida
    /*log_info(memoria_logger, "Esperando a entradasalida...");
    fd_entradaSalida = esperar_cliente(fd_memoria, memoria_logger, "ENTRADASALIDA");
    log_error(memoria_logger, "loos sockets soon: %d %d %d", fd_cpu, fd_kernel, fd_entradaSalida);
*/
    // Atender mensajes del KERNEL

    pthread_join(hilo_memoria_kernel, NULL);

    // Atender mensajes de Entrada/Salida  -

    pthread_join(hilo_memoria_entsal, NULL);

    // Atender mensajes del CPU

    pthread_join(hilo_memoria_cpu, NULL);

    // Finaliza Memoria
    return EXIT_SUCCESS;
}
