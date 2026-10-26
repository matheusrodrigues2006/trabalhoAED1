#ifndef LISTA_GEN_H
#define LISTA_GEN_H
#include "consulta.h"
#include "percurso.h"

typedef struct lista_gen LISTA_GEN;
LISTA_GEN *lista_gen_criar(const CONSULTA *consulta);
bool lista_gen_inserir(LISTA_GEN *estrutura, ITEM *item);
ITEM *lista_gen_remover(LISTA_GEN *estrutura);
bool lista_gen_percorrer(const LISTA_GEN *estrutura, VISITA visitar, void *contexto);
void lista_gen_apagar(LISTA_GEN *estrutura, bool apagar_itens);
#endif
