#include "lista_enc.h"
#include <stdlib.h>

typedef struct no { ITEM *item; struct no *proximo; } NO;

struct lista_enc { NO *inicio, *fim; const CONSULTA *consulta; };

LISTA_ENC *lista_enc_criar(const CONSULTA *consulta) {
    LISTA_ENC *e = calloc(1, sizeof *e);
    if (!e) return NULL;
    e->consulta = consulta;

    return e;
}

bool lista_enc_inserir(LISTA_ENC *e, ITEM *item) {
    if (!e || !item) return false;
    NO *novo = malloc(sizeof *novo);
    if (!novo) return false;
    novo->item = item; novo->proximo = NULL;
    if (e->fim) e->fim->proximo = novo;
    else e->inicio = novo;
    e->fim = novo;
    return true;
}

ITEM *lista_enc_remover(LISTA_ENC *e) {
    if (!e || !e->inicio) return NULL;
    NO *no = e->inicio;
    ITEM *item = no->item;
    e->inicio = no->proximo;
    if (!e->inicio) e->fim = NULL;
    free(no); return item;
}

bool lista_enc_percorrer(const LISTA_ENC *e, VISITA visitar, void *contexto) {
    if (!e || !visitar) return false;
    for (NO *no = e->inicio; no; no = no->proximo)
        if (!visitar(no->item, contexto)) return false;
    return true;
}

void lista_enc_apagar(LISTA_ENC *e, bool apagar_itens) {
    if (!e) return;
    ITEM *item;
    while ((item = lista_enc_remover(e))) if (apagar_itens) item_apagar(&item);

    free(e);
}
