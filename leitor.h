#ifndef LEITOR_H
#define LEITOR_H
#include <stdio.h>
#include "item.h"

typedef struct leitor LEITOR;
LEITOR *leitor_abrir(const char *caminho, const char *nome);
/* 1 = item lido, 0 = fim, -1 = erro. */
int leitor_proximo(LEITOR *leitor, ITEM **item);
bool leitor_tem_campo(const LEITOR *leitor, const char *campo);
TIPO_CAMPO leitor_tipo(const LEITOR *leitor, const char *campo);
bool leitor_cabecalho(const LEITOR *leitor, FILE *saida);
void leitor_fechar(LEITOR * *leitor);
#endif
