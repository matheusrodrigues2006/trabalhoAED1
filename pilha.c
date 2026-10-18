#include "pilha.h"
#include <stdlib.h>

typedef struct no { ITEM *item; struct no *proximo; } NO;

struct pilha { NO *inicio, *fim; const CONSULTA *consulta; };

PILHA *pilha_criar(const CONSULTA *consulta) {
    PILHA *e = calloc(1, sizeof *e);
    if (!e) return NULL;
    e->consulta = consulta;
    
    return e;
}

bool pilha_inserir(PILHA *e, ITEM *item) {
    if (!e || !item) return false;

    NO *novo = malloc(sizeof *novo);
    if (!novo) return false;
    novo->item = item; novo->proximo = NULL;
    novo->proximo = e->inicio;
    e->inicio = novo;
    if (!e->fim) e->fim = novo;
    return true;
}

ITEM *pilha_remover(PILHA *e) {
    
    if (!e || !e->inicio) return NULL;
    NO *no = e->inicio;
    ITEM *item = no->item;
    e->inicio = no->proximo;
    if (!e->inicio) e->fim = NULL;
    free(no); return item;
}


bool pilha_percorrer(const PILHA *e, VISITA visitar, void *contexto) {
    if (!e || !visitar) return false;
    for (NO *no = e->inicio; no; no = no->proximo)
        if (!visitar(no->item, contexto)) return false;
    return true;
}


void pilha_apagar(PILHA *e, bool apagar_itens) {
    if (!e) return;
    ITEM *item;
    while ((item = pilha_remover(e))) if (apagar_itens) item_apagar(&item);
    
    free(e);
}
