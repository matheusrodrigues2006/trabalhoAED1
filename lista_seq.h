#ifndef LISTA_SEQ_H
#define LISTA_SEQ_H

#include "consulta.h"
#include "percurso.h"

typedef struct lista_seq LISTA_SEQ;

LISTA_SEQ *lista_seq_criar(const CONSULTA *consulta);
bool lista_seq_inserir(LISTA_SEQ *estrutura, ITEM *item);
ITEM *lista_seq_remover(LISTA_SEQ *estrutura);
bool lista_seq_percorrer(const LISTA_SEQ *estrutura, VISITA visitar, void *contexto);
void lista_seq_apagar(LISTA_SEQ *estrutura, bool apagar_itens);


#endif
