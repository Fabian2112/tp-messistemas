#ifndef KERNEL_H_
#define KERNEL_H_

#include "k_gestor.h"
#include "inicializar_kernel.h"
#include "kernel_cpuDisp.h"
#include "kernel_cpuInt.h"
#include "kernel_es.h"
#include "kernel_memoria.h"
#include "consola.h"
#include "servicios_kernel.h"

#include "planificador_largo_plazo.h"
#include "planificador_corto_plazo.h"
//#include "test.h"
int conectar_a_cpu_interrupt();
int conectar_a_cpu_dispatch();
int conectar_a_memoria();
int conectar_a_generica();
int conectar_a_io();
void manejo_multi();
#endif  /* KERNEL_H_ */