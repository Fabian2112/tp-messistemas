#include <../include/hello.h>

//CLIENTE


//<---------------Funciones de la cátedra--------------->
int crear_conexion(char *ip, char* puerto)
{
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	// Ahora vamos a crear el socket.
	int socket_cliente = socket(server_info->ai_family,
                         server_info->ai_socktype,
                         server_info->ai_protocol);;

	// Ahora que tenemos el socket, vamos a conectarlo
	int conn = connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen);
    while(conn == -1){
		sleep(3);
		conn = connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen);
	};
	printf("socket_cliente: %d\n", socket_cliente);
	

	freeaddrinfo(server_info);

	return socket_cliente;
}



//SERVIDOR


/*int iniciar_servidor(char* puerto, t_log* un_log, char* msj_servidor)
{
	// Quitar esta línea cuando hayamos terminado de implementar la funcion
	// assert(!"no implementado!");

	int socket_servidor;

	struct addrinfo hints, *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(NULL, puerto, &hints, &server_info);

	// Creamos el socket de escucha del servidor
    socket_servidor = socket(server_info->ai_family,
                         server_info->ai_socktype,
                         server_info->ai_protocol);
	// Asociamos el socket a un puerto
    bind(socket_servidor, server_info->ai_addr, server_info->ai_addrlen);
	// Escuchamos las conexiones entrantes
    listen(socket_servidor, SOMAXCONN);

	freeaddrinfo(server_info);
	log_info(un_log, "%s", msj_servidor);
	log_info(un_log, "El socket_servidor de Iniciar_Servidor es:%d", socket_servidor);
	return socket_servidor;
}*/
int iniciar_servidor(char* puerto, t_log* un_log, char* msj_servidor) {
    int socket_servidor;
    struct addrinfo hints, *server_info, *p;
    int yes = 1;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;       // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP
    hints.ai_flags = AI_PASSIVE;     // Usar mi dirección IP

    int rv;
    if ((rv = getaddrinfo(NULL, puerto, &hints, &server_info)) != 0) {
        log_error(un_log, "getaddrinfo: %s", gai_strerror(rv));
        return -1;
    }

    // Loop a través de todos los resultados y enlazar al primero posible
    for(p = server_info; p != NULL; p = p->ai_next) {
        if ((socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            log_error(un_log, "socket: %s", strerror(errno));
            continue;
        }

        // Reutilizar el puerto
        if (setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            log_error(un_log, "setsockopt: %s", strerror(errno));
            close(socket_servidor);
            freeaddrinfo(server_info);
            return -1;
        }

        if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) {
            log_error(un_log, "bind: %s", strerror(errno));
            close(socket_servidor);
            continue;
        }

        break; // Si llegamos aquí, el bind fue exitoso
    }

    if (p == NULL) {
        log_error(un_log, "No se pudo enlazar a ningún socket");
        freeaddrinfo(server_info);
        return -1;
    }

    freeaddrinfo(server_info);

    if (listen(socket_servidor, SOMAXCONN) == -1) {
        log_error(un_log, "listen: %s", strerror(errno));
        close(socket_servidor);
        return -1;
    }

    log_info(un_log, "%s", msj_servidor);
    log_info(un_log, "El socket_servidor de Iniciar_Servidor es: %d", socket_servidor);
    return socket_servidor;
}
int esperar_cliente(int socket_servidor, t_log* un_log, char* msj_servidor)
{
	// Quitar esta línea cuando hayamos terminado de implementar la funcion
	// assert(!"no implementado!");

	// Aceptamos un nuevo cliente
	
	int socket_cliente = accept(socket_servidor, NULL, NULL);
	log_info(un_log, "Se conecto el: %s", msj_servidor);
	return socket_cliente;
}


int recibir_operacion(int socket_cliente)
{
	int cod_op;
	if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0)
		return cod_op;
	else
	{
		close(socket_cliente);
		return -1;
	}
}

void* recibir_buffer(int* size, int socket_cliente){
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

void* serializar_paquete(t_paquete* paquete, int bytes){
	void * magic = malloc(bytes);
	int desplazamiento = 0;

	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento+= paquete->buffer->size;

	return magic;
}

t_paquete *crear_paquete(int codigo_operacion){
	t_paquete *paquete = (t_paquete *)malloc(sizeof(t_paquete));

	assert(paquete != NULL);

	paquete->codigo_operacion = codigo_operacion;
	paquete->buffer = crear_buffer();
	return paquete;
}
void enviar_paquete(t_paquete* paquete, int socket_cliente){
	int bytes = paquete->buffer->size + 2*sizeof(int);
	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
}

//--------------------Funciones creadas -----------------------------------------



t_buffer* recibir_todo_buffer(int conexion){

	t_buffer* un_buffer = malloc(sizeof(t_buffer));

	if(recv(conexion, &(un_buffer->size), sizeof(int), MSG_WAITALL)>0){
		un_buffer->stream = malloc(un_buffer->size);
		if(recv(conexion, un_buffer->stream, un_buffer->size, MSG_WAITALL) > 0){
			return un_buffer;
		}else{
			perror("Error al recibir el void* del buffer de la conexion");
			EXIT_FAILURE;
		}
	}else{
		perror("Error al recibir el tamanio de memoria del buffer de la conexion");
		EXIT_FAILURE;
	}
	return un_buffer;
}


void* extraer_choclo_del_buffer(t_buffer* un_buffer){
    
	if(un_buffer->size == 0){
		perror("Error al extraer del buffer (BUFFER VACIO)");
		exit(EXIT_FAILURE);
	}

	if(un_buffer->size < 0){
		perror("Error al extraer del buffer (BUFFER NEGATIVO (?¡?¡?))");
		exit(EXIT_FAILURE);
	}

	int size_choclo;
	memcpy(&size_choclo, un_buffer->stream, sizeof(int));
	void* choclo = malloc(size_choclo);
	memcpy(choclo, un_buffer->stream + sizeof(int), size_choclo);

	int nuevo_size = un_buffer->size - sizeof(int) - size_choclo;
	if(nuevo_size == 0){
		un_buffer->size = 0;
		free(un_buffer->stream);
		un_buffer->stream = NULL;
		return choclo;
	}
	if(nuevo_size < 0){
		perror("Error dentro del buffer extraido (TAMAÑO NEGATIVO)");
		exit(EXIT_FAILURE);
	}
	void* nuevo_stream = malloc(nuevo_size);
	memcpy(nuevo_stream, un_buffer->stream + sizeof(int) + size_choclo, nuevo_size);
	free(un_buffer->stream);
	un_buffer->size = nuevo_size;
	un_buffer->stream = nuevo_stream;

	return choclo;
}

int extraer_int_del_buffer(t_buffer* un_buffer){
	int* un_entero = extraer_choclo_del_buffer(un_buffer);
	int valor_retorno = *un_entero;
	free(un_entero);
	return valor_retorno;
}

char* extraer_string_del_buffer(t_buffer* un_buffer){
	char* un_string = extraer_choclo_del_buffer(un_buffer);
	return un_string;
}

uint32_t extraer_uint32_del_buffer(t_buffer* un_buffer){
	uint32_t* un_valor = extraer_choclo_del_buffer(un_buffer);
	uint32_t valor_retorno = *un_valor;
	free(un_valor);
	return valor_retorno;
}

//----------------------------------------------------------------------

t_buffer* crear_buffer(){
	t_buffer* un_buffer = malloc(sizeof(t_buffer));
	
	un_buffer->size = 0;
	un_buffer->stream = NULL;
    return un_buffer;
}

void destruir_buffer(t_buffer* un_buffer){
    if(un_buffer->stream != NULL){
		free(un_buffer->stream);
	}
	free(un_buffer);
}


void agregar_choclo_al_buffer(t_buffer* un_buffer, void* un_choclo, int size_choclo){
	if(un_buffer->size == 0){
		un_buffer->stream = malloc(sizeof(int) + size_choclo);
		memcpy(un_buffer->stream, &size_choclo, sizeof(int));
		memcpy(un_buffer->stream + sizeof(int), un_choclo, size_choclo);
	}else{
		un_buffer->stream = realloc(un_buffer->stream, un_buffer->size + sizeof(int) + size_choclo);
		memcpy(un_buffer->stream + un_buffer->size, &size_choclo, sizeof(int));
		memcpy(un_buffer->stream + un_buffer->size + sizeof(int), un_choclo, size_choclo);
	}

	un_buffer->size += sizeof(int);
	un_buffer->size += size_choclo;
}


void cargar_int_al_buffer(t_buffer* un_buffer, int int_value){
	agregar_choclo_al_buffer(un_buffer, &int_value, sizeof(int));
}

void cargar_uint32_al_buffer(t_buffer* un_buffer, uint32_t un_valor){
	agregar_choclo_al_buffer(un_buffer, &un_valor, sizeof(uint32_t));
}

void cargar_string_al_buffer(t_buffer* un_buffer, char* un_string){
	agregar_choclo_al_buffer(un_buffer, un_string, strlen(un_string)+1);
}

//---------------------------------------------------------------------------
/*
t_paquete* crear_paquete(){//esta función estaba sin nombre y tenía los parametros op_code codigo_operacion, t_buffer* un_buffer 
	t_paquete* un_paquete = malloc(sizeof(t_paquete));
	un_paquete->codigo_operacion = PAQUETE;
	un_paquete->buffer = crear_buffer();
	return un_paquete;
}*/
/*t_paquete* (op_code codigo_operacion, t_buffer* un_buffer ){
	t_paquete* un_paquete = malloc(sizeof(t_paquete));
	un_paquete->codigo_operacion = codigo_operacion;
	un_paquete->buffer = un_buffer;
	return un_paquete;
}*/

void destruir_paquete(t_paquete* un_paquete){
	destruir_buffer(un_paquete->buffer);
	free(un_paquete);
}



//------------------BUFFER/STREAM ------------------



int paquete_buffer(t_buffer* un_buffer, void* stream_agregar, int size, int desplazamiento){
	void* un_stream = realloc(un_buffer->stream, un_buffer->size+size);
	un_buffer->stream = un_stream;
	memcpy(un_buffer->stream + un_buffer->size, stream_agregar, size);
	un_buffer->size += size;
	return size+desplazamiento;
}

int paquete_buffer_string(t_buffer* un_buffer, void* string_agregar, int desplazamiento){
	uint32_t length = strlen(string_agregar)+1;
	desplazamiento = paquete_buffer(un_buffer, &length, sizeof(length), desplazamiento);
	desplazamiento = paquete_buffer(un_buffer, string_agregar, length *sizeof(char), desplazamiento);
	return desplazamiento + sizeof(length) + sizeof(string_agregar);
}



void enviar_stream(int socket_enviar, void* stream_enviar, int tamanio_buffer) {
    //uint8_t opcion_funcion = 0; no se utiliza 	
    //uint32_t size = 0; no lo utilizamos 

    ssize_t bytes_enviar = send(socket_enviar, stream_enviar, tamanio_buffer, 0);
    if (bytes_enviar == -1) {
        printf("Error en el envío del buffer [%s]\n", strerror(errno));
    }
}

void* crear_stream(uint8_t opcion_funcion, t_buffer* un_buffer, int bytes){

	void* stream_enviar = malloc(bytes);
	int desplazamiento = 0;

	memcpy(stream_enviar + desplazamiento, &opcion_funcion, sizeof(opcion_funcion));
	desplazamiento += sizeof(opcion_funcion);
	memcpy(stream_enviar + desplazamiento, &(un_buffer->size), sizeof(un_buffer->size));
	desplazamiento += sizeof(un_buffer->size);
	memcpy(stream_enviar + desplazamiento, un_buffer->stream, un_buffer->size);
	
	return stream_enviar;
}

void enviar_stream_buffer(int socket_enviar, uint8_t opcion_funcion, t_buffer* un_buffer){
	int bytes = un_buffer->size + sizeof(opcion_funcion) + sizeof(un_buffer->size);
	void* stream = crear_stream(opcion_funcion, un_buffer, bytes);
	enviar_stream(socket_enviar, stream, bytes);
	free(stream);
}



void recibir_stream_buffer(int socket_recibir, t_buffer* buffer_recibido) {
    ssize_t bytes_mensaje = recv(socket_recibir, &(buffer_recibido->size), sizeof(buffer_recibido->size), 0);
    if (bytes_mensaje <= 0) {
        if (bytes_mensaje == 0) {
            printf("La conexión ha sido cerrada por el otro extremo\n");
        } else {
            printf("Error en la recepción del buffer [%s]\n", strerror(errno));
        }
        exit(-1);
    } else if (buffer_recibido->size > 0) {
        buffer_recibido->stream = malloc(buffer_recibido->size);
        if (buffer_recibido->stream == NULL) {
            printf("Error en la asignación de memoria\n");
            exit(-1);
        }
        recv(socket_recibir, buffer_recibido->stream, buffer_recibido->size, 0);
    }
}


uint8_t recibir_stream_funcion(int socket_recibir) {
    uint8_t opcion_funcion;
    ssize_t bytes_mensaje = recv(socket_recibir, &opcion_funcion, sizeof(opcion_funcion), 0);
    if (bytes_mensaje == -1) {
        printf("Error en la recepción del header [%s]\n", strerror(errno));
    }
    return opcion_funcion;
}


int desempaquetar_buffer(t_buffer* un_buffer, void* data_destino, int size, int desplazamiento) {
    if (un_buffer->stream == NULL || un_buffer->size == 0) {
        puts("Error en el desempaquetado del buffer\n");
        exit(-1);
    }
    memcpy(data_destino, un_buffer->stream + desplazamiento, size);
    return desplazamiento + size;
}


char* desempaquetar_buffer_string(t_buffer* un_buffer, int *desplazamiento) {
	uint32_t tamanio;
    *desplazamiento = desempaquetar_buffer(un_buffer, &tamanio, sizeof(tamanio), *desplazamiento);
    char *string = malloc(tamanio * sizeof(char));
    *desplazamiento = desempaquetar_buffer(un_buffer, string, tamanio * sizeof(char), *desplazamiento);
    return string;
}


t_list* desempaquetar_buffer_list_char(t_buffer *buffer, int *desplazamiento) {
	t_list *lista_nueva = list_create();
	int tamanioLista;
	*desplazamiento = desempaquetar_buffer(buffer, &tamanioLista, sizeof(tamanioLista), *desplazamiento);
	if (tamanioLista != -1) {
		void *elem = NULL;
		elem = desempaquetar_buffer_string(buffer, desplazamiento);
		list_add(lista_nueva, elem);
		}
		return lista_nueva;
	}


//------------------CPU - MEMORIA -------------------


void enviar_cpu_memoria(memoria_cpu_data* data, int socket_memoria, opcion_funciones opcion_funcion){
	t_buffer* buffer_enviar = crear_buffer();
	int desplazamiento = 0;
	desplazamiento = paquete_buffer(buffer_enviar, &data->programCounter, sizeof(int), desplazamiento);
	desplazamiento = paquete_buffer_string(buffer_enviar, &data->parametro_1, desplazamiento);
	desplazamiento = paquete_buffer_string(buffer_enviar, &data->parametro_2, desplazamiento);
	desplazamiento = paquete_buffer_string(buffer_enviar, &data->parametro_3, desplazamiento);
	desplazamiento = paquete_buffer(buffer_enviar, &data->pid, sizeof(int), desplazamiento);
	desplazamiento = paquete_buffer(buffer_enviar, &data->direccion, sizeof(int), desplazamiento);
	desplazamiento = paquete_buffer(buffer_enviar, &data->registro_valor, sizeof(uint32_t), desplazamiento);
	enviar_stream_buffer(socket_memoria, opcion_funcion, buffer_enviar);

	destruir_buffer(buffer_enviar);

}

memoria_cpu_data* recibir_memoria_cpu(int socket_memoria){
    t_buffer* buffer_enviar =  crear_buffer();
    memoria_cpu_data* data = malloc(sizeof(memoria_cpu_data));
    data->flag = recibir_stream_funcion(socket_memoria);
    recibir_stream_buffer(socket_memoria, buffer_enviar);
    int desplazamiento = 0;
    desplazamiento = desempaquetar_buffer(buffer_enviar, &(data->programCounter), sizeof(int), desplazamiento);
    data->parametro_1 = desempaquetar_buffer_string(buffer_enviar, &desplazamiento);
    data->parametro_2 = desempaquetar_buffer_string(buffer_enviar, &desplazamiento);
    data->parametro_3 = desempaquetar_buffer_string(buffer_enviar, &desplazamiento);
    desplazamiento = desempaquetar_buffer(buffer_enviar, &(data->pid), sizeof(int), desplazamiento);
    desplazamiento = desempaquetar_buffer(buffer_enviar, &(data->direccion), sizeof(int), desplazamiento);
    desplazamiento = desempaquetar_buffer(buffer_enviar, &(data->registro_valor), sizeof(uint32_t), desplazamiento);
    destruir_buffer(buffer_enviar);
    return data;
}



//------------------CPU - KERNEL d -------------------

/*
void enviar_cpu_kernel(t_pcb* pcb, int fd_kernel_dispatch, kernel_cpu_dato operacion){
   t_buffer* buffer_enviar =  crear_buffer();
    int desplazamiento = 0;
	
	int aux = pcb->pid;    
    desplazamiento = paquete_buffer(buffer_enviar, &aux, sizeof(int), desplazamiento);
    aux = pcb->programCounter;
    desplazamiento = paquete_buffer(buffer_enviar, &aux, sizeof(int), desplazamiento);
	aux = pcb->quantum;
    desplazamiento = paquete_buffer(buffer_enviar, &aux, sizeof(int), desplazamiento);
    
	uint8_t aux2 =  pcb->registros_cpu->AX;
    desplazamiento = paquete_buffer(buffer_enviar, &aux2, sizeof(uint8_t), desplazamiento);
    aux2 = pcb->registros_cpu->BX;
    desplazamiento = paquete_buffer(buffer_enviar, &aux2, sizeof(uint8_t), desplazamiento);
    aux2 = pcb->registros_cpu->CX;
    desplazamiento = paquete_buffer(buffer_enviar, &aux2, sizeof(uint8_t), desplazamiento);
    aux2 = pcb->registros_cpu->DX;
    desplazamiento = paquete_buffer(buffer_enviar, &aux2, sizeof(uint8_t), desplazamiento);
	
	uint32_t aux3 =  pcb->registros_cpu->PC;
    desplazamiento = paquete_buffer(buffer_enviar, &aux3, sizeof(uint32_t), desplazamiento);
	aux3 =  pcb->registros_cpu->EAX;
    desplazamiento = paquete_buffer(buffer_enviar, &aux3, sizeof(uint32_t), desplazamiento);
    aux3 = pcb->registros_cpu->EBX;
    desplazamiento = paquete_buffer(buffer_enviar, &aux3, sizeof(uint32_t), desplazamiento);
    aux3 = pcb->registros_cpu->ECX;
    desplazamiento = paquete_buffer(buffer_enviar, &aux3, sizeof(uint32_t), desplazamiento);
    aux3 = pcb->registros_cpu->EDX;
    desplazamiento = paquete_buffer(buffer_enviar, &aux3, sizeof(uint32_t), desplazamiento);
	aux3 = pcb->registros_cpu->SI;
    desplazamiento = paquete_buffer(buffer_enviar, &aux3, sizeof(uint32_t), desplazamiento);
	aux3 = pcb->registros_cpu->DI;
    desplazamiento = paquete_buffer(buffer_enviar, &aux3, sizeof(uint32_t), desplazamiento);
	
    enviar_stream_buffer(fd_kernel_dispatch, operacion, buffer_enviar);
    destruir_buffer(buffer_enviar);
}


t_pcb* recibir_cpu_kernel(int fd_kernel_dispatch){
    t_buffer* buffer_recibido =  crear_buffer(); //buffer_create no existe
   
    kernel_cpu_dato flag_recibido = recibir_stream_funcion(fd_kernel_dispatch);
    
    recibir_stream_buffer(fd_kernel_dispatch, buffer_recibido);
    
    t_pcb* pcb1 = malloc(sizeof(t_pcb));
    int desplazamiento = 0;
    pcb1->registros_cpu = malloc(sizeof(registros_cpu));
    int aux;
    desplazamiento = desempaquetar_buffer(buffer_recibido, &aux, sizeof(int), desplazamiento);
    pcb1->pid = aux;
    desplazamiento = desempaquetar_buffer(buffer_recibido, &pcb1->programCounter, sizeof(int), desplazamiento);
    desplazamiento = desempaquetar_buffer(buffer_recibido, &pcb1->registros_cpu->AX, sizeof(uint8_t), desplazamiento);
    desplazamiento = desempaquetar_buffer(buffer_recibido, &pcb1->registros_cpu->BX, sizeof(uint8_t), desplazamiento);
    desplazamiento = desempaquetar_buffer(buffer_recibido, &pcb1->registros_cpu->CX, sizeof(uint8_t), desplazamiento);
    desplazamiento = desempaquetar_buffer(buffer_recibido, &pcb1->registros_cpu->DX, sizeof(uint8_t), desplazamiento);
	desplazamiento = desempaquetar_buffer(buffer_recibido, &pcb1->registros_cpu->PC, sizeof(uint32_t), desplazamiento);
    desplazamiento = desempaquetar_buffer(buffer_recibido, &pcb1->registros_cpu->EAX, sizeof(uint32_t), desplazamiento);
    desplazamiento = desempaquetar_buffer(buffer_recibido, &pcb1->registros_cpu->EBX, sizeof(uint32_t), desplazamiento);
    desplazamiento = desempaquetar_buffer(buffer_recibido, &pcb1->registros_cpu->ECX, sizeof(uint32_t), desplazamiento);
	desplazamiento = desempaquetar_buffer(buffer_recibido, &pcb1->registros_cpu->EDX, sizeof(uint32_t), desplazamiento);
    desplazamiento = desempaquetar_buffer(buffer_recibido, &pcb1->registros_cpu->SI, sizeof(uint32_t), desplazamiento);
    desplazamiento = desempaquetar_buffer(buffer_recibido, &pcb1->registros_cpu->DI, sizeof(uint32_t), desplazamiento);
    pcb1->recurso = desempaquetar_buffer_string(buffer_recibido, &desplazamiento);
    pcb1->recurso_solicitado = desempaquetar_buffer_string(buffer_recibido, &desplazamiento);
    pcb1->recurso_utilizado = desempaquetar_buffer_list_char(buffer_recibido, &desplazamiento);
    //pcb1->estado = desempaquetar_buffer_string(buffer_recibido, &desplazamiento); //estás asignandole un char a un tipo  enum, deberías castearlo antes 
    desplazamiento = desempaquetar_buffer(buffer_recibido, &pcb1->sleep, sizeof(pcb1->sleep), desplazamiento);
    destruir_buffer(buffer_recibido);
    pcb1->flag = flag_recibido;
    return pcb1;
}*/


//--------------- MEMORIA - CPU -----------------

void enviar_memoria_cpu(memoria_cpu_data* data, int socket_CPU, opcion_funciones instruccion){
    t_buffer* buffer_enviar =  crear_buffer();
    int desplazamiento = 0;
    desplazamiento = paquete_buffer(buffer_enviar, &data->programCounter, sizeof(int), desplazamiento);
    desplazamiento = paquete_buffer_string(buffer_enviar, data->parametro_1, desplazamiento);
    desplazamiento = paquete_buffer_string(buffer_enviar, data->parametro_2, desplazamiento);
    desplazamiento = paquete_buffer_string(buffer_enviar, data->parametro_3, desplazamiento);
    desplazamiento = paquete_buffer(buffer_enviar, &data->pid, sizeof(int), desplazamiento);
    desplazamiento = paquete_buffer(buffer_enviar, &data->direccion, sizeof(int), desplazamiento);
    desplazamiento = paquete_buffer(buffer_enviar, &data->registro_valor, sizeof(uint32_t), desplazamiento);
    enviar_stream_buffer(socket_CPU, instruccion, buffer_enviar);
    destruir_buffer(buffer_enviar);
}



//------------ KERNEL - MEMORIA -------------

kernel_memoria_data* recibir_kernel_memoria(int socket_kernel){
    t_buffer* buffer_enviado =  crear_buffer(); //estaba como buffer_create pero nadie definio esa funciono, así que la cambié a la más valida 
    kernel_memoria_data* data = malloc(sizeof(kernel_memoria_data));
    data->flag = recibir_stream_funcion(socket_kernel);
    recibir_stream_buffer(socket_kernel, buffer_enviado);
    int desplazamiento = 0;
    desplazamiento = desempaquetar_buffer(buffer_enviado, &data->pagina, sizeof(int), desplazamiento);
    desplazamiento = desempaquetar_buffer(buffer_enviado, &data->pid, sizeof(int), desplazamiento);
    desplazamiento = desempaquetar_buffer(buffer_enviado, &data->size, sizeof(int), desplazamiento);
    data->nombre = desempaquetar_buffer_string(buffer_enviado, &desplazamiento);
    destruir_buffer(buffer_enviado);
    return data;
}

void enviar_kernel_memoria(kernel_memoria_data* data, int socket, opcion_funciones flag){
    t_buffer* bufferEnviar =  crear_buffer();
    int desplazamiento = 0;
    desplazamiento = paquete_buffer(bufferEnviar, &data->pagina, sizeof(int), desplazamiento);
    desplazamiento = paquete_buffer(bufferEnviar, &data->pid, sizeof(int), desplazamiento);
    desplazamiento = paquete_buffer(bufferEnviar, &data->size, sizeof(int), desplazamiento);
    desplazamiento = paquete_buffer_string(bufferEnviar, data->nombre, desplazamiento);
    enviar_stream_buffer(socket, flag, bufferEnviar);
    destruir_buffer(bufferEnviar); 
    free(data);
}


//Funciones que agrego de mi antiguo proyecto

void eliminar_paquete(t_paquete* paquete){
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio){
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio + sizeof(int));

	memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio, sizeof(int));
	memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), valor, tamanio);

	paquete->buffer->size += tamanio + sizeof(int);
}
/*
void enviarPCB(t_paquete* paquete, t_pcb* pcb, int socket){
	//agregar_a_paquete(paquete, &(pcb->pid), sizeof(int));
	//agregar_a_paquete(paquete, &(pcb->programCounter), sizeof(int));
	preparar_registros(paquete,pcb->registros_cpu);
	enviar_paquete(paquete, socket);
	eliminar_paquete(paquete);
}

void preparar_registros(t_paquete *paquete, registros_cpu* registros_cpu)
{
 	agregar_a_paquete(paquete, &(registros_cpu->AX), sizeof(uint8_t));
	agregar_a_paquete(paquete, &(registros_cpu->BX), sizeof(uint8_t));
	agregar_a_paquete(paquete, &(registros_cpu->CX), sizeof(uint8_t));
	agregar_a_paquete(paquete, &(registros_cpu->DX), sizeof(uint8_t));
	agregar_a_paquete(paquete, &(registros_cpu->EAX), sizeof(uint32_t));
	agregar_a_paquete(paquete, &(registros_cpu->EBX), sizeof(uint32_t));
	agregar_a_paquete(paquete, &(registros_cpu->ECX), sizeof(uint32_t));
	agregar_a_paquete(paquete, &(registros_cpu->EDX), sizeof(uint32_t));
	agregar_a_paquete(paquete, &(registros_cpu->SI), sizeof(uint32_t));
	agregar_a_paquete(paquete, &(registros_cpu->PC), sizeof(uint32_t));
	agregar_a_paquete(paquete, &(registros_cpu->DI), sizeof(uint32_t));
}
*/
/*
t_pcb* recibirPCB(int socket){
	t_list* valores_recibidos = recibir_paquete(socket);

    t_pcb* pcb_recibido = malloc(sizeof(t_pcb));
   
	pcb_recibido->quantum = *((int*)list_get(valores_recibidos, 1)); //chequear esto porque seguramente empezaría desde cero ya que pensaba que el pid sería el cero
	pcb_recibido->programCounter = *((int*)list_get(valores_recibidos, 2));
	recibir_registros(valores_recibidos,pcb_recibido);
	return pcb_recibido;
}*/

const char* traducir_estado(t_estado estado){ //estaba como enum t_estado pero lo cambie porque se tenía incomplete type
	//CAMBIÉ el retorno de está funciń de char** a char* porque solo se devuelve un char, si se necesita que se devuelva un char** habrá que cambiar lo que retorna 
	switch(estado){
		case NEW: return "NEW";
			
		case READY: return "READY";
			
		case BLOCKED: return "BLOCKED";
			
		case RUNNING: return "RUNNING";
			
		case EXIT: return "EXIT"; 
			default: return "UKNOWM" ;
				
	}
}

t_list* recibir_paquete(int socket_cliente){
	int size;
	int desplazamiento = 0;
	void * buffer;
	t_list* valores = list_create();
	int tamanio;

	buffer = recibir_buffer(&size, socket_cliente);
	while(desplazamiento < size){
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		desplazamiento+=sizeof(int);
		char* valor = malloc(tamanio);
		memcpy(valor, buffer+desplazamiento, tamanio);
		desplazamiento+=tamanio;
		list_add(valores, valor);
	}
	free(buffer);
	return valores;
}

/*
void recibir_registros(t_list* lista, t_pcb* pcb_recibido)
{
	pcb_recibido->registros_cpu->AX = *((uint8_t*)list_get(lista, 3));
	pcb_recibido->registros_cpu->BX = *((uint8_t*)list_get(lista, 4));
	pcb_recibido->registros_cpu->CX = *((uint8_t*)list_get(lista, 5));
	pcb_recibido->registros_cpu->DX = *((uint8_t*)list_get(lista, 6));
	pcb_recibido->registros_cpu->EAX = *((uint32_t*)list_get(lista, 7));
	pcb_recibido->registros_cpu->EBX = *((uint32_t*)list_get(lista, 8));
	pcb_recibido->registros_cpu->ECX = *((uint32_t*)list_get(lista, 9));
	pcb_recibido->registros_cpu->EDX = *((uint32_t*)list_get(lista, 10));
	pcb_recibido->registros_cpu->SI = *((uint32_t*)list_get(lista, 11));
	pcb_recibido->registros_cpu->DI = *((uint32_t*)list_get(lista, 12));
}
*/
void mostrarPCB(t_pcb* proceso)
{
	printf("PID: %d\n", proceso->pid);
    			printf("Quantum Restante: %d\n", proceso->quantum);
    			printf("Program Counter: %d\n", proceso->programCounter);
   				printf("Register CPU:\n");
				//CHEQUEAR LOS TIPOS DE DATOS
    			printf("  EAX: %d\n", proceso->registros_cpu.EAX);
    			printf("  EBX: %d\n", proceso->registros_cpu.EBX);
    			printf("  ECX: %d\n", proceso->registros_cpu.ECX);
    			printf("  EDX: %d\n", proceso->registros_cpu.EDX);
				printf("  AX: %d\n", proceso->registros_cpu.AX);
				printf("  BX: %d\n", proceso->registros_cpu.BX);
				printf("  CX: %d\n", proceso->registros_cpu.DX);
				
				printf("  DX: %d\n", proceso->registros_cpu.PC);
				printf("  BX: %d\n", proceso->registros_cpu.SI);
				printf("  CX: %d\n", proceso->registros_cpu.DI);
				
}
