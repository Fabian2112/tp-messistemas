#include <../include/entsal_kernel.h>

void atender_entsal_kernel(){
	
	log_error(entradaSalida_logger, "Entrando a atender_entsal_kernel");
    fd_kernel=-1;//para que se quede esperando la conexion
    while(fd_kernel<=0)
    {
        log_error(entradaSalida_logger, "Conectando a kernel...");
        fd_kernel= crear_conexion(ioconfig->IP_KERNEL, ioconfig->PUERTO_KERNEL);
        if((fd_kernel)<=0) log_error(entradaSalida_logger, "Intentando conectar a kernel");

        sleep(1);
    }
	
	enviar_tipo_interfaz(fd_kernel, "GENERICA", ioconfig->TIPO_INTERFAZ); //Estoy hay que cambiarlo por argv[2] cuando se activen los modulos
    bool control_key = 1;
    t_buffer* buffer;

    //Esta funcion es para levantar los archivos de bitmap.dat y bloques.dat si se trata de un filesystem
    if(strcmp(ioconfig->TIPO_INTERFAZ, "FS") == 0 ){ //Tengo que liberarlo luego, cuando finaliza con munmap()
    	obtener_archivo("bloques.dat", (void*) crear_bloques, (void*) abrir_bloques);
    	obtener_archivo("bitmap.dat", (void*) crear_bitmap, (void*) abrir_bitmap);
    }

    int pid;
    char* nombre_archivo = malloc(sizeof(char));
    int registro_direccion;
    int registro_tamanio;
    int registro_puntero;

    while (control_key) {
		int cod_op = recibir_operacion(fd_kernel);
		recibir_int(fd_kernel);
		log_error(entradaSalida_logger, "??????????");

		switch (cod_op) {
            case IO_GEN_SLEEP:
					log_error(entradaSalida_logger, "me meti con generica, la voy a quedar");
            		//buffer = recibir_todo_buffer(fd_kernel);
					pid = recibir_int(fd_kernel);
            		//pid = extraer_int_del_buffer(buffer);
            		int valor = recibir_int(fd_kernel);//extraer_int_del_buffer(buffer);

            		gen_sleep(pid, valor);
            	break;
            case IO_STDIN_READ:
            		buffer = recibir_todo_buffer(fd_kernel);
            		//En estas funciones vamos a necesitar saber que tipo se manda uint_8, etc
            		pid = extraer_int_del_buffer(buffer);
            		registro_direccion = extraer_int_del_buffer(buffer);
            		registro_tamanio = extraer_int_del_buffer(buffer);

            		stdin_read(pid, registro_direccion, registro_tamanio);
            	break;
            case IO_STDOUT_WRITE: //debemos cambiarles el nombre, pero no queria tocar un archivo en comun, se deja para las pruebas finales
            		buffer = recibir_todo_buffer(fd_kernel);
            		//En estas funciones vamos a necesitar saber que tipo se manda uint_8, etc
            		pid = extraer_int_del_buffer(buffer);
            		registro_direccion = extraer_int_del_buffer(buffer);
            		registro_tamanio = extraer_int_del_buffer(buffer);

            		stdout_write(pid, registro_direccion, registro_tamanio);
            	break;
    		case IO_FS_CREATE:
        			buffer = recibir_todo_buffer(fd_kernel);
        			pid = extraer_int_del_buffer(buffer);
        			nombre_archivo = extraer_string_del_buffer(buffer);

					log_info(entradaSalida_logger, "PID: <%i> - Operacion: <DIALFS>", pid);
    				fs_create(pid, nombre_archivo); //Esto va a debolver un int, pero hay qe ver los protocolos
    			break;
    		case IO_FS_DELETE:
    				buffer = recibir_todo_buffer(fd_kernel);
    				pid = extraer_int_del_buffer(buffer);
    				nombre_archivo = extraer_string_del_buffer(buffer);

					log_info(entradaSalida_logger, "PID: <%i> - Operacion: <DIALFS>", pid);
    				fs_delete(pid, nombre_archivo);
    			break;
    		case IO_FS_TRUNCATE:
    				buffer = recibir_todo_buffer(fd_kernel);
    				pid = extraer_int_del_buffer(buffer);
    				nombre_archivo = extraer_string_del_buffer(buffer);
    				registro_tamanio = extraer_int_del_buffer(buffer);

				log_info(entradaSalida_logger, "PID: <%i> - Operacion: <DIALFS>", pid);
    				fs_truncate(pid, nombre_archivo, registro_tamanio);
    			break;
    		case IO_FS_WRITE:
    				buffer = recibir_todo_buffer(fd_kernel);
    				pid = extraer_int_del_buffer(buffer);
    				nombre_archivo = extraer_string_del_buffer(buffer);
    				registro_direccion = extraer_int_del_buffer(buffer);
    				registro_tamanio = extraer_int_del_buffer(buffer);
    				registro_puntero = extraer_int_del_buffer(buffer);

					log_info(entradaSalida_logger, "PID: <%i> - Operacion: <DIALFS>", pid);
    				fs_write(pid, nombre_archivo, registro_direccion, registro_tamanio, registro_puntero);
    			break;
    		case IO_FS_READ:
    				buffer = recibir_todo_buffer(fd_kernel);
    			    pid = extraer_int_del_buffer(buffer);
    			    nombre_archivo = extraer_string_del_buffer(buffer);
    			    registro_direccion = extraer_int_del_buffer(buffer);
    			    registro_tamanio = extraer_int_del_buffer(buffer);
    			    registro_puntero = extraer_int_del_buffer(buffer);

					log_info(entradaSalida_logger, "PID: <%i> - Operacion: <DIALFS>", pid);
    				fs_read(pid, nombre_archivo, registro_direccion, registro_tamanio, registro_puntero);
    			break;
            case -1:
                log_error(entradaSalida_logger, "El KERNEL se desconecto. Terminando servidor");
                control_key = 0;
                // return EXIT_FAILURE;
                break;
            default:
                log_warning(entradaSalida_logger,"Operacion desconocida de KERNEL");
                break;
            }
	}
}

int gen_sleep(int pid, int valor){
	log_info(entradaSalida_logger, "PID: <%i> - Operacion: <IO_GEN_SLEEP>", pid); //Operacion deberia tomarse parametro, nose...

	usleep(valor * ((ioconfig->TIEMPO_UNIDAD_TRABAJO)*1000));

	return EXIT_SUCCESS;
}

int stdin_read(int pid, int registro, int tamanio){
	log_info(entradaSalida_logger, "PID: <%i> - Operacion: <IO_STDIN_READ>", pid); //Operacion deberia tomarse parametro

	char* teclado = malloc(sizeof(char));
	char* datos = malloc(sizeof(char));
	printf("Ingrese un texto: ");
	scanf("%s", teclado);
	strncpy(datos, teclado, tamanio);

	enviar_a_memoria(datos, registro); //Esta comentado para pruebas

	return EXIT_SUCCESS;
}

int stdout_write(int pid, int registro, int tamanio){
	log_info(entradaSalida_logger, "PID: <%i> - Operacion: <IO_STDOUT_WRITE>", pid); //Operacion deberia tomarse parametro

	char* datos = pedir_a_memoria_datos(registro, tamanio); //Esta comentado para pruebas

	if(!datos){
		return EXIT_FAILURE;
	}
	printf("Contenido de direccion de memoria (%s)", datos); //Esto solo es de ayuda

	return EXIT_SUCCESS;
}

//De aca para abajo DIALFS

int fs_create(int pid, char* ruta){
	log_info(entradaSalida_logger, "PID: <%i> - Crear Archivo <%s>", pid, ruta);

	int bloque_libre;
	int tamanio = 0;

	if((bloque_libre = buscar_bloque_bitmap(cantidad_bloques(tamanio))) == -1){
		printf("No se puede encontrar un bloque libre\n");
		return EXIT_FAILURE;
	}

	asignar_bloque_bitmap(bloque_libre, cantidad_bloques(tamanio));

	crear_metadata(ruta, bloque_libre, tamanio);

	return EXIT_SUCCESS;
}

int fs_delete(int pid, char* ruta){
	log_info(entradaSalida_logger, "PID: <%i> - Eliminar Archivo: <%s>", pid, ruta);

	t_metadata* metadata = datos_metadata(ruta);

	remover_bloque_bitmap(metadata->bloque_inicial, cantidad_bloques(metadata->tamanio_archivo));
	//Aqui debo borrar datos del bloques.dat
	remover_datos_bloque(inicio_bloque(metadata->bloque_inicial), metadata->tamanio_archivo);

	char* ruta_archivo = ruta_absoluta(ruta);
	remove(ruta_archivo);

	free(metadata);
	free(ruta_archivo);

	return EXIT_SUCCESS;
}

int fs_truncate(int pid, char* ruta, int registro_tamanio){
	log_info(entradaSalida_logger, "PID: <%i> - Truncar Archivo: <%s> - Tamaño: <%i>", pid, ruta, registro_tamanio);
	t_metadata* metadata = datos_metadata(ruta);

	int tamanio_anterior = metadata->tamanio_archivo;
	int cantidad_bloques_actual = cantidad_bloques(metadata->tamanio_archivo);
	int cantidad_nueva_bloques = cantidad_bloques(registro_tamanio);
	int nuevo_tamanio = registro_tamanio;

	int diferencia_bloques = cantidad_nueva_bloques - cantidad_bloques_actual;

	int ultimo_bloque = cantidad_bloques_actual + metadata->bloque_inicial - 1;
	if(diferencia_bloques > 0){ //Quiere decir que el archivo se va a expandir
		int nuevo_bloque;
		if((nuevo_bloque = buscar_bloque_bitmap(diferencia_bloques)) == -1){

			compactar(pid);

			free(metadata);

			t_metadata* metadata_compactacion_1 = datos_metadata(ruta);
			ultimo_bloque = cantidad_bloques_actual + metadata_compactacion_1->bloque_inicial - 1;

			//Como compacto busco un nuevo lugar
			if((nuevo_bloque = buscar_bloque_bitmap(cantidad_bloques(cantidad_nueva_bloques))) == -1){
				printf("No hay espacio en memoria!!! debe esperar que se libere la memoria\n!!!");
				free(metadata_compactacion_1);
				return EXIT_FAILURE;
			}else{
				asignar_bloque_bitmap(nuevo_bloque, cantidad_bloques(nuevo_tamanio));
				//Primero copio lo que estaba en los bloques anteriores al inicio del nuevo bloque
				escribir_datos_bloque(buffer_bloques + inicio_bloque(metadata_compactacion_1->bloque_inicial), inicio_bloque(nuevo_bloque), metadata_compactacion_1->tamanio_archivo);
				//Ahora limpio el espacio donde estaba el bloque antes
				remover_datos_bloque(inicio_bloque(metadata_compactacion_1->bloque_inicial), tamanio_anterior);
				remover_bloque_bitmap(metadata_compactacion_1->bloque_inicial, cantidad_bloques_actual);

				modificar_metadata(ruta, nuevo_bloque, nuevo_tamanio);
				free(metadata_compactacion_1);
			}
		}else{
			if((nuevo_bloque - ultimo_bloque) == 1){//Los bloques son contiguos
				asignar_bloque_bitmap(nuevo_bloque, diferencia_bloques);
				modificar_metadata(ruta, metadata->bloque_inicial, nuevo_tamanio);
			}
			else{

				compactar(pid);

				t_metadata* metadata_compactacion_2 = datos_metadata(ruta);
				ultimo_bloque = cantidad_bloques_actual + metadata_compactacion_2->bloque_inicial - 1;

					//Como compacto debo buscar todo el nuevo tamano completo
				if((nuevo_bloque = buscar_bloque_bitmap(cantidad_bloques(nuevo_tamanio))) == -1){
					printf("No hay espacio en memoria!!! debe esperar que se libere la memoria!!!\n");
					free(metadata_compactacion_2);
					return EXIT_FAILURE;
				}else{
					asignar_bloque_bitmap(nuevo_bloque, cantidad_bloques(nuevo_tamanio));
					//Primero copio lo que estaba en los bloques anteriores al inicio del nuevo bloque
					escribir_datos_bloque(buffer_bloques + inicio_bloque(metadata_compactacion_2->bloque_inicial), inicio_bloque(nuevo_bloque), metadata_compactacion_2->tamanio_archivo);
					//Ahora limpio el espacio donde estaba el bloque
					remover_datos_bloque(inicio_bloque(metadata_compactacion_2->bloque_inicial), tamanio_anterior);
					remover_bloque_bitmap(metadata_compactacion_2->bloque_inicial, cantidad_bloques_actual);
					modificar_metadata(ruta, nuevo_bloque, nuevo_tamanio);

					free(metadata_compactacion_2);
				}
			}
		}
	}else{
		if(diferencia_bloques == 0){
			//Aca entra en el mismo bloque
			modificar_metadata(ruta, metadata->bloque_inicial, nuevo_tamanio);
		}else{
			//Aca remueve del bitmap la cantidad que sobra
			remover_bloque_bitmap(metadata->bloque_inicial + cantidad_nueva_bloques, abs(diferencia_bloques));
			modificar_metadata(ruta, metadata->bloque_inicial, nuevo_tamanio);
		}
	}
	free(metadata);

	return EXIT_SUCCESS;
}

int fs_write(int pid, char* ruta, int registro_direccion, int registro_tamanio, int registro_puntero){
	log_info(entradaSalida_logger, "PID: <%i> - Escribir Archivo: <%s> - Tamaño a Escribir: <%i> - Puntero Archivo <%i>", pid, ruta, registro_tamanio, registro_puntero);

	//Pedir a memoria que me devuelva la cantidad de registro_tamanio bytes en registro_direccion

//	char* datos_memoria = pedir_a_memoria_datos(registro_direccion, registro_tamanio); //Esto esta comentado para pruebas
	char* datos_memoria = "Aquella procesion de testosterona,que las hormonas formaron parte del bello arte del beso al cuello, le puso el sello de aquella noche, todo iba ser fenomenal....";;

	//Lo de arriba es memoria, esperar OK o algo como NULL

	t_metadata* metadata = datos_metadata(ruta);

	int tamanio_bloques = cantidad_bloques(metadata->tamanio_archivo)*(ioconfig->BLOCK_SIZE)/8; //Es el peso en bytes de los bloques que ocupa

	if(registro_puntero > tamanio_bloques){ //Si el registro_puntero es mas grande que el bloque
		printf("No se puede escribir en ese sector, pertenece a otro archivo!!!\n");
		free(metadata);
		return EXIT_FAILURE;
	}else{
		if(registro_puntero + registro_tamanio > tamanio_bloques){ //SI registro_puntero + tamanio se pasan del tamanio de bloques
			escribir_datos_bloque(datos_memoria, inicio_bloque(metadata->bloque_inicial) + registro_puntero, tamanio_bloques - registro_puntero);
			modificar_metadata(ruta, metadata->bloque_inicial, tamanio_bloques);
		}else{
			if((registro_puntero + registro_tamanio) > metadata->tamanio_archivo){ //Este caso es cuando puntero + tamanio es mas grande que tamanio_archivo
				escribir_datos_bloque(datos_memoria, inicio_bloque(metadata->bloque_inicial) + registro_puntero, registro_tamanio);
				modificar_metadata(ruta, metadata->bloque_inicial, registro_puntero + registro_tamanio);
			}else{
				escribir_datos_bloque(datos_memoria, inicio_bloque(metadata->bloque_inicial) + registro_puntero, registro_tamanio);
			}
		}
	}

	free(metadata);

	return EXIT_SUCCESS;
}

int fs_read(int pid, char* ruta, int registro_direccion, int registro_tamanio, int registro_puntero){
	log_info(entradaSalida_logger, "PID: <%i> - Leer Archivo: <%s> - Tamaño a Leer: <%i> - Puntero Archivo <%i>", pid, ruta, registro_tamanio, registro_puntero);

	t_metadata* metadata = datos_metadata(ruta);
	char* datos_a_memoria = malloc(sizeof(char));

	if(registro_puntero + registro_tamanio > metadata->tamanio_archivo){//Quiere decir que no hay tantos datos para leer, sino leeria basura
		memcpy(datos_a_memoria + registro_direccion, buffer_bloques + inicio_bloque(metadata->bloque_inicial) + registro_puntero, abs(metadata->tamanio_archivo - registro_puntero));
	}
	else{//Hay sufuicientes datos para leer
		memcpy(datos_a_memoria + registro_direccion, buffer_bloques + inicio_bloque(metadata->bloque_inicial) + registro_puntero, registro_tamanio);
	}
	//Aca es parte de conexion, hay que ver el protocolo y demas, espera recibir un OK
//	enviar_a_memoria(datos_a_memoria, registro_direccion); //Esto esta comentado para pruebas

	free(datos_a_memoria);
	free(metadata);

	return EXIT_SUCCESS;
}

//Solicitudes a memoria
char* pedir_a_memoria_datos(int registro_direccion, int registro_tamanio){
	pthread_mutex_lock(&mutex_memoria);
		t_paquete* paquete = crear_paquete(LEER_MEMORIA);
		agregar_int_a_paquete(paquete, registro_direccion);
		agregar_int_a_paquete(paquete, registro_tamanio);
		enviar_paquete(paquete, fd_memoria);
		eliminar_paquete(paquete);

		//Espero a Memoria
		op_code cod;
		cod = recibir_operacion(fd_memoria); //Habria que verificar que op_code me manda, pero voy a suponer que no hubo error

		if(cod == -1){
			pthread_mutex_unlock(&mutex_memoria);
			return NULL;
		}
		t_buffer* buffer = recibir_todo_buffer(fd_memoria); //Aca quise poner algo de recepcion de memoria, nose si esta bien

		char* datos_memoria = extraer_string_del_buffer(buffer);
	pthread_mutex_unlock(&mutex_memoria);

	return datos_memoria;
}

int enviar_a_memoria(char* datos, int registro_direccion){
	pthread_mutex_lock(&mutex_memoria);
		t_paquete* paquete = crear_paquete(ESCRIBIR_MEMORIA);
		agregar_string_a_paquete(paquete, datos);
		agregar_int_a_paquete(paquete, registro_direccion);
		enviar_paquete(paquete, fd_memoria);
		eliminar_paquete(paquete);
	pthread_mutex_unlock(&mutex_memoria);

	return EXIT_SUCCESS;
}

//Funciones generales para bitmap y bloques
char* ruta_absoluta(char* ruta_relativa){
	int tamanio_ruta = strlen(ioconfig->PATH_BASE_DIALFS) + strlen(ruta_relativa) + 2;
	char* ruta = malloc(tamanio_ruta*sizeof(char));

	strcpy(ruta, ioconfig->PATH_BASE_DIALFS);
	strcat(ruta, "/");
	strcat(ruta, ruta_relativa);

	return ruta;
}

void obtener_datos_filesystem(){
	obtener_archivo("bloques.dat", (void*) crear_bloques, (void*) abrir_bloques);
	obtener_archivo("bitmap.dat", (void*) crear_bitmap, (void*) abrir_bitmap);
}

int obtener_archivo(char* ruta, void (*tipo_creacion)(int), void (*tipo_apertura)(int)){
	char* ruta_archivo = ruta_absoluta(ruta);
	if(access(ruta_archivo, F_OK) == -1){
		crear_archivo(ruta, (*tipo_creacion));
	}else{
		abrir_archivo(ruta, (*tipo_apertura));
	}

	free(ruta_archivo);

	return EXIT_SUCCESS;
}

int crear_archivo(char* ruta, void (*tipo_Archivo)(int)){
	int fd;

	char* ruta_archivo = ruta_absoluta(ruta);

	if((fd = open(ruta_archivo, O_CREAT|O_RDWR|O_TRUNC, S_IRWXU)) == -1){
		printf("No se pudo crear el archivo!!!\n");
		return EXIT_FAILURE;
	}else{
		(*tipo_Archivo)(fd);
		close(fd);
	}

	free(ruta_archivo);

	return EXIT_SUCCESS;
}


int abrir_archivo(char* ruta, void (*tipo_Archivo)(int)){
	int fd;

	char* ruta_archivo = ruta_absoluta(ruta);

	if((fd = open(ruta_archivo, O_RDWR)) == -1){
		printf("No se pudo abrir el archivo!!!\n");
		return EXIT_FAILURE;
	}else{
		(*tipo_Archivo)(fd);
		close(fd);
	}

	free(ruta_archivo);

	return EXIT_SUCCESS;
}

//Funciones de bloques.dat
void crear_bloques(int file_descriptor){
	int tamanio_archivo = ioconfig->BLOCK_COUNT * ceil((ioconfig->BLOCK_SIZE)/8);

	ftruncate(file_descriptor, tamanio_archivo);

	struct stat buf;

	fstat(file_descriptor, &buf);
	buffer_bloques = mmap(NULL, buf.st_size, PROT_READ|PROT_WRITE, MAP_SHARED, file_descriptor, 0);

	int posicion = 0;
	for(int i=0;i<tamanio_archivo;i++){
		memset(buffer_bloques + posicion, 0x00, sizeof(char));
		posicion++;
	}
	msync(buffer_bloques, buf.st_size, MS_SYNC);

}

void abrir_bloques(int file_descriptor){
	struct stat buf;

	fstat(file_descriptor, &buf);
	buffer_bloques = mmap(NULL, buf.st_size, PROT_READ|PROT_WRITE, MAP_SHARED, file_descriptor, 0);

}

int inicio_bloque(int n_bloque){
	int bytes;

	bytes = n_bloque * ((ioconfig->BLOCK_SIZE)/8);

	return bytes;
}

int cantidad_bloques(int tamanio){
	int cantidad = ((tamanio+7)/ 8);
	if(cantidad == 0){
		return 1;
	}
	return cantidad;
}

int escribir_datos_bloque(char* buffer, int posicion_inicial, int cantidad){
	memcpy(buffer_bloques + posicion_inicial, buffer, cantidad);

	msync(buffer_bloques, strlen(buffer_bloques),MS_SYNC);

	return EXIT_SUCCESS;
}

int remover_datos_bloque(int posicion_inicial, int cantidad){
	for(int i = posicion_inicial;i< (posicion_inicial + cantidad);i++){
		memset(buffer_bloques + i, 0x00, sizeof(char));
	}
	msync(buffer_bloques, strlen(buffer_bloques),MS_SYNC);

	return EXIT_SUCCESS;
}

//Funciones de bitmap.dat
void crear_bitmap(int file_descriptor){
	int tamanio_archivo = ioconfig->BLOCK_COUNT;

	ftruncate(file_descriptor, tamanio_archivo);

	char* buffer;
	struct stat buf;

	fstat(file_descriptor, &buf);
	buffer = mmap(NULL, buf.st_size, PROT_READ|PROT_WRITE, MAP_SHARED, file_descriptor, 0);
	buffer_bitmap = bitarray_create_with_mode(buffer, buf.st_size, LSB_FIRST);

	for(int i=0;i<buffer_bitmap->size;i++){
		bitarray_clean_bit(buffer_bitmap, i);
	}
	msync(buffer_bitmap, buffer_bitmap->size, MS_SYNC);
}

void abrir_bitmap(int file_descriptor){
	char* buffer;
	struct stat buf;

	fstat(file_descriptor, &buf);
	buffer = mmap(NULL, buf.st_size, PROT_READ|PROT_WRITE, MAP_SHARED, file_descriptor, 0);
	buffer_bitmap = bitarray_create_with_mode(buffer, buf.st_size, LSB_FIRST);

	msync(buffer_bitmap, buffer_bitmap->size, MS_SYNC);
}

int buscar_bloque_bitmap(int longitud){
	int n_bloque = -1; //Si devuelve este valor, no hay lugar libre para ese tamaño
	int flag = 0; // 0  es false, 1 es true
	int contador_espacios = 0;
	for(int i=0;i<buffer_bitmap->size;i++){
		if(!bitarray_test_bit(buffer_bitmap, i)){
			if(flag == 0){
				n_bloque = i;
				flag = 1;
			}
			contador_espacios++;
			if(contador_espacios == longitud){
				return n_bloque;
			}
		}else{
			n_bloque = -1;
			contador_espacios = 0;
			flag = 0;
		}
	}

	msync(buffer_bitmap, buffer_bitmap->size, MS_SYNC);

	return n_bloque;
}
int asignar_bloque_bitmap(int bloque_libre, int longitud){

	for(int i = bloque_libre;i<(bloque_libre+longitud);i++){
		bitarray_set_bit(buffer_bitmap, i);
	}

	msync(buffer_bitmap, buffer_bitmap->size, MS_SYNC);

	return EXIT_SUCCESS;
}

int remover_bloque_bitmap(int bloque_inicial, int longitud){
	for(int i = bloque_inicial;i<(bloque_inicial+longitud);i++){
		bitarray_clean_bit(buffer_bitmap, i);
	}
	msync(buffer_bitmap, buffer_bitmap->size, MS_SYNC);

	return EXIT_SUCCESS;
}

//Archivos de metadata

int crear_metadata(char* ruta, int bloque_inicial, int tamanio_archivo){
	int fd;
	char* ruta_archivo = ruta_absoluta(ruta);

	if((fd = open(ruta_archivo, O_CREAT, S_IRWXU)) == -1){
		perror("No se pudo crear el archivo!!!\n");
		return EXIT_FAILURE;
	}else{
		close(fd);

		char* bloque_string = malloc(50*sizeof(char));
		char* tamanio_string = malloc(50*sizeof(char));

		sprintf(bloque_string, "%i", bloque_inicial);
		sprintf(tamanio_string, "%i", tamanio_archivo);

		modificar_metadata(ruta, bloque_inicial, tamanio_archivo);

		free(bloque_string);
		free(tamanio_string);
		free(ruta_archivo);
	}

	return EXIT_SUCCESS;
}

int cargar_metadata(char* ruta, t_metadata* metadata){
	t_config* config = config_create(ruta);

	if(!config){
		perror("No se pudo crear el config correctamente!!!\n");
		return EXIT_FAILURE;
	}

	metadata->bloque_inicial = config_get_int_value(config, "BLOQUE_INICIAL");
	metadata->tamanio_archivo = config_get_int_value(config, "TAMANIO_ARCHIVO");

	config_destroy(config);

	return EXIT_SUCCESS;
}

int modificar_metadata(char* ruta, int bloque_inicial, int tamanio_archivo){
	char* ruta_archivo = ruta_absoluta(ruta);
	t_config* config = config_create(ruta_archivo);

	char* bloque_string = malloc(50*sizeof(char));
	char* tamanio_string = malloc(50*sizeof(char));

	sprintf(bloque_string, "%i", bloque_inicial);
	sprintf(tamanio_string, "%i", tamanio_archivo);

	config_set_value(config, "BLOQUE_INICIAL", bloque_string);
	config_set_value(config, "TAMANIO_ARCHIVO", tamanio_string);
	config_save(config);

	free(ruta_archivo);
	free(bloque_string);
	free(tamanio_string);
	config_destroy(config);

	return EXIT_SUCCESS;
}

t_metadata* datos_metadata(char* ruta){
	t_metadata* metadata = malloc(sizeof(t_metadata));
	char* ruta_archivo = ruta_absoluta(ruta);
	t_config* config = config_create(ruta_archivo);

	metadata->bloque_inicial = config_get_int_value(config, "BLOQUE_INICIAL");
	metadata->tamanio_archivo = config_get_int_value(config, "TAMANIO_ARCHIVO");

	free(ruta_archivo);
	config_destroy(config);

	return metadata;
}

//Proceso de compactacion
int compactar(int pid){
	log_info(entradaSalida_logger, "PID: <%i> - Inicio compactacion", pid);
	t_list* lista_archivos = cargar_archivos(ioconfig->PATH_BASE_DIALFS);

	//Aca debe limpiar todo el buffer del bitmap
	remover_bloque_bitmap(0, buffer_bitmap->size);

	int contador_posicion = 0;
	for(int i=0;i<list_size(lista_archivos);i++){
		t_metadata* metadata = (t_metadata*) list_get(lista_archivos, i); //Agarra el primer elemento de la lista
		int nuevo_bloque_inicial = contador_posicion;

		//Muevo todos los datos del bloque al nuevo bloque
		if(metadata->bloque_inicial != contador_posicion){
			escribir_datos_bloque(buffer_bloques + inicio_bloque(metadata->bloque_inicial), nuevo_bloque_inicial, metadata->tamanio_archivo);
		}
		asignar_bloque_bitmap(contador_posicion, cantidad_bloques(metadata->tamanio_archivo)); //Aca voy seteando el bitmap con los datos del archivo

		contador_posicion += cantidad_bloques(metadata->tamanio_archivo);

		modificar_metadata(metadata->nombre, nuevo_bloque_inicial, metadata->tamanio_archivo);

		free(metadata->nombre);
		free(metadata);
	}

	list_destroy(lista_archivos);

		usleep(ioconfig->RETRASO_COMPACTACION*1000);
		log_info(entradaSalida_logger, "PID: <%i> - Fin compactacion", pid);

		return EXIT_SUCCESS;
}

//Carga todos los archivos de la ruta directorio, que sean distintos de .txt, para armar una lista y usarlo para compactacion
t_list* cargar_archivos(char* ruta_directorio){
	t_list* lista_archivos = list_create();
	DIR *dir = opendir(ruta_directorio);
	if(!dir){
		perror("opendir");
		return NULL;
	}

	struct dirent *ent;
	while((ent = readdir(dir))){
		//Muestra todos los archivos menos bloques.dat y bitmap.dat
		if(strlen(ent->d_name) > 4 && strcmp(ent->d_name + strlen(ent->d_name)-4,".txt") == 0){
			t_metadata* metadata = datos_metadata(ent->d_name);
			metadata->nombre = malloc(50*sizeof(char));
			strcpy(metadata->nombre, ent->d_name);

			//Mete todo a una lista pero ordenada segun su bloque inicial
			list_add_sorted(lista_archivos, metadata, (void*) comparar_bloque_inicial);
		}
	}
	closedir(dir);

	return lista_archivos;
}

//Usado para comparar el numerpo de blqoue de inicio
bool comparar_bloque_inicial(t_metadata* metadata_lista, t_metadata* metadata){
	return metadata_lista->bloque_inicial < metadata->bloque_inicial;
}
