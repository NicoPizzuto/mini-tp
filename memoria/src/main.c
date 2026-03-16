#include "memoria.h"

t_log* logger;
t_log_level log_level;
t_config* config;
int puerto_escucha;
int socket_servidor;
int socket_cpu;
int socket_kernel;

int main(int argc, char* argv[]) {
    saludar("memoria");
    obtenerConfig();
    logger = log_create("memoria.log", "MEMORIA", true, log_level);
    socket_servidor = iniciarServidor(puerto_escucha);
    socket_cpu = esperarConexion(socket_servidor);
    log_info(logger, "Se conecto cpu");
    socket_kernel = esperarConexion(socket_servidor);
    log_info(logger, "Se conecto kernel");
    return 0;
}

void obtenerConfig(){
    config = config_create("memoria.config");
    puerto_escucha = config_get_int_value(config, "PUERTO_ESCUCHA");
    log_level = log_level_from_string(config_get_string_value(config, "LOG_LEVEL"));
}
