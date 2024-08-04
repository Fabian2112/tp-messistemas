#include "hello.h"

t_paquete* crear_paquete(int codigo_operacion);
void* serializar_paquete(t_paquete* paquete, int bytes);
void enviar_paquete(t_paquete* paquete, int socket_cliente);
void agregar_a_paquete_datoDinamico(t_paquete* paquete, void* valor, int tamanio);
void agregar_a_paquete_datoEstatico(t_paquete* paquete, void* valor, int bytes);
void agregar_int_a_paquete(t_paquete* paquete, int valor);
void agregar_registros_a_paquete(t_paquete* paquete, registros_cpu registrosCpu);
void enviar_registros(t_paquete* paquete, registros_cpu registrosCpu, int socket);

int recibir_int(int socket);
registros_cpu recibir_registros2(int socket);
char* recibir_string(int socket);
void agregar_string_a_paquete(t_paquete* paquete, char* string);

void* recibir_buffer(int* size, int socket_cliente);
