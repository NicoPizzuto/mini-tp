#ifndef MEMORIA_H_
#define MEMORIA_H_
#include <utils.h>

typedef struct {
    int cant_paginas;
    int* entradas;
} tabla_de_paginas;

void obtenerConfig();
char* leerMemoria(int direccion, int tamanio);
void escribirMemoria(int direccion, char* contenido);
void crearBitMap();
bool reservarMemoriaParaProceso(int pid, int tamanio);
char* deIntAString(int entero);
int contarDigitos(int numero);
void liberarProceso(int pid);
int paginaFisicaDeLogica(int pid, int pagina_logica);

#endif 