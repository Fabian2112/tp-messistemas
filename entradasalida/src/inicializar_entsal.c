#include <../include/inicializar_entsal.h>

t_ioconfig *inicializar_entsal(char *nombre, char *config_path)
{
    inicializar_logs(nombre);
    t_ioconfig *ioconfig = inicializar_configs(config_path);
    imprimir_configs(ioconfig);
    return ioconfig;
}

void inicializar_logs(char *logger_name)
{
    char *newName = malloc(sizeof(logger_name) + 6);
    strcat(newName, logger_name);
    strcat(newName, "_io.log");
    entradaSalida_logger = log_create(newName, "ENTRADASALIDA_LOG", 1, LOG_LEVEL_INFO);
    if (entradaSalida_logger == NULL)
    {
        perror("Algo salio mal con el entraSalida_log, no se pudo crear o escuchar el archivo");
        exit(EXIT_FAILURE);
    }
}

t_ioconfig *inicializar_configs(char *config_path)
{
    entradaSalida_config = config_create(config_path);

    if (entradaSalida_config == NULL)
    {
        perror("Error al cargar entradaSalida_config");
        exit(EXIT_FAILURE);
    }
    struct t_ioconfig *interfaz_config = malloc(sizeof(t_config));

    interfaz_config->TIPO_INTERFAZ = config_get_string_value(entradaSalida_config, "TIPO_INTERFAZ");
    interfaz_config->TIEMPO_UNIDAD_TRABAJO = config_get_int_value(entradaSalida_config, "TIEMPO_UNIDAD_TRABAJO");
    interfaz_config->IP_KERNEL = config_get_string_value(entradaSalida_config, "IP_KERNEL");
    interfaz_config->PUERTO_KERNEL = config_get_string_value(entradaSalida_config, "PUERTO_KERNEL");
    interfaz_config->IP_MEMORIA = config_get_string_value(entradaSalida_config, "IP_MEMORIA");
    interfaz_config->PUERTO_MEMORIA = config_get_string_value(entradaSalida_config, "PUERTO_MEMORIA");
    interfaz_config->PATH_BASE_DIALFS = config_get_string_value(entradaSalida_config, "PATH_BASE_DIALFS");
    interfaz_config->BLOCK_SIZE = config_get_int_value(entradaSalida_config, "BLOCK_SIZE");
    interfaz_config->BLOCK_COUNT = config_get_int_value(entradaSalida_config, "BLOCK_COUNT");
    interfaz_config->RETRASO_COMPACTACION = config_get_int_value(entradaSalida_config, "RETRASO_COMPACTACION");

    return interfaz_config;
}

void imprimir_configs(t_ioconfig *ioconfig)
{

    log_info(entradaSalida_logger, "TIPO_INTERFAZ: %s", ioconfig->TIPO_INTERFAZ);
    log_info(entradaSalida_logger, "TIEMPO_UNIDAD_TRABAJO: %d", ioconfig->TIEMPO_UNIDAD_TRABAJO);
    log_info(entradaSalida_logger, "IP_KERNEL: %s", ioconfig->IP_KERNEL);
    log_info(entradaSalida_logger, "PUERTO_KERNEL: %s", ioconfig->PUERTO_KERNEL);
    log_info(entradaSalida_logger, "IP_MEMORIA: %s", ioconfig->IP_MEMORIA);
    log_info(entradaSalida_logger, "PUERTO_MEMORIA: %s", ioconfig->PUERTO_MEMORIA);
    log_info(entradaSalida_logger, "PATH_BASE_DIALFS: %s", ioconfig->PATH_BASE_DIALFS);
    log_info(entradaSalida_logger, "BLOCK_SIZE: %d", ioconfig->BLOCK_SIZE);
    log_info(entradaSalida_logger, "BLOCK_COUNT: %d", ioconfig->BLOCK_COUNT);
    log_info(entradaSalida_logger, "RETRASO_COMPACTACION: %d", ioconfig->RETRASO_COMPACTACION);
}