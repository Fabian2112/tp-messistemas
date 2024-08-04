#ifndef CPU_CICLO_DE_INSTRUCCION_
#define CPU_CICLO_DE_INSTRUCCION_

#include "c_gestor.h"


void set(void *ptrRegistro, void *ptrValor, size_t tamReg);
void mov_in(void* ptrRegistroDatos, void* ptrRegistroDireccion, size_t tamRegDatos);
void mov_out(void* ptrRegistroDireccion, void* ptrRegistroDatos, size_t tamRegDatos);
void sum(void *ptrRegistroDestino, void *ptrRegistroOrigen, size_t tamDestino);
void sub(void *ptrRegistroDestino, void *ptrRegistroOrigen, size_t tamDestino);
void jnz(void *registro, int nroInstruccion, size_t tamReg);
void resize(int tamanio);
void copy_string(int tamanio);
char* fetch();
t_instruccion* decode(char *lineaInstruccion);
void execute(t_instruccion* instruccion_params);
void* decodeRegister(char* registroBuscado);
size_t decodeSize(char* registroBuscado);
bool continuar_ejecucion();
bool condicion_continuar_ejecucion(t_identificador identificador);
void ejecutar_ciclo_de_instruccion();
char *concatenarConComas(char *primerParam, char *segundoParam, char *tercerParam);
void log_instruccion(char* nombreInstruccion, char* paramsConcatenados);

#endif  /* CPU_CICLO_DE_INSTRUCCION_ */