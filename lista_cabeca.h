#ifndef LISTA_CABECA_H
#define LISTA_CABECA_H
#include "consulta.h"
#include "percurso.h"

typedef struct lista_cabeca LISTA_CABECA;
LISTA_CABECA *lista_cabeca_criar(const CONSULTA *consulta);
bool lista_cabeca_inserir(LISTA_CABECA *estrutura, ITEM *item);
ITEM *lista_cabeca_remover(LISTA_CABECA *estrutura);
bool lista_cabeca_percorrer(const LISTA_CABECA *estrutura, VISITA visitar, void *contexto);
void lista_cabeca_apagar(LISTA_CABECA *estrutura, bool apagar_itens);

#endif
