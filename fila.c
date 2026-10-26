#include "fila.h"
#include <stdlib.h>

typedef struct no { ITEM *item; struct no *proximo; } NO;
struct fila { NO *inicio, *fim; const CONSULTA *consulta; };

FILA *fila_criar(const CONSULTA *consulta) {
    FILA *e = calloc(1, sizeof *e);
    if (!e) return NULL;
    e->consulta = consulta;

    return e;
}

// Funções de inserção, remoção, percorrimento e liberação de memória para a fila
bool fila_inserir(FILA *e, ITEM *item) {
    if (!e || !item) return false;
    NO *novo = malloc(sizeof *novo);
    if (!novo) return false;
    novo->item = item; novo->proximo = NULL;
    if (e->fim) e->fim->proximo = novo;
    else e->inicio = novo;
    e->fim = novo;
    return true;
}

ITEM *fila_remover(FILA *e) {
    if (!e || !e->inicio) return NULL;
    NO *no = e->inicio;
    ITEM *item = no->item;
    e->inicio = no->proximo;
    if (!e->inicio) e->fim = NULL;
    free(no); return item;
}

bool fila_percorrer(const FILA *e, VISITA visitar, void *contexto) {
    if (!e || !visitar) return false;
    for (NO *no = e->inicio; no; no = no->proximo)
        if (!visitar(no->item, contexto)) return false;
    return true;
}

void fila_apagar(FILA *e, bool apagar_itens) {
    if (!e) return;
    ITEM *item;
    while ((item = fila_remover(e))) if (apagar_itens) item_apagar(&item);

    free(e);
}
