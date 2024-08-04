#ifndef ENTSAL_KERNEL_
#define ENTSAL_KERNEL_

#include "es_gestor.h"

//Estructura de metadata archivos
typedef struct{
	char* nombre;
	int bloque_inicial;
	int tamanio_archivo;
}t_metadata;

void enviar_tipo_interfaz(int socket_kernel, char* nombre_interfaz, char* tipo_interfaz);

int gen_sleep(int pid, int valor);
int stdin_read(int pid, int registro, int tamanio);
int stdout_write(int pid, int registro, int tamanio);

int crear_metadata(char* ruta, int bloque_inicial, int tamanio_archivo);
int modificar_metadata(char* ruta, int bloque_inicial, int tamanio_archivo);
int cargar_metadata(char* ruta, t_metadata* metadata);
t_metadata* datos_metadata(char* ruta);

int obtener_archivo(char* ruta, void (*tipo_creacion)(int), void (*tipo_apertura)(int));
char* ruta_absoluta(char* ruta_relativa);
int crear_archivo(char* ruta, void (*tipo_Archivo)(int));
int abrir_archivo(char* ruta, void (*tipo_Archivo)(int));
int abrir_archivos();

t_list* cargar_archivos(char* ruta_directorio);
bool comparar_bloque_inicial(t_metadata* metadata_lista, t_metadata* metadata);
int mostrar_archivos(char* ruta_directorio);
int compactar(int pid);
void mostrar_lista_archivos(t_metadata* metadata);

void crear_bitmap(int file_descriptor);
void crear_bloques(int file_descriptor);
void abrir_bitmap(int file_descriptor);
void abrir_bloques(int file_descriptor);
int buscar_bloque_bitmap(int longitud);
int asignar_bloque_bitmap(int bloque_libre, int longitud);
int remover_bloque_bitmap(int bloque_inicial, int longitud);
int escribir_datos_bloque(char* buffer, int posicion_inicial, int cantidad);
int remover_datos_bloque(int posicion_inicial, int cantidad);

int fs_create(int pid, char* ruta);
int fs_delete(int pid, char* ruta);
int fs_truncate(int pid, char* ruta, int registro_tamanio);
int fs_write(int pid, char* ruta, int registro_direccion, int registro_tamanio, int registro_puntero);
int fs_read(int pid, char* ruta, int registro_direccion, int registro_tamanio, int registro_puntero);

void obtener_datos_filesystem();
int cantidad_bloques(int tamanio);
char* pedir_a_memoria_datos(int registro_direccion, int registro_tamanio);
int inicio_bloque(int bloque);
int enviar_a_memoria(char* datos, int registro_direccion);

#endif  /* ENTSAL_KERNEL_ */
