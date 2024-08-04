#ifndef CPU_MEMORIA_
#define CPU_MEMORIA_

#include "c_gestor.h"

void atender_cpu_memoria();
pagina_desplazamiento obtenerPaginaDesplazamiento(void* direccionLogica, int tamPagina);
int obtenerNumeroMarco(int pid, int nroPagina);
int obtenerMarcoTLB(int pid, int nroPagina);
bool matchesPidAndPage(TLB* tlb);
void reemplazarListaTlbSegunAlgoritmo(int pid, int pagina, int marco);
void* tlb_mayor_tiempo_transcurrido(TLB* tlb, TLB* tlb2);
int buscarMarcoEnTablaPaginas(int pid, int pagina);
void* buscarDato(void* direccionLogica, size_t tamDatos);
void enviarDato(void* ptrRegistro, void* direccionLogica, size_t tamReg);//antes devolvía un void*
void* buscarDatoMemoria(int marco, int desplazamiento, size_t tamDatos);
void pedirIncrementoMemoria(int tamanio);


#endif  /* CPU_MEMORIA_ */