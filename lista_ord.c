#include "lista_ord.h"
#include <stdlib.h>

typedef struct no { ITEM *item; struct no *proximo; } NO;

struct lista_ord { NO *inicio, *fim; const CONSULTA *consulta; };

LISTA_ORD *lista_ord_criar(const CONSULTA *consulta) {
    LISTA_ORD *e = calloc(1, sizeof *e);
    if (!e) return NULL;
    e->consulta = consulta;

    return e;
}

bool lista_ord_inserir(LISTA_ORD *e, ITEM *item) {
    if (!e || !item) return false;
    NO *novo = malloc(sizeof *novo);
    if (!novo) return false;
    novo->item = item; novo->proximo = NULL;
    // Encontra a posicao correta antes de inserir.
    NO **pos = &e->inicio;
    while (*pos && consulta_comparar(e->consulta, (*pos)->item, item) <= 0)
        pos = &(*pos)->proximo;
    novo->proximo = *pos;
    *pos = novo;
    if (!novo->proximo) e->fim = novo;
    return true;
}

ITEM *lista_ord_remover(LISTA_ORD *e) {

    if (!e || !e->inicio) return NULL;
    NO *no = e->inicio;
    ITEM *item = no->item;
    e->inicio = no->proximo;
    if (!e->inicio) e->fim = NULL;
    free(no); return item;
}

bool lista_ord_percorrer(const LISTA_ORD *e, VISITA visitar, void *contexto) {
    if (!e || !visitar) return false;
    for (NO *no = e->inicio; no; no = no->proximo)
        if (!visitar(no->item, contexto)) return false;
    return true;
}

void lista_ord_apagar(LISTA_ORD *e, bool apagar_itens) {
    if (!e) return;
    ITEM *item;
    while ((item = lista_ord_remover(e))) if (apagar_itens) item_apagar(&item);

    free(e);
}
