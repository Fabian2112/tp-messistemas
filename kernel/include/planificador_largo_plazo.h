#ifndef PLANIFICADOR_CORTO_PLAZO_H_
#define PLANIFICADOR_CORTO_PLAZO_H_

#include "k_gestor.h"
#include "desalojador.h"
#include "servicios_kernel.h" //incluido porque se utiliza el  crear_pcb
#include "instrucciones.h" //incluido porque se utiliza el  crear_memoria_pcb



void manejadorPlanificadorLargoPlazo();
void iteratorReady(t_pcb* pcbActual);
void moveProcessFIFO(t_list* listaDesde, t_list* listaHasta);
void logColaReady();

//Variables propias 
//t_list* new;
//Funciones
//void nuevoProceso(char *path);


#endif /* PLANIFICADOR_CORTO_PLAZO_H_ */