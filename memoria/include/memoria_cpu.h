#ifndef MEMORIA_CPU_
#define MEMORIA_CPU_

#include "m_gestor.h"

void atender_memoria_cpu();
void atender_peticion_instruccion();
char* obtenerInstruccion(char* pathInstrucciones, int program_counter);


#endif  /* MEMORIA_CPU_ */
