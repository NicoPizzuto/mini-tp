#include "memoria.h"

t_log* logger;
t_log_level log_level;
t_config* config;
int puerto_escucha;
int socket_servidor;
int socket_cpu;
int socket_kernel;
int tam_memoria;
void* memoria_fisica;
int tam_pagina;
t_bitarray* bitmap;
t_dictionary* de_pid_a_tabla;
int memoria_restante;

int main(int argc, char* argv[]) {
    saludar("memoria");
    obtenerConfig();
    logger = log_create("memoria.log", "MEMORIA", true, log_level);

    /*
    socket_cpu = esperarConexion(socket_servidor);
    log_info(logger, "Se conecto cpu");
    socket_kernel = esperarConexion(socket_servidor);
    log_info(logger, "Se conecto kernel");
    */

    memoria_fisica = malloc(tam_memoria);
    crearBitMap();
    de_pid_a_tabla = dictionary_create();
    memoria_restante = tam_memoria;

    /*
    escribirMemoria(500, "holaaaa");
    leerMemoria(500, 5)

    reservarMemoriaParaProceso(0, 48);
    reservarMemoriaParaProceso(1, 48);
    liberarProceso(1);
    reservarMemoriaParaProceso(2, 30);
    log_info(logger, "La pagina 3 del proceso 2 esta en el marco %d", paginaFisicaDeLogica(2, 3));
    */

    identificarCliente();
    return 0;
}

void identificarCliente() {
    socket_servidor = iniciarServidor(puerto_escucha);
    for (int i = 0; i < 2; i++) {
        int socket_misterioso = esperarConexion(socket_servidor);
        int op_code = recibirOperacion(socket_misterioso);
        if (op_code == KERNEL) {
            socket_kernel = socket_misterioso;
        }
        else if (op_code == CPU) {
            socket_cpu = socket_misterioso;
        }
    }
}

void atenderCPU() {
    while(1) {
        int op_code = recibirOperacion(socket_cpu);
        switch (op_code) {
            case LEER:
                break;
            case ESCRIBIR:
                break;
            case PAGINA_FISICA:
                break;
        }
    }
}

void atenderKernel() {
    while(1) {
        int op_code = recibirOperacion(socket_kernel);
        switch (op_code) {
            case CARGAR_PROC:
                break;
            case DESCARGAR_PROC:
                break;
        }
    }
}

char* leerMemoria(int direccion, int tamanio) {
    char* lectura = malloc(tamanio + 1);
    memcpy(lectura, memoria_fisica + direccion, tamanio);
    lectura[tamanio] = '\0'; // *(lectura + sizeof(char) * tamanio)
    log_info(logger, "Lectura realizada. Direccion: %d. Contenido: %s", direccion, lectura);
    return lectura;
}

void escribirMemoria(int direccion, char* contenido) {
    memcpy(memoria_fisica + direccion, contenido, strlen(contenido));
    log_info(logger, "Escritura realizada. Direccion: %d. Contenido: %s", direccion, contenido);
}

void crearBitMap() {
    int cant_paginas = tam_memoria / tam_pagina;
    int tam_bitmap_en_bytes = (cant_paginas + 7) / 8; // Redondeo hacia arriba para obtener el tamaño en bytes
    char* contenido = malloc(tam_bitmap_en_bytes);
    memset(contenido, 0, tam_bitmap_en_bytes); // Inicializo el bitmap con ceros
    bitmap = bitarray_create_with_mode(contenido, tam_bitmap_en_bytes, LSB_FIRST);
}

char* deIntAString(int entero) {
    int tamanio = contarDigitos(entero);
    char* buffer = malloc(tamanio + 1);
    snprintf(buffer, tamanio + 1, "%d", entero);
    return buffer;
}

int contarDigitos(int numero) {
    if (numero == 0) return 1;
    int contador = 0;
    while (numero > 0) {
        numero /= 10;
        contador++;
    }
    return contador;
}

bool reservarMemoriaParaProceso(int pid, int tamanio) {
    // Verificar si hay memoria disponible
    if (tamanio > memoria_restante) {
        log_info(logger, "No hay memoria suficiente para el proceso %d. Requerido: %d, Disponible: %d", 
                 pid, tamanio, memoria_restante);
        return false;
    }

    // Calcular cantidad de páginas necesarias
    int cant_paginas_necesarias = (tamanio + tam_pagina - 1) / tam_pagina;
    int* paginas_asignadas = malloc(cant_paginas_necesarias * sizeof(int));
    int cant_paginas_totales = tam_memoria / tam_pagina;
    int paginas_encontradas = 0;

    // Buscar páginas libres en el bitmap y marcarlas como ocupadas
    for (int i = 0; i < cant_paginas_totales && paginas_encontradas < cant_paginas_necesarias; i++) {
        if (!bitarray_test_bit(bitmap, i)) {  // Si la página está libre
            bitarray_set_bit(bitmap, i);       // Marcar como ocupada
            paginas_asignadas[paginas_encontradas] = i;
            log_info(logger, "Reserve el marco %d para el proceso %d", i, pid);
            paginas_encontradas++;
        }
    }

    // Si no encontramos suficientes páginas libres, revertir cambios
    if (paginas_encontradas < cant_paginas_necesarias) {
        for (int i = 0; i < paginas_encontradas; i++) {
            bitarray_clean_bit(bitmap, paginas_asignadas[i]);
        }
        free(paginas_asignadas);
        log_info(logger, "No hay suficientes páginas libres para el proceso %d", pid);
        return false;
    }

    // Crear tabla de páginas
    tabla_de_paginas* tabla = malloc(sizeof(tabla_de_paginas));
    tabla->cant_paginas = cant_paginas_necesarias;
    tabla->entradas = paginas_asignadas;

    // Crear clave (pid convertido a string) y guardar en diccionario
    char* clave = deIntAString(pid);
    dictionary_put(de_pid_a_tabla, clave, tabla);
    free(clave);

    // Descontar de memoria restante
    memoria_restante -= tamanio;

    return true;
}

void liberarProceso(int pid) {
    tabla_de_paginas* tabla = dictionary_get(de_pid_a_tabla, deIntAString(pid));
    for (int i = 0; i < tabla->cant_paginas; i++) {
        bitarray_clean_bit(bitmap, tabla->entradas[i]);
    }
    memoria_restante += tabla->cant_paginas * tam_pagina;
    free(tabla->entradas);
    free(tabla);
}

int paginaFisicaDeLogica(int pid, int pagina_logica) {
    // Convertir pid a string para buscar en el diccionario
    char* clave = deIntAString(pid);
    tabla_de_paginas* tabla = (tabla_de_paginas*) dictionary_get(de_pid_a_tabla, clave);
    free(clave);
    
    // Validar que la página lógica esté dentro del rango
    if (tabla == NULL || pagina_logica >= tabla->cant_paginas || pagina_logica < 0) {
        log_info(logger, "Error: página lógica %d inválida para el proceso %d", pagina_logica, pid);
        return -1;
    }
    
    // Devolver la página física correspondiente
    int pagina_fisica = tabla->entradas[pagina_logica];
    return pagina_fisica;
}

void obtenerConfig(){
    config = config_create("memoria.config");
    puerto_escucha = config_get_int_value(config, "PUERTO_ESCUCHA");
    log_level = log_level_from_string(config_get_string_value(config, "LOG_LEVEL"));
    tam_memoria = config_get_int_value(config, "TAM_MEMORIA");
    tam_pagina = config_get_int_value(config, "TAM_PAGINA");
}
