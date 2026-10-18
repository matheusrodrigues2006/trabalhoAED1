#ifndef LISTA_CRUZ_H
#define LISTA_CRUZ_H
#include "consulta.h"
#include "percurso.h"

typedef struct lista_cruz LISTA_CRUZ;

LISTA_CRUZ *lista_cruz_criar(const CONSULTA *consulta);
bool lista_cruz_inserir(LISTA_CRUZ *estrutura, ITEM *item);
ITEM *lista_cruz_remover(LISTA_CRUZ *estrutura);
bool lista_cruz_percorrer(const LISTA_CRUZ *estrutura, VISITA visitar, void *contexto);
void lista_cruz_apagar(LISTA_CRUZ *estrutura, bool apagar_itens);
bool lista_cruz_percorrer_eixo(const LISTA_CRUZ *lista, size_t eixo, VISITA visitar, void *contexto);

#endif
