
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <commons/log.h>
#include <netdb.h>
#include <unistd.h>
#include <commons/config.h>

void saludar(char* quien);
int iniciarServidor(int puerto);
int esperarConexion(int socket_servidor);
int crearConexion(char* ip, int puerto);
