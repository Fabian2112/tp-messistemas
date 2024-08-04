#include <../include/consola.h>

void iniciar_consola_funciones()
{
    while (conexiones < 4)
    {
        log_info(kernel_logger, "Esperando conexiones...");
        sleep(5);
    }

    printf("Consola Iniciada - Ingrese un comando Valido: \n");
    char *leido = readline("> ");
    bool validacion_leido;

    while (strcmp(leido, "\0") != 0)
    {
        validacion_leido = validacion_de_instruccion_de_consola(leido);
        if (!validacion_leido)
        {
            log_error(kernel_logger, "Comando DESCONOCIDO!!");
            free(leido);
            leido = readline("> ");
            continue; // saltea y continua con la iteracion
        }

        atender_instruccion_validada(leido);
        free(leido);
        printf("Ingrese un comando Valido: \n");
        leido = readline("> ");
    }

    free(leido);
}

bool validacion_de_instruccion_de_consola(char *leido)
{
    bool resultado_validacion = false;

    char **comando_consola = string_split(leido, " ");


    if(strcmp(comando_consola[0], "EJECUTAR_SCRIPT") == 0 ||strcmp(comando_consola[0], "A") == 0 ){
        resultado_validacion = true;
    }
    else if (strcmp(comando_consola[0], "INICIAR_PROCESO") == 0)
    {
        resultado_validacion = true;
    }
    else if (strcmp(comando_consola[0], "FINALIZAR_PROCESO") == 0)
    {
        resultado_validacion = true;
    }
    else if (strcmp(comando_consola[0], "DETENER_PLANIFICACION") == 0)
    {
        resultado_validacion = true;
    }
    else if (strcmp(comando_consola[0], "INICIAR_PLANIFICACION") == 0)
    {
        resultado_validacion = true;
    }
    else if (strcmp(comando_consola[0], "MULTIPROGRAMACION") == 0)
    {
        resultado_validacion = true;
    }
    else if (strcmp(comando_consola[0], "PROCESO_ESTADO") == 0)
    {
        resultado_validacion = true;
    }
    else
    {
        log_error(kernel_logger, "Comando Desconocido");
        resultado_validacion = false;
    }

    string_array_destroy(comando_consola);

    return resultado_validacion;
}

void f_iniciar_proceso(char *path)
{
    t_pcb *aux = malloc(sizeof(*aux));

    aux->pid = asignar_pid();

    aux->programCounter = 0;
    // aux->registros_cpu = (registros_cpu *)malloc(sizeof(registros_cpu));
    aux->registros_cpu.AX = 0;
    aux->registros_cpu.BX = 0;
    aux->registros_cpu.CX = 0;
    aux->registros_cpu.DX = 0;
    aux->registros_cpu.PC = 0;
    aux->registros_cpu.EAX = 0;
    aux->registros_cpu.EBX = 0;
    aux->registros_cpu.ECX = 0;
    aux->registros_cpu.EDX = 0;
    aux->registros_cpu.SI = 0;
    aux->registros_cpu.DI = 0;
    aux->flag = 0;
    aux->recurso = "recurso_default";
    aux->estado = NEW;
    aux->recurso_solicitado = "recurso_default";
    aux->recurso_utilizado = list_create();

    crear_memoria_pcb(aux->pid, path);
    list_add(listaNuevosProcesos, aux);

    log_info(kernel_logger, "Se crea el proceso ID %d en NEW", aux->pid);
}


 void f_ejecutar_script(char* path) 

{
    FILE* file;
    file = fopen(path, "r");
    int numLineaActual = 0;
    char lineaActual[1000];
    while(fgets(lineaActual, 1000, file) != NULL){
        log_info(kernel_logger, "for: %d", numLineaActual);
        lineaActual[strcspn(lineaActual, "\n")] = 0;
        char* nuevaLinea = malloc(strlen(lineaActual) + 1);
        strcpy(nuevaLinea, lineaActual);
        atender_instruccion_validada(nuevaLinea);
        //agregar_a_paquete(paquete, nuevaLinea, strlen(nuevaLinea)+1);
        numLineaActual ++;
    }
    fclose(file);
} 

void atender_instruccion_validada(char *leido)
{
    char **comando_consola = string_split(leido, " ");


    if (strcmp(comando_consola[0], "EJECUTAR_SCRIPT") == 0 ||strcmp(comando_consola[0], "A")==0)

    {
        //char* comando=comando_consola[1];
        char* comando= "/home/utnso/tp-2024-1c-Los-Messistemas/utils/pruebas/SCRIP1";
        f_ejecutar_script(comando);
        
        // cargar_string_al_buffer(un_buffer, comando_consola[1]); //[path]
        // f_ejecutar_script(un_buffer);
        // pthread_create(&un_hilo, NULL, (void*)f_ejecutar_script, un_buffer);
        // pthread_detach(un_hilo);
    }
    else if (strcmp(comando_consola[0], "INICIAR_PROCESO") == 0)
    {
        char *pathProceso = "/home/utnso/tp-2024-1c-Los-Messistemas/utils/pruebas/plani.txt"; //(comando_consola[1]);

        f_iniciar_proceso(pathProceso);

        // free(pathProceso);
    }
    else if (strcmp(comando_consola[0], "FINALIZAR_PROCESO") == 0)
    {
    }
    else if (strcmp(comando_consola[0], "DETENER_PLANIFICACION") == 0)
    {
        pausar_plani();
    }
    else if (strcmp(comando_consola[0], "INICIAR_PLANIFICACION") == 0)
    {
        iniciar_plani();
    }
    else if (strcmp(comando_consola[0], "MULTIPROGRAMACION") == 0)

    {   
        int new_multi=atoi(comando_consola[1]);
        dif_multi=abs(GRADO_MULTIPROGRAMACION-new_multi);
        subir = GRADO_MULTIPROGRAMACION<=new_multi ? true:false;
        sem_post(&cambio_multi);
        GRADO_MULTIPROGRAMACION=new_multi;
        log_info(kernel_logger,"NUevo grado de multiprogramación %d",GRADO_MULTIPROGRAMACION);
    

    }
    else if (strcmp(comando_consola[0], "PROCESO_ESTADO") == 0)
    {
    }
    else
    {
        log_error(kernel_logger, "Comando Desconocido PERO VALIDADO!!");
        exit(EXIT_FAILURE);
    }

    string_array_destroy(comando_consola);
}

void pausar_plani()
{
    sem_wait(&planiLargo);
    sem_wait(&planiCorto);
    log_error(kernel_logger, "Planificaciones detenidas");
}
void iniciar_plani()
{
    sem_post(&planiLargo);
    sem_post(&planiCorto);
    log_info(kernel_logger, "Planificaciones iniciadas");
}