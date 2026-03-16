#include "utils.h"

int iniciarServidor(int puerto) {
    struct addrinfo hints, *server_info;
    int fd_escucha, err;
    char puerto_str[10];
    
    // Convertir el puerto a string
    snprintf(puerto_str, sizeof(puerto_str), "%d", puerto);
    
    // Configurar hints para obtener información de red
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;  // Para indicar que es servidor
    
    // Obtener información de red
    err = getaddrinfo(NULL, puerto_str, &hints, &server_info);
    if (err != 0) {
        fprintf(stderr, "Error en getaddrinfo: %s\n", gai_strerror(err));
        return -1;
    }
    
    // Crear el socket
    fd_escucha = socket(server_info->ai_family, 
                       server_info->ai_socktype, 
                       server_info->ai_protocol);
    if (fd_escucha == -1) {
        perror("Error creando socket");
        freeaddrinfo(server_info);
        return -1;
    }
    
    // Permitir reutilizar el puerto
    int opt = 1;
    err = setsockopt(fd_escucha, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(int));
    if (err == -1) {
        perror("Error en setsockopt");
        close(fd_escucha);
        freeaddrinfo(server_info);
        return -1;
    }
    
    // Asociar el socket al puerto
    err = bind(fd_escucha, server_info->ai_addr, server_info->ai_addrlen);
    if (err == -1) {
        perror("Error en bind");
        close(fd_escucha);
        freeaddrinfo(server_info);
        return -1;
    }
    
    // Marcar el socket en modo escucha
    err = listen(fd_escucha, SOMAXCONN);
    if (err == -1) {
        perror("Error en listen");
        close(fd_escucha);
        freeaddrinfo(server_info);
        return -1;
    }
    
    freeaddrinfo(server_info);
    printf("Servidor iniciado en puerto %d\n", puerto);
    return fd_escucha;
}

// Espera una conexión entrante en el socket del servidor
// Retorna: file descriptor de la nueva conexión, o -1 en error
int esperarConexion(int socket_servidor) {
    int fd_conexion;
    
    // accept() es bloqueante, espera hasta que se conecte un cliente
    fd_conexion = accept(socket_servidor, NULL, NULL);
    
    if (fd_conexion == -1) {
        perror("Error en accept");
        return -1;
    }
    
    printf("Cliente conectado. FD: %d\n", fd_conexion);
    return fd_conexion;
}

// Crea una conexión cliente a un servidor
// Retorna: file descriptor de la conexión, o -1 en error
int crearConexion(char* ip, int puerto) {
    struct addrinfo hints, *server_info;
    int fd_conexion, err;
    char puerto_str[10];
    
    // Convertir el puerto a string
    snprintf(puerto_str, sizeof(puerto_str), "%d", puerto);
    
    // Configurar hints para obtener información de red
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    
    // Obtener información de red del servidor
    err = getaddrinfo(ip, puerto_str, &hints, &server_info);
    if (err != 0) {
        fprintf(stderr, "Error en getaddrinfo: %s\n", gai_strerror(err));
        return -1;
    }
    
    // Crear el socket
    fd_conexion = socket(server_info->ai_family,
                        server_info->ai_socktype,
                        server_info->ai_protocol);
    if (fd_conexion == -1) {
        perror("Error creando socket");
        freeaddrinfo(server_info);
        return -1;
    }
    
    // Conectar al servidor
    err = connect(fd_conexion, server_info->ai_addr, server_info->ai_addrlen);
    if (err == -1) {
        perror("Error en connect");
        close(fd_conexion);
        freeaddrinfo(server_info);
        return -1;
    }
    
    freeaddrinfo(server_info);
    printf("Conectado al servidor %s:%d\n", ip, puerto);
    return fd_conexion;
}

void saludar(char* quien) {
    printf("Hola desde %s!!\n", quien);
}