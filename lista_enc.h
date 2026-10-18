#ifndef LISTA_ENC_H
#define LISTA_ENC_H
#include "consulta.h"
#include "percurso.h"

typedef struct lista_enc LISTA_ENC;

LISTA_ENC *lista_enc_criar(const CONSULTA *consulta);
bool lista_enc_inserir(LISTA_ENC *estrutura, ITEM *item);
ITEM *lista_enc_remover(LISTA_ENC *estrutura);
bool lista_enc_percorrer(const LISTA_ENC *estrutura, VISITA visitar, void *contexto);
void lista_enc_apagar(LISTA_ENC *estrutura, bool apagar_itens);

#endif
