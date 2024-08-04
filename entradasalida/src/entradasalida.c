#include <../include/entradasalida.h>
#include <../include/inicializar_entsal.h>

int main(int argc, char *argv[])
{
    // Inicializar estructuras de entradasalida
    ioconfig = inicializar_entsal("INTERFAZ", argv[1]);
    // Conectar como cliente a MEMORIA
    
    // Conectar como cliente a KERNEL
    

    // Atender mensajes del KERNEL
    pthread_t hilo_entsal_kernel;
    pthread_create(&hilo_entsal_kernel, NULL, (void *)atender_entsal_kernel, NULL);


    // Atender mensajes de la MEMORIA
    pthread_t hilo_entsal_memoria;
    pthread_create(&hilo_entsal_memoria, NULL, (void *)atender_entsal_memoria, NULL);
    pthread_join(hilo_entsal_kernel, NULL);
    pthread_join(hilo_entsal_memoria, NULL);

    return EXIT_SUCCESS;
}

//Esta funcion va a enviar el nombre de interfaz y el tipo, para que el Kernel lo meta a una lista de entradasalida
void enviar_tipo_interfaz(int socket_kernel, char* nombre_interfaz, char* tipo_interfaz){
	t_paquete* paquete = crear_paquete(GENERICA);
	agregar_string_a_paquete(paquete, nombre_interfaz);
	agregar_string_a_paquete(paquete, tipo_interfaz);
	enviar_paquete(paquete, socket_kernel);
	eliminar_paquete(paquete);
}
