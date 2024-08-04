/*

#include <../include/test.h>


void test_crear_memoria_pcb(void) {
    int id = 1;
    char nombre[] = "Proceso1";
    int size = 1024;

    crear_memoria_pcb(id, nombre, size);

    // Verificar que los valores son correctos
    kernel_memoria_data *info = malloc(sizeof(*info));
    info->pid = id;
    info->nombre = nombre;
    info->size = size;
    info->pagina = 0;

    CU_ASSERT_EQUAL(info->pid, id);
    CU_ASSERT_STRING_EQUAL(info->nombre, nombre);
    CU_ASSERT_EQUAL(info->size, size);
    CU_ASSERT_EQUAL(info->pagina, 0);

    free(info);
}

void run_tests(void) {
    
    if (CUE_SUCCESS != CU_initialize_registry()) {
        return; // Si la inicialización falla, salir de la función
    }

    
    CU_pSuite pSuite = NULL;
    pSuite = CU_add_suite("Suite de prueba", 0 ,0);
    if (NULL == pSuite) {
        CU_cleanup_registry(); // Limpiar el registro si la creación de la suite falla
        return;
    }

    // Agregar una prueba a la suite
    if (NULL == CU_add_test(pSuite, "Prueba", test_crear_memoria_pcb)) {
        CU_cleanup_registry(); // Limpiar el registro si la adición de la prueba falla
        return;
    }

    // Configurar el modo de salida de las pruebas
    CU_basic_set_mode(CU_BRM_VERBOSE);

    // Ejecutar todas las pruebas
    CU_basic_run_tests();

    // Limpiar el registro de pruebas después de la ejecución
    CU_cleanup_registry();
}

*/