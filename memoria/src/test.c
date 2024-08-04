/* #include <../include/test.h>
t_buffer* crear_buffer_de_ejemplo() {
    t_buffer* buffer = (t_buffer*)malloc(sizeof(t_buffer));
    if (buffer == NULL) {
        perror("Error al asignar memoria para el buffer");
        exit(EXIT_FAILURE);
    }

    // Datos de ejemplo
    int pid = 5;
    char path_proceso[] = "algo para copiar";
    int path_length = strlen(path_proceso)+1;

    // Calculamos el tamaño total del buffer
    buffer->size = sizeof(int) + path_length;
    buffer->stream = malloc(buffer->size);
    if (buffer->stream == NULL) {
        perror("Error al asignar memoria para los datos del buffer");
        free(buffer);
        exit(EXIT_FAILURE);
    }

    // Copiamos los datos al buffer

    int ptr = 0;
    memcpy(buffer->stream+ptr, &pid, sizeof(int));

    ptr += sizeof(int);

    memcpy(buffer->stream+ptr, &path_length, sizeof(int));
    ptr += sizeof(int);

    memcpy(buffer->stream+ptr ,path_proceso,path_length);

    return buffer;
}
void test_atender_peticion_instruccion(void) {

    t_buffer* buffer = malloc(sizeof(t_buffer));
    buffer->size = 10;
    buffer->stream = malloc(buffer->size);
    strncpy(buffer->stream, "0123456789", buffer->size);
    printf("Testeando");
    // Ejecutar la función a testear
    atender_peticion_instruccion(buffer);

   printf("Testeando");

    // Liberar la memoria asignada
    free(buffer->stream);
    free(buffer);
}
void test_obtener_instruccion(void){
    char* path="/home/utnso/tp-2024-1c-Los-Messistemas/memoria/instruccionesPruebas.text";
    int program_counter=0;


    for (int i = 0; i < 10; i++)
    {   program_counter=i;
       printf("Lo que retorna obtenerInstruccion: %s\n",obtenerInstruccion(path,program_counter));
    }

}

void test_atender_crear_proceso_memoria(){
    t_buffer *buffer=crear_buffer_de_ejemplo();

    printf("EL int del buffer es: %d\n" , extraer_int_del_buffer(buffer));
    printf("EL string extraído es: %s" , extraer_string_del_buffer(buffer));
    //atender_crear_proceso_km(buffer);
    /*printf("funciona atender_crear_proceso\n");
    kernel_memoria_process* kernelm= list_get(listaProcesosMemoria,0);
    printf("%d",kernelm->pid);
    printf("%s", kernelm->path_proceso);

}

void funcion_a_Testear(void){
    test_atender_crear_proceso_memoria();
    //test_obtener_instruccion();
}
// Función principal de pruebas
void run_test() {
    // Inicializar suite de pruebas
    CU_initialize_registry();

    // Agregar las pruebas a la suite
    CU_pSuite suite = CU_add_suite("Suite de pruebas", 0,0);
    CU_add_test(suite, "test_atender_peticion_instruccion", funcion_a_Testear);
    printf("Entro a runtest");
    // Ejecutar las pruebas
    CU_basic_run_tests();

    // Limpiar suite de pruebas
    CU_cleanup_registry();


}
*/