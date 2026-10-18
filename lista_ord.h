#ifndef LISTA_ORD_H
#define LISTA_ORD_H
#include "consulta.h"
#include "percurso.h"

typedef struct lista_ord LISTA_ORD;
LISTA_ORD *lista_ord_criar(const CONSULTA *consulta);
bool lista_ord_inserir(LISTA_ORD *estrutura, ITEM *item);
ITEM *lista_ord_remover(LISTA_ORD *estrutura);
bool lista_ord_percorrer(const LISTA_ORD *estrutura, VISITA visitar, void *contexto);
void lista_ord_apagar(LISTA_ORD *estrutura, bool apagar_itens);
#endif
