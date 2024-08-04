#include <../include/ciclo_de_instruccion.h>


void set(void *ptrRegistro, void *ptrValor, size_t tamReg){
    if(tamReg == sizeof(uint32_t)){
        *(uint32_t *) ptrRegistro = *(uint32_t *) ptrValor;
    }
    else if(tamReg == sizeof(uint8_t)){
        *(uint8_t *) ptrRegistro = *(uint8_t *) ptrValor;
    }
    int* puntero = (int*)ptrRegistro;
}

void mov_in(void* ptrRegistroDatos, void* ptrRegistroDireccion, size_t tamRegDatos){
    if(tamRegDatos == sizeof(uint32_t)){
        *(uint32_t *) ptrRegistroDatos = *(uint32_t *)buscarDato(ptrRegistroDireccion, tamRegDatos);
    }
    else{
        *(uint8_t *) ptrRegistroDatos = *(uint8_t *)buscarDato(ptrRegistroDireccion, tamRegDatos);
    }
}

void mov_out(void* ptrRegistroDireccion, void* ptrRegistroDatos, size_t tamRegDatos){
    enviarDato(ptrRegistroDatos, ptrRegistroDireccion, tamRegDatos);
}

void sum(void *ptrRegistroDestino, void *ptrRegistroOrigen, size_t tamDestino){
    if(tamDestino == sizeof(uint32_t)){
        *(uint32_t *) ptrRegistroDestino = *(uint32_t*) ptrRegistroDestino + *(uint32_t*) ptrRegistroOrigen;
    }
    else if(tamDestino == sizeof(uint8_t)){
        *(uint8_t *) ptrRegistroDestino = *(uint8_t*) ptrRegistroDestino + *(uint8_t*) ptrRegistroOrigen ;
    }
    log_error(cpu_logger, "Valor luego del SUM: %u", *(uint8_t*)ptrRegistroDestino);
}

void sub(void *ptrRegistroDestino, void *ptrRegistroOrigen, size_t tamDestino){
    if(tamDestino == sizeof(uint32_t)){
        *(uint32_t *) ptrRegistroDestino = *(uint32_t*) ptrRegistroDestino - *(uint32_t*) ptrRegistroOrigen;
    }
    else if(tamDestino == sizeof(uint8_t)){
        *(uint8_t *) ptrRegistroDestino = *(uint8_t*) ptrRegistroDestino - *(uint8_t*) ptrRegistroOrigen ;
    }
    log_error(cpu_logger, "Valor luego del SUB: %u", *(uint8_t*)ptrRegistroDestino);
}

void jnz(void *registro, int nroInstruccion, size_t tamReg){
    if(tamReg == sizeof(uint32_t) && *(uint32_t *) registro != 0){
        registrosCpu.PC = nroInstruccion;
    }
    else if(tamReg == sizeof(uint8_t) && *(uint8_t *) registro != 0){
        registrosCpu.PC = nroInstruccion;
    }
}

void io_gen_sleep(char* interfaz, int tiempoADormir){
    t_paquete* paquete = crear_paquete(IO_GEN_SLEEP);
    agregar_int_a_paquete(paquete, pid);
    agregar_string_a_paquete(paquete, interfaz);
    agregar_int_a_paquete(paquete, tiempoADormir);
    enviar_paquete(paquete, fd_kernel_dispatch);
    //int confirmacion = 0;
    //recv(fd_kernel_dispatch, &confirmacion, sizeof(int), MSG_WAITALL);
     continua_ejecucion = false;
}

void resize(int tamanio){
    pedirIncrementoMemoria(tamanio);
}

void copy_string(int tamanio){
    //string de la direccion guardada en SI
    //copia cantidad de bytes indicadas por tamanio en DI
    void* direccionLogica = (void*)(uintptr_t)registrosCpu.SI;
    char* string =(char*)buscarDato(direccionLogica, tamanio);
    char* stringCopied = NULL;
    stringCopied = malloc(tamanio);
    memcpy(stringCopied, string, tamanio);
    enviarDato(stringCopied, &registrosCpu.DI, tamanio);
}

void exec_exit(){
    t_paquete* paquete = crear_paquete(EXIT_INSTRUCCTION);
    agregar_int_a_paquete(paquete, pid);
    //enviar_registros(paquete, registrosCpu, fd_kernel_dispatch);
    agregar_registros_a_paquete(paquete, registrosCpu);
    enviar_paquete(paquete, fd_kernel_dispatch);
    continua_ejecucion = false;
    log_error(cpu_logger, "llego aca");
}

char* fetch(){
    log_info(cpu_logger, "PID: %d - FETCH - Program Counter: %d", pid, registrosCpu.PC);
    t_paquete* paquete = crear_paquete(SOLICITUD_INSTRUCCION);
    agregar_int_a_paquete(paquete, pid);
    //agregar_registros_a_paquete(paquete, registrosCpu);
    //agregar_int_a_paquete(paquete, registrosCpu.PC);
    agregar_a_paquete_datoEstatico(paquete, &registrosCpu.PC, sizeof(uint32_t));
    enviar_paquete(paquete, fd_memoria);
    //enviar_registros(paquete, registrosCpu, fd_memoria);
    int cod_op;
    int size_paq;
    /*semaforos*/
    recv(fd_memoria, &cod_op, sizeof(int), MSG_WAITALL);
    recv(fd_memoria, &size_paq, sizeof(int), MSG_WAITALL);
    char* instruccion_recibida = recibir_string(fd_memoria);
    log_info(cpu_logger, "Instruccion a ejecutar: %s", instruccion_recibida);
	registrosCpu.PC += 1;
    
	free(paquete);

    return instruccion_recibida;
}



//deberia ser este quien dispare la traduccion de logica a fisica (quitar de execute y agregar aca), ejemplo en mov_in
//a traves del modulo mmu
t_instruccion* decode(char *lineaInstruccion){
   
    t_instruccion* instruccion_params = malloc(sizeof(t_instruccion));
    char *instruccion;
    t_list *listaParam = list_create();
    char *token = strtok(lineaInstruccion, " ");
    
    if (token != NULL) {
        instruccion = strdup(token);
        token = strtok(NULL, " ");
        while (token != NULL) {
            list_add(listaParam, strdup(token));
            token = strtok(NULL, " ");
        }
    }
    instruccion_params->parametros = listaParam;
    if(strcmp(instruccion, "SET") == 0){
        instruccion_params->identificador = SET;
        instruccion_params->cant_params = 2;
    }
    else if(strcmp(instruccion, "SUM") == 0){
        instruccion_params->identificador = SUM;
        instruccion_params->cant_params = 2;
    }
    else if(strcmp(instruccion, "SUB") == 0){
        instruccion_params->identificador = SUB;
        instruccion_params->cant_params = 2;
    }
    else if(strcmp(instruccion, "JNZ") == 0){
        instruccion_params->identificador = JNZ;
        instruccion_params->cant_params = 2;
    }
    else if(strcmp(instruccion, "IO_GEN_SLEEP") == 0){
        instruccion_params->identificador = IO_GEN_SLEEP;
        instruccion_params->cant_params = 2;
    }
    else if(strcmp(instruccion, "WAIT") == 0){
        instruccion_params->identificador = WAIT;
        instruccion_params->cant_params = 2;
    }
    else if(strcmp(instruccion, "EXIT") == 0){
        instruccion_params->identificador = EXIT_INSTRUCCTION;
        instruccion_params->cant_params = 0;
    }
    else if(strcmp(instruccion, "SIGNAL") == 0){
        instruccion_params->identificador = SIGNAL;
        instruccion_params->cant_params = 2;
    }
    else if(strcmp(instruccion, "MOV_IN") == 0){
        //elemento_lista = list_get(primerParam) 
        //elemento_lista = traducirAFisica(elemento_lista);
        //list_replace(elemento_lista, 0);
        instruccion_params->identificador = MOV_IN ;
        instruccion_params->cant_params = 2;
    }
    else if(strcmp(instruccion, "MOV_OUT") == 0){
        instruccion_params->identificador = MOV_OUT;
        instruccion_params->cant_params = 2;
    }
    else if(strcmp(instruccion, "RESIZE") == 0){
        instruccion_params->identificador = RESIZE;
        instruccion_params->cant_params = 2;
    }
    else if(strcmp(instruccion, "COPY_STRING") == 0){
        instruccion_params->identificador = COPY_STRING;
        instruccion_params->cant_params = 2;
    }
    else if(strcmp(instruccion, "IO_STDIN_READ") == 0){
        instruccion_params->identificador = IO_STDIN_READ;
        instruccion_params->cant_params = 2;
    }
    else if(strcmp(instruccion, "IO_STDOUT_WRITE") == 0){
        instruccion_params->identificador = IO_STDOUT_WRITE;
        instruccion_params->cant_params = 2;
    }
    else if(strcmp(instruccion, "IO_FS_CREATE") == 0){
        instruccion_params->identificador = IO_FS_CREATE;
        instruccion_params->cant_params = 2;
    }
    else if(strcmp(instruccion, "IO_FS_DELETE") == 0){
        instruccion_params->identificador = IO_FS_DELETE ;
        instruccion_params->cant_params = 2;
    }
    else if(strcmp(instruccion, "IO_FS_TRUNCATE") == 0){
        instruccion_params->identificador = IO_FS_TRUNCATE;
        instruccion_params->cant_params = 2;
    }
    else if(strcmp(instruccion, "IO_FS_WRITE") == 0){
        instruccion_params->identificador = IO_FS_WRITE;
        instruccion_params->cant_params = 2;
    }
    else if(strcmp(instruccion, "COPY_STRING") == 0){
        instruccion_params->identificador = IO_FS_READ;
        instruccion_params->cant_params = 2;
    }

    return instruccion_params;
}


void execute(t_instruccion* instruccion_params){
        int segundoParamNum=0;
		char* primerParam = instruccion_params->cant_params > 0 ? list_get(instruccion_params->parametros, 0): "";
		char* segundoParam = instruccion_params->cant_params > 1 ? list_get(instruccion_params->parametros, 1): "";
		char* tercerParam = instruccion_params->cant_params > 2 ? list_get(instruccion_params->parametros, 2): "";
        char* paramsConcatenados = instruccion_params->cant_params > 0 ? (primerParam, segundoParam, tercerParam):"";
		switch(instruccion_params->identificador){
			case SET:
                //set(registro, valor)
                log_instruccion("SET", paramsConcatenados);
                segundoParamNum = atoi(segundoParam);
				set(decodeRegister(primerParam), &segundoParamNum, decodeSize(primerParam));
                break;
            case MOV_IN:
                //mov_in(registroDatos, registroDireccion)
                log_instruccion("MOV_IN", paramsConcatenados);
                mov_in(decodeRegister(primerParam), decodeRegister(segundoParam), decodeSize(primerParam));
                break;
            case MOV_OUT:
                //mov_out(registroDireccion, registroDatos)
                log_instruccion("MOV_OUT", paramsConcatenados);
                mov_out(decodeRegister(primerParam), decodeRegister(segundoParam), decodeSize(segundoParam));
                break;
			case SUM:
                //sum(registroDestino, registroOrigen)
                log_instruccion("SUM", paramsConcatenados);
                sum(decodeRegister(primerParam), decodeRegister(segundoParam), decodeSize(primerParam));
                break;
			case SUB:
                //sub(registroDestino, registroOrigen)
                log_instruccion("SUB", paramsConcatenados);
				sub(decodeRegister(primerParam), decodeRegister(segundoParam), decodeSize(primerParam));
				break;
			case JNZ:
                //jnz(registro, instruccion)
                log_instruccion("JNZ", paramsConcatenados);
                 segundoParamNum = atoi(segundoParam);
				jnz(decodeRegister(primerParam), segundoParamNum, decodeSize(primerParam));
				break;
            case RESIZE:
                //resize(tamanio)
                log_instruccion("RESIZE", paramsConcatenados);
                int param = atoi(primerParam);
                resize(param);
                break;
            case COPY_STRING:
                //copy_string(tamanio)
                log_instruccion("COPY_STRING", paramsConcatenados);
                int algoString=(int)(uintptr_t)decodeRegister(primerParam);
                copy_string(algoString);
                break;
            case WAIT:
                //wait(recurso)
                log_instruccion("WAIT", paramsConcatenados);
                break;
            case SIGNAL:
                //signal(recurso)
                log_instruccion("SIGNAL", paramsConcatenados);
                break;
              
            case IO_GEN_SLEEP:
                //io_gen_sleep(interfaz, unidadesDeTrabajo);
                //pedir a kernel que duerma proceso por tantas <unidadesDeTrabajo> en la <interfaz>
                log_instruccion("IO_GEN_SLEEP", paramsConcatenados);
                segundoParamNum = atoi(segundoParam);
                io_gen_sleep(primerParam, segundoParamNum);
                //io_gen_sleep(decodeRegister(primerParam), decodeRegister(segundoParam));
                break;
            case IO_STDIN_READ:
                //io_stdin_read(interfaz, registroDireccion, registroTamanio)
                log_instruccion("IO_STDIN_READ", paramsConcatenados);
                //io_stdin_read(primerParam, segundoParam, tercerParam);
                break;
            case IO_STDOUT_WRITE:
                //io_stdout_write(interfaz, registroDireccion, registroTamanio)
                log_instruccion("IO_STDOUT_WRITE", paramsConcatenados);
                //io_stdout_write(primerParam, segundoParam, tercerParam);
                break;
            case IO_FS_CREATE:
                //io_fs_create(interfaz, nombreArchivo)
                log_instruccion("IO_FS_CREATE", paramsConcatenados);
                break;
            case IO_FS_DELETE:
                //io_fs_delete(interfaz, nombreArchivo)
                log_instruccion("IO_FS_DELETE", paramsConcatenados);
                break;
            case IO_FS_TRUNCATE:
                //io_fs_truncate(interfaz, nombreArchivo, registroTamanio)
                log_instruccion("IO_FS_TRUNCATE", paramsConcatenados);
                break;
            case IO_FS_WRITE:
                //io_fs_write(interfaz, nombreArchivo, registroDireccion, registroTamanio, registroPunteroArchivo)
                log_instruccion("IO_FS_WRITE", paramsConcatenados);
                break;
            case IO_FS_READ:
                //io_fs_read(interfaz, nombreArchivo, registroDireccion, registroTamanio, registroPunteroArchivo)
                log_instruccion("IO_FS_READ", paramsConcatenados);
                break; 
            case EXIT_INSTRUCCTION:
                log_instruccion("EXIT", paramsConcatenados);
                exec_exit();
                break;
		}
		//free(instruccion_params);

}

void* decodeRegister(char* registroBuscado) {
    if (strcmp(registroBuscado, "EAX") == 0) {
        return &registrosCpu.EAX;
    } else if (strcmp(registroBuscado, "EBX") == 0) {
        return &registrosCpu.EBX;
    } else if (strcmp(registroBuscado, "ECX") == 0) {
        return &registrosCpu.ECX;
    } else if (strcmp(registroBuscado, "EDX") == 0) {
        return &registrosCpu.EDX;
    } else if (strcmp(registroBuscado, "AX") == 0) {
        return &registrosCpu.AX;
    } else if (strcmp(registroBuscado, "BX") == 0) {
        return &registrosCpu.BX;
    } else if (strcmp(registroBuscado, "CX") == 0) {
        return &registrosCpu.CX;
    } else if (strcmp(registroBuscado, "DX") == 0) {
        return &registrosCpu.DX;
    } else if (strcmp(registroBuscado, "PC") == 0) {
        return &registrosCpu.PC;
    } else if (strcmp(registroBuscado, "DI") == 0) {
        return &registrosCpu.DI;
    } else if (strcmp(registroBuscado, "SI") == 0) {
        return &registrosCpu.SI;
    }else {
        return NULL;
    }
}

size_t decodeSize(char* registroBuscado) {
    if (strcmp(registroBuscado, "EAX") == 0) {
        return sizeof(uint32_t);
    } else if (strcmp(registroBuscado, "EBX") == 0) {
        return sizeof(uint32_t);
    } else if (strcmp(registroBuscado, "ECX") == 0) {
        return sizeof(uint32_t);
    } else if (strcmp(registroBuscado, "EDX") == 0) {
        return sizeof(uint32_t);
    } else if (strcmp(registroBuscado, "AX") == 0) {
        return sizeof(uint8_t);
    } else if (strcmp(registroBuscado, "BX") == 0) {
        return sizeof(uint8_t);
    } else if (strcmp(registroBuscado, "CX") == 0) {
        return sizeof(uint8_t);
    } else if (strcmp(registroBuscado, "DX") == 0) {
        return sizeof(uint8_t);
    } else if (strcmp(registroBuscado, "PC") == 0) {
        return sizeof(uint32_t);
    } else if (strcmp(registroBuscado, "DI") == 0) {
        return sizeof(uint32_t);
    } else if (strcmp(registroBuscado, "SI") == 0) {
        return sizeof(uint32_t);
    }else {
        log_error(cpu_logger, "NO puedes retornar NULL porque no es del tipo size_t");
        return sizeof(NULL);
    }
}

bool continuar_ejecucion() {
    log_error(cpu_logger, "Continua ejecucion? %d", continua_ejecucion);
    bool continuar = false;
	pthread_mutex_lock(&mutex_interrupt);
	continuar = continua_ejecucion;
	pthread_mutex_unlock(&mutex_interrupt);
    return continuar;
}

//tiene sentido realmente?
bool condicion_continuar_ejecucion(t_identificador identificador) {
	if (identificador ==  EXIT_INSTRUCCTION|| identificador == WAIT
			|| identificador == SIGNAL || identificador == RESIZE 
            || identificador == IO_GEN_SLEEP || identificador == IO_STDIN_READ 
            || identificador == IO_STDOUT_WRITE || identificador == IO_FS_CREATE 
            || identificador == IO_FS_DELETE || identificador == IO_FS_TRUNCATE  
            || identificador == IO_FS_WRITE	|| identificador == IO_FS_READ) {
		return true;
	}
	return false;
}

void ejecutar_ciclo_de_instruccion() {
	while (continuar_ejecucion()) {
		char* lineaInstruccion = fetch();
		t_instruccion* instruccion = decode(lineaInstruccion);
        execute(instruccion);
		//free(instruccion);
	}
}

char *concatenarConComas(char *primerParam, char *segundoParam, char *tercerParam) {
    int longitudTotal = strlen(primerParam) + strlen(segundoParam) + strlen(tercerParam) + 3;
    char *resultado = (char *)malloc(longitudTotal * sizeof(char));
    
    strcpy(resultado, primerParam);  
    
    if (strcmp(segundoParam, "") != 0) {
        strcat(resultado, ",");
        strcat(resultado, segundoParam);
    }
    
    if (strcmp(tercerParam, "") != 0) {
        strcat(resultado, ",");
        strcat(resultado, tercerParam);
    }
    
    return resultado;
}

void log_instruccion(char* nombreInstruccion, char* paramsConcatenados){
    log_info(cpu_logger, "%d - Ejecutando: %s - %s", pid, nombreInstruccion, paramsConcatenados);
}