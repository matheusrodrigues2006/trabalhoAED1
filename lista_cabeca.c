#include "lista_cabeca.h"
#include <stdlib.h>

typedef struct no { ITEM *item; struct no *proximo; } NO;

struct lista_cabeca { NO *cabeca, *fim; const CONSULTA *consulta; };

LISTA_CABECA *lista_cabeca_criar(const CONSULTA *consulta) {
    
    LISTA_CABECA *e = calloc(1, sizeof *e);
    if (!e) return NULL;
    e->consulta = consulta;
    e->cabeca = calloc(1, sizeof *e->cabeca);
    if (!e->cabeca) { free(e); return NULL; }
    e->fim = e->cabeca;
    return e;
}
bool lista_cabeca_inserir(LISTA_CABECA *e, ITEM *item) {
    if (!e || !item) return false;
    NO *novo = malloc(sizeof *novo);
    if (!novo) return false;
    novo->item = item; novo->proximo = NULL;
    e->fim->proximo = novo;
    e->fim = novo;
    return true;
}

ITEM *lista_cabeca_remover(LISTA_CABECA *e) {
    if (!e || !e->cabeca->proximo) return NULL;
    NO *no = e->cabeca->proximo;
    ITEM *item = no->item;
    e->cabeca->proximo = no->proximo;
    if (!e->cabeca->proximo) e->fim = e->cabeca;
    free(no); return item;
}

bool lista_cabeca_percorrer(const LISTA_CABECA *e, VISITA visitar, void *contexto) {
    if (!e || !visitar) return false;
    for (NO *no = e->cabeca->proximo; no; no = no->proximo)
        if (!visitar(no->item, contexto)) return false;
    return true;
}

void lista_cabeca_apagar(LISTA_CABECA *e, bool apagar_itens) {
    if (!e) return;
    ITEM *item;
    while ((item = lista_cabeca_remover(e))) if (apagar_itens) item_apagar(&item);
    free(e->cabeca);
    free(e);
}
