#ifndef UTIL_H
#define UTIL_H
#include <stdbool.h>

char *texto_copiar(const char *texto);
bool texto_numero(const char *texto, double *saida);
bool texto_booleano(const char *texto, bool *saida);

#endif
