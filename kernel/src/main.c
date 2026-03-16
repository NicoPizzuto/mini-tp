#include "kernel.h"

int socket_servidor;
int puerto_escucha;
t_config* config;
int socket_cpu;
t_log* logger;
t_log_level log_level;
int socket_memoria;
char* ip_memoria;
int puerto_memoria;

int main(int argc, char* argv[]) {
    saludar("kernel");
    obtenerConfig();
    logger = log_create("kernel.log", "KERNEL", true, log_level);
    socket_servidor = iniciarServidor(puerto_escucha);
    socket_cpu = esperarConexion(socket_servidor);
    socket_memoria = crearConexion(ip_memoria, puerto_memoria);
    log_info(logger, "Se conecto la cpu");
    return 0;
}

void obtenerConfig() {
    config = config_create("kernel.config");
    puerto_escucha = config_get_int_value(config, "PUERTO_ESCUCHA");
    log_level = log_level_from_string(config_get_string_value(config, "LOG_LEVEL"));
    ip_memoria = config_get_string_value(config, "IP_MEMORIA");
    puerto_memoria = config_get_int_value(config, "PUERTO_MEMORIA");
}
