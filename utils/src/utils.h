
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <commons/log.h>
#include <netdb.h>
#include <unistd.h>
#include <commons/config.h>
#include <commons/collections/dictionary.h>
#include <commons/bitarray.h>

typedef struct
{
	int size;
	void* stream;
} t_buffer;

typedef struct
{
	int codigo_operacion;
	t_buffer* buffer;
} t_paquete;

typedef enum
{
	MENSAJE,
	ENTERO
} utils_op_code;

typedef enum
{
	KERNEL,
	CPU,
	LEER,
	ESCRIBIR,
	PAGINA_FISICA,
	CARGAR_PROC,
	DESCARGAR_PROC
} utils_op_code_memoria;


void saludar(char* quien);
int iniciarServidor(int puerto);
int esperarConexion(int socket_servidor);
int crearConexion(char* ip, int puerto);
void* recibirBuffer(int* size, int socket_cliente);
void crearBuffer(t_paquete* paquete);
t_paquete* crearPaquete(int cop);
void agregarMensajeAPaquete(t_paquete* paquete, void* valor, int tamanio);

// Funciones para enviar y recibir paquetes
void enviarOpCode(int socket_receptor, int OP);
void enviarEntero(int socket_receptor, int ent);
int recibirOperacion(int socket_emisor);
int recibirEntero(int socket_emisor);
