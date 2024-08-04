#include <../include/utils.h>

void agregar_a_paquete_datoDinamico(t_paquete* paquete, void* valor, int tamanio){
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio + sizeof(int));

	memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio, sizeof(int));
	memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), valor, tamanio);

	paquete->buffer->size += tamanio + sizeof(int);
}
void agregar_a_paquete_datoEstatico(t_paquete *paquete, void *valor, int bytes)
{
	t_buffer *buffer = paquete->buffer;
	buffer->stream = realloc(buffer->stream, buffer->size + bytes);
	memcpy(buffer->stream + buffer->size, valor, bytes);
	buffer->size += bytes;
}

void* recibir_dato_estatico(int socket, int cant_bytes){
	void* recibido = malloc(cant_bytes); // antes : int* recibido;
	recv(socket, recibido, cant_bytes, MSG_WAITALL);
	return recibido; 
}

void agregar_int_a_paquete(t_paquete* paquete, int valor){
	agregar_a_paquete_datoEstatico(paquete, &valor, sizeof(int));
}
void agregar_int_a_paquete2(t_paquete* paquete, int valor, t_log*log){

	agregar_a_paquete_datoEstatico(paquete, &valor, sizeof(int));
}

void agregar_size_t_a_paquete(t_paquete* paquete, size_t valor){
	agregar_a_paquete_datoEstatico(paquete, &valor, sizeof(size_t));
}

int recibir_int(int socket){
	
	int* recibido=malloc(sizeof(int)); // antes : int* recibido;
	recv(socket, recibido, sizeof(int), MSG_WAITALL);
	return *recibido; //antes return *recibido
}

size_t recibir_size_t(int socket){
	size_t* recibido = malloc(sizeof(size_t));
	recv(socket, recibido, sizeof(size_t), MSG_WAITALL);
	return *recibido;
}

void agregar_registros_a_paquete(t_paquete* paquete, registros_cpu registrosCpu){
	agregar_a_paquete_datoEstatico(paquete, &(registrosCpu.AX), sizeof(uint8_t));
	agregar_a_paquete_datoEstatico(paquete, &(registrosCpu.BX), sizeof(uint8_t));
	agregar_a_paquete_datoEstatico(paquete, &(registrosCpu.CX), sizeof(uint8_t));
	agregar_a_paquete_datoEstatico(paquete, &(registrosCpu.DX), sizeof(uint8_t));
	agregar_a_paquete_datoEstatico(paquete, &(registrosCpu.EAX), sizeof(uint32_t));
	agregar_a_paquete_datoEstatico(paquete, &(registrosCpu.EBX), sizeof(uint32_t));
	agregar_a_paquete_datoEstatico(paquete, &(registrosCpu.ECX), sizeof(uint32_t));
	agregar_a_paquete_datoEstatico(paquete, &(registrosCpu.EDX), sizeof(uint32_t));
	agregar_a_paquete_datoEstatico(paquete, &(registrosCpu.SI), sizeof(uint32_t));
	agregar_a_paquete_datoEstatico(paquete, &(registrosCpu.PC), sizeof(uint32_t));
	agregar_a_paquete_datoEstatico(paquete, &(registrosCpu.DI), sizeof(uint32_t));
}

void enviar_registros(t_paquete* paquete, registros_cpu registrosCpu, int socket){
	agregar_registros_a_paquete(paquete, registrosCpu);
	enviar_paquete(paquete, socket);
}

registros_cpu recibir_registros2(int socket){
	uint8_t AX,BX,CX,DX;
	uint32_t EAX,EBX,ECX,EDX,SI,PC,DI;
	recv(socket, &AX, sizeof(uint8_t), MSG_WAITALL);
	recv(socket, &BX, sizeof(uint8_t), MSG_WAITALL);
	recv(socket, &CX, sizeof(uint8_t), MSG_WAITALL);
	recv(socket, &DX, sizeof(uint8_t), MSG_WAITALL);
	recv(socket, &EAX, sizeof(uint32_t), MSG_WAITALL);
	recv(socket, &EBX, sizeof(uint32_t), MSG_WAITALL);
	recv(socket, &ECX, sizeof(uint32_t), MSG_WAITALL);
	recv(socket, &EDX, sizeof(uint32_t), MSG_WAITALL);
	recv(socket, &SI, sizeof(uint32_t), MSG_WAITALL);
	recv(socket, &PC, sizeof(uint32_t), MSG_WAITALL);
	recv(socket, &DI, sizeof(uint32_t), MSG_WAITALL);
	registros_cpu registros;
	registros.AX = AX;
	registros.BX = BX;
	registros.CX = CX;
	registros.DX = DX;
	registros.EAX = EAX;
	registros.EBX = EBX;
	registros.ECX = ECX;
	registros.EDX = EDX;
	registros.SI = SI;
	registros.PC = PC;
	registros.DI = DI;
	return registros;
}

char* recibir_string(int socket){
	int size = 0;
	char* buffer = recibir_buffer(&size, socket);
	return buffer;
}

//asegurarse que el ultimo caracter del string sea \0 para que funcione correctamente
void agregar_string_a_paquete(t_paquete* paquete, char* string){
	agregar_a_paquete_datoDinamico(paquete, string, strlen(string) + 1);
}

