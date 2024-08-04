
#include <../include/mem_instrucciones.h>

char *leer_archivo_pseudocodigo(char *archivo_a_leer)
{
    // leer del directorio el archivo indicado por PC
    DIR *directorio = opendir(PATH_INSTRUCCIONES);
    char *archivo_con_extension;
    asprintf(&archivo_con_extension, "%s.txt", archivo_a_leer);
    char *contenido = NULL;
    // asprintf(&contenido, "%s %s", PATH_INSTRUCCIONES, archivo_a_leer);
    // free(archivo_con_extension);
    closedir(directorio);
    return contenido;
}
