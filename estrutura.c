#include "estrutura.h"
#include <stdlib.h>
#include <string.h>
#include "pilha.h"
#include "fila.h"
#include "lista_seq.h"
#include "lista_enc.h"
#include "lista_cabeca.h"
#include "lista_ord.h"
#include "lista_gen.h"
#include "lista_cruz.h"

struct estrutura { int tipo; void *dados; };

ESTRUTURA *estrutura_criar(const char *nome, const CONSULTA *consulta) {
    if (!nome || !consulta) return NULL;
    ESTRUTURA *e = calloc(1, sizeof *e);
    if (!e) return NULL;
    if (!strcmp(nome, "pilha")) { e->tipo = 0; e->dados = pilha_criar(consulta); }
    else if (!strcmp(nome, "fila")) { e->tipo = 1; e->dados = fila_criar(consulta); }
    else if (!strcmp(nome, "lista_seq")) { e->tipo = 2; e->dados = lista_seq_criar(consulta); }
    else if (!strcmp(nome, "lista_enc")) { e->tipo = 3; e->dados = lista_enc_criar(consulta); }
    else if (!strcmp(nome, "lista_cabeca")) { e->tipo = 4; e->dados = lista_cabeca_criar(consulta); }
    else if (!strcmp(nome, "lista_ord")) { e->tipo = 5; e->dados = lista_ord_criar(consulta); }
    else if (!strcmp(nome, "lista_gen")) { e->tipo = 6; e->dados = lista_gen_criar(consulta); }
    else if (!strcmp(nome, "lista_cruz")) { e->tipo = 7; e->dados = lista_cruz_criar(consulta); }
    if (!e->dados) { free(e); return NULL; }
    return e;
}

bool estrutura_inserir(ESTRUTURA *e, ITEM *item) {
    if (!e) return false;
    switch (e->tipo) {
        case 0: return pilha_inserir(e->dados, item);
        case 1: return fila_inserir(e->dados, item);
        case 2: return lista_seq_inserir(e->dados, item);
        case 3: return lista_enc_inserir(e->dados, item);
        case 4: return lista_cabeca_inserir(e->dados, item);
        case 5: return lista_ord_inserir(e->dados, item);
        case 6: return lista_gen_inserir(e->dados, item);
        case 7: return lista_cruz_inserir(e->dados, item);
        default: return false;
    }
}

ITEM *estrutura_remover(ESTRUTURA *e) {
    if (!e) return NULL;
    switch (e->tipo) {
        case 0: return pilha_remover(e->dados);
        case 1: return fila_remover(e->dados);
        case 2: return lista_seq_remover(e->dados);
        case 3: return lista_enc_remover(e->dados);
        case 4: return lista_cabeca_remover(e->dados);
        case 5: return lista_ord_remover(e->dados);
        case 6: return lista_gen_remover(e->dados);
        case 7: return lista_cruz_remover(e->dados);
        default: return NULL;
    }
}

bool estrutura_percorrer(const ESTRUTURA *e, VISITA visitar, void *contexto) {
    if (!e) return false;
    switch (e->tipo) {
        case 0: return pilha_percorrer(e->dados, visitar, contexto);
        case 1: return fila_percorrer(e->dados, visitar, contexto);
        case 2: return lista_seq_percorrer(e->dados, visitar, contexto);
        case 3: return lista_enc_percorrer(e->dados, visitar, contexto);
        case 4: return lista_cabeca_percorrer(e->dados, visitar, contexto);
        case 5: return lista_ord_percorrer(e->dados, visitar, contexto);
        case 6: return lista_gen_percorrer(e->dados, visitar, contexto);
        case 7: return lista_cruz_percorrer(e->dados, visitar, contexto);
        default: return false;
    }
}

void estrutura_apagar(ESTRUTURA **e, bool apagar_itens) {
    if (!e || !*e) return;
    switch ((*e)->tipo) {
        case 0: pilha_apagar((*e)->dados, apagar_itens); break;
        case 1: fila_apagar((*e)->dados, apagar_itens); break;
        case 2: lista_seq_apagar((*e)->dados, apagar_itens); break;
        case 3: lista_enc_apagar((*e)->dados, apagar_itens); break;
        case 4: lista_cabeca_apagar((*e)->dados, apagar_itens); break;
        case 5: lista_ord_apagar((*e)->dados, apagar_itens); break;
        case 6: lista_gen_apagar((*e)->dados, apagar_itens); break;
        case 7: lista_cruz_apagar((*e)->dados, apagar_itens); break;
    }
    free(*e); *e = NULL;
}
