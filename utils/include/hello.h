
#ifndef HELLO_H_
#define HELLO_H_

#include <stdlib.h>
#include <stdio.h>

#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>

#include <readline/readline.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <commons/collections/queue.h>
#include <commons/collections/list.h>
#include <semaphore.h>
#include "struct.h"
#include <assert.h>
#include <commons/temporal.h>

/**
 * @fn    decir_hola
 * @brief Imprime un saludo al nombre que se pase por parámetro por consola.
 */

// Estructuras del cliente del TP0




//Funciones del cliente del TP0
int crear_conexion(char *ip, char* puerto);

//Funciones del servidor del TP0
int iniciar_servidor(char* puerto, t_log* un_log, char* msj_servidor);
int esperar_cliente(int socket_servidor, t_log* un_log, char* msj_servidor);
int recibir_operacion(int socket_cliente);

//Funciones de manejo de buffer

t_buffer* recibir_todo_buffer(int conexion);
void* extraer_choclo_del_buffer(t_buffer* un_buffer);
int extraer_int_del_buffer(t_buffer* un_buffer);
char* extraer_string_del_buffer(t_buffer* un_buffer);
uint32_t extraer_uint32_del_buffer(t_buffer* un_buffer);

//Funciones de carga de uint32, int y strings al buffer


t_buffer* crear_buffer();
void destruir_buffer(t_buffer* un_buffer);
void agregar_choclo_al_buffer(t_buffer* un_buffer, void* choclo, int size_choclo);
void cargar_int_al_buffer(t_buffer* un_buffer, int int_value);
void cargar_uint32_al_buffer(t_buffer* un_buffer, uint32_t un_valor);
void cargar_string_al_buffer(t_buffer* un_buffer, char* un_string);


void destruir_paquete(t_paquete* un_paquete);
void eliminar_paquete(t_paquete* paquete);
void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio);
t_paquete* crear_paquete();
const char* traducir_estado(t_estado estado);


void enviarPCB(t_pcb* pcb, int socket);
void preparar_registros(t_paquete *paquete, registros_cpu *register_cpu);
t_pcb* recibirPCB(int socket);
void recibir_registros(t_list* lista, t_pcb* pcb_recibido);
void mostrarPCB(t_pcb* proceso);
t_list* recibir_paquete(int socket_cliente);
void* recibir_buffer(int* size, int socket_cliente);

//MEmoria
void enviar_memoria_cpu(memoria_cpu_data* data, int socket_CPU, opcion_funciones instruccion);
//Kernel
t_pcb* recibir_cpu_kernel(int fd_kernel_dispatch);
void enviar_kernel_memoria(kernel_memoria_data* data, int socket, opcion_funciones flag);
kernel_memoria_data* recibir_kernel_memoria(int socket_kernel);
//CPU 
void enviar_cpu_kernel(t_pcb* pcb, int fd_kernel_dispatch, kernel_cpu_dato operacion);

#endif /* HELLO_H_ */

