#include "lista_seq.h"

#include <stdlib.h>
#include <string.h>

struct lista_seq { ITEM **itens; size_t quantidade, capacidade; };

LISTA_SEQ *lista_seq_criar(const CONSULTA *consulta) {
    (void)consulta;
    return calloc(1, sizeof(LISTA_SEQ));
}

bool lista_seq_inserir(LISTA_SEQ *e, ITEM *item) {
    if (!e || !item) return false;
    if (e->quantidade == e->capacidade) {
        size_t capacidade = e->capacidade ? e->capacidade * 2 : 16;
        ITEM **novo = realloc(e->itens, capacidade * sizeof *novo);
        if (!novo) return false;
        e->itens = novo; e->capacidade = capacidade;
    }
    e->itens[e->quantidade++] = item; return true;
}

ITEM *lista_seq_remover(LISTA_SEQ *e) {
    if (!e || !e->quantidade) return NULL;
    ITEM *item = e->itens[0];
    e->quantidade--;
    memmove(e->itens, e->itens + 1, e->quantidade * sizeof *e->itens);
    return item;
}

bool lista_seq_percorrer(const LISTA_SEQ *e, VISITA visitar, void *contexto) {

    if (!e || !visitar) return false;
    for (size_t i = 0; i < e->quantidade; i++) if (!visitar(e->itens[i], contexto)) return false;
    return true;
}

void lista_seq_apagar(LISTA_SEQ *e, bool apagar_itens) {

    if (!e) return;
    if (apagar_itens) for (size_t i = 0; i < e->quantidade; i++) item_apagar(&e->itens[i]);
    free(e->itens); free(e);
}
