#include "util.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>


char *texto_copiar(const char *texto) {
    if (!texto) return NULL;
    char *copia = malloc(strlen(texto) + 1);
    if (copia) strcpy(copia, texto);
    return copia;
}


bool texto_numero(const char *texto, double *saida) {
    if (!texto || !saida) return false;
    char *fim;
    errno = 0;
    double valor = strtod(texto, &fim);
    if (fim == texto || errno == ERANGE || !isfinite(valor)) return false;
    while (isspace((unsigned char) * fim)) fim++;
    if (*fim) return false;
    *saida = valor;
    return true;
}

bool texto_booleano(const char *texto, bool *saida) {
    if (!texto || !saida) return false;
    char copia[6];
    size_t n = strlen(texto);
    if (n >= sizeof copia) return false;
    for (size_t i = 0; i <= n; i++) copia[i] = (char)tolower((unsigned char)texto[i]);
    if (!strcmp(copia, "true")) { *saida = true; return true; }
    if (!strcmp(copia, "false")) { *saida = false; return true; }
    return false;
}
