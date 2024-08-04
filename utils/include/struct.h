#ifndef STRUCT_H_
#define STRUCT_H_
#include <commons/temporal.h>
// Estructuras del cliente del TP0
typedef enum
{
	MENSAJE,
	PAQUETE,
	// IREMOS COLOCANDO MAS VARIABLES SEGUN NECESITEMOS
	// PODEMOS SEPARAR SEGUN MENSAJES ENTRE DISTINTOS MODULOS O
	// CREAR ENUMS DISTINTOS PARA CADA MODULOS

	// OP_CODES KERNEL -> MODULO DESEADO
	CREAR_PROCESO_KERNEL_MEMORIA,
	INICIAR_PROCESO_KERNEL,
	ELIMINAR_PROCESO_KERNEL_MEMORIA,
	//
	SOLICITUD_INSTRUCCION,
	INSTRUCCION_ENVIADA_CORRECTAMENTE,
	TIPO_INTERFAZ,
	MONITOR,
	LEER_MEMORIA,
	ESCRIBIR_MEMORIA,
	GENERICA,
	TECLADO,
	DIAL_FS,
	OBTENER_MARCO,
	OBTENER_DATO_MARCO,
	OUT_OF_MEMORY,
	KERNEL,
	MEMORIA,
	CPU,
	ENTRADASALIDA,
	STDIN,
	STDOUT,
	KERNEL_INTERRUPT,
	KERNEL_DISPATCH,
	CPU_DISPATCH,
	CPU_INTERRUPT
} op_code;

typedef enum
{
	NEW,
	READY,
	RUNNING,
	BLOCKED,
	EXIT
} t_estado;

typedef struct
{
	int size;
	void *stream;
} t_buffer;

typedef struct
{
	op_code codigo_operacion;
	t_buffer *buffer;
} t_paquete;

typedef enum
{
	SET,
	MOV_IN,
	MOV_OUT,
	SUM,
	SUB,
	JNZ,
	RESIZE,
	COPY_STRING,
	WAIT,
	SIGNAL,
	IO_GEN_SLEEP,
	IO_STDIN_READ,
	IO_STDOUT_WRITE,
	IO_FS_CREATE,
	IO_FS_DELETE,
	IO_FS_TRUNCATE,
	IO_FS_WRITE,
	IO_FS_READ,
	EXIT_INSTRUCCTION, // Estaba como EXIT pero lo cambié porque la consola indica redeclaration of EXIT
} t_identificador;

typedef enum
{
	INICIAR_PROCESO_CPU,
	FINALIZAR_PROCESO,
	ACCESO_A_TABLA,
} opcion_funciones;

typedef enum
{
	EXECUTE_CPU,
	INTERRUPT_CPU_EXITO,
	INTERRUPT_CPU_FALLO,
	DESALOJAR_PCB,
	i_SET,
	i_MOV_IN,
	i_MOV_OUT,
	i_SUM,
	i_SUB,
	i_JNZ,
	i_RESIZE,
	i_COPY_STRING,
	i_WAIT,
	i_SIGNAL,
	i_IO_GEN_SLEEP,
	i_IO_STDIN_READ,
	i_IO_STDOUT_WRITE,
	i_IO_FS_CREATE,
	i_IO_FS_DELETE,
	i_IO_FS_TRUNCATE,
	i_IO_FS_WRITE,
	i_IO_FS_READ,
	i_EXIT,
} kernel_cpu_dato;

typedef struct
{
	uint8_t AX, BX, CX, DX;
	uint32_t PC, EAX, EBX, ECX, EDX, SI, DI;
} registros_cpu;

typedef struct
{
	char *nombre;
	int size;
	int pid; // el pid asociado a espacio de memoria
	int pagina;
	opcion_funciones flag;
} kernel_memoria_data;

typedef struct
{
	int pid;
	t_estado estado;
	int programCounter;
	int quantum;
	int sleep;
	char *recurso;
	char *recurso_solicitado;
	t_list *recurso_utilizado;
	t_list *tabla_archivos;
	registros_cpu registros_cpu;
	kernel_cpu_dato flag;
} t_pcb;

typedef struct
{
	t_identificador identificador;
	t_list *parametros;
	char *parametro_1;
	char *parametro_2;
	char *parametro_3;
	int cant_params;
} t_instruccion;

typedef struct
{
	int pid;
	int programCounter;
	char *parametro_1;
	char *parametro_2;
	char *parametro_3;
	int direccion;
	uint32_t registro_valor;
	opcion_funciones flag;
} memoria_cpu_data;

typedef struct 
{
	int pid;
	char *path_proceso;
	t_list *tabla_paginas;
} kernel_memoria_process;

typedef struct
{
	int pid;
	int pagina;
	int marco;
	t_temporal *time;
} TLB;

typedef struct
{
	int nroPagina;
	int desplazamiento;
} pagina_desplazamiento;

typedef struct fila_tabla_paginas
{
	int pagina;
	int marco;
} fila_tabla_paginas;

typedef struct {
	char* nombre; 
	int fd_socket;
	bool  atendido;
}interfaz_fd;
#endif /* STRUCT_H_ */
