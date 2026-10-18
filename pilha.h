#ifndef PILHA_H
#define PILHA_H
#include "consulta.h"
#include "percurso.h"

typedef struct pilha PILHA;

PILHA *pilha_criar(const CONSULTA *consulta);
bool pilha_inserir(PILHA *estrutura, ITEM *item);
ITEM *pilha_remover(PILHA *estrutura);
bool pilha_percorrer(const PILHA *estrutura, VISITA visitar, void *contexto);
void pilha_apagar(PILHA *estrutura, bool apagar_itens);

#endif
