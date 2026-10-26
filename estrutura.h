#ifndef ESTRUTURA_H
#define ESTRUTURA_H
#include "consulta.h"
#include "percurso.h"

typedef struct estrutura ESTRUTURA;
ESTRUTURA *estrutura_criar(const char *nome, const CONSULTA *consulta);
bool estrutura_inserir(ESTRUTURA *estrutura, ITEM *item);
ITEM *estrutura_remover(ESTRUTURA *estrutura);
bool estrutura_percorrer(const ESTRUTURA *estrutura, VISITA visitar, void *contexto);
// apagar_itens deve ser true somente para a estrutura dona dos registros.
void estrutura_apagar(ESTRUTURA **estrutura, bool apagar_itens);
#endif
