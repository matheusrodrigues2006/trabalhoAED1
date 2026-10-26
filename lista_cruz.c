#include "lista_cruz.h"
#include "util.h"
#include <stdlib.h>
#include <string.h>

typedef struct no {
    ITEM *item;
    struct no *proximo[2];
} NO;
typedef struct grupo {
    char *chave;
    NO *inicio, *fim;
    struct grupo *proximo;
} GRUPO;
struct lista_cruz { GRUPO *eixos[2]; const CONSULTA *consulta; };

LISTA_CRUZ *lista_cruz_criar(const CONSULTA *consulta) {
    if (!consulta_campo_cruzamento(consulta, 0) || !consulta_campo_cruzamento(consulta, 1)) return NULL;
    LISTA_CRUZ *l = calloc(1, sizeof *l);
    if (l) l->consulta = consulta;
    return l;
}

static GRUPO *obter_grupo(GRUPO **inicio, const char *chave) {

    while (*inicio && strcmp((*inicio)->chave, chave)) inicio = &(*inicio)->proximo;
    if (!*inicio) {
        GRUPO *novo = calloc(1, sizeof *novo);
        if (!novo) return NULL;
        novo->chave = texto_copiar(chave);
        if (!novo->chave) { free(novo); return NULL; }
        * inicio = novo;
    }
    return *inicio;
}

bool lista_cruz_inserir(LISTA_CRUZ *l, ITEM *item) {
    if (!l || !item) return false;
    GRUPO *grupos[2];
    for (size_t i = 0; i < 2; i++) {
        const char *chave = item_obter_texto(item, consulta_campo_cruzamento(l->consulta, i));
        grupos[i] = obter_grupo(&l->eixos[i], chave ? chave : "");
        if (!grupos[i]) return false;
    }
    NO *novo = calloc(1, sizeof *novo);
    if (!novo) return false;
    novo->item = item;
    /* O mesmo no participa das duas cadeias. */
    for (size_t i = 0; i < 2; i++) {
        if (grupos[i]->fim) grupos[i]->fim->proximo[i] = novo;
        else grupos[i]->inicio = novo;
        grupos[i]->fim = novo;
    }
    return true;
}

bool lista_cruz_percorrer_eixo(const LISTA_CRUZ *l, size_t eixo, VISITA visitar, void *contexto) {
    if (!l || eixo > 1 || !visitar) return false;
    for (GRUPO *g = l->eixos[eixo]; g; g = g->proximo)
        for (NO *no = g->inicio; no; no = no->proximo[eixo])
            if (!visitar(no->item, contexto)) return false;
    return true;
}

bool lista_cruz_percorrer(const LISTA_CRUZ *l, VISITA visitar, void *contexto) {
    return lista_cruz_percorrer_eixo(l, 0, visitar, contexto);
}

ITEM *lista_cruz_remover(LISTA_CRUZ *l) {
    if (!l) return NULL;
    GRUPO *primeiro = l->eixos[0];
    while (primeiro && !primeiro->inicio) primeiro = primeiro->proximo;
    if (!primeiro) return NULL;
    NO *alvo = primeiro->inicio;
    for (size_t eixo = 0; eixo < 2; eixo++) {
        bool achou = false;
        for (GRUPO *g = l->eixos[eixo]; g && !achou; g = g->proximo) {
            NO *anterior = NULL, *no = g->inicio;
            while (no && no != alvo) { anterior = no; no = no->proximo[eixo]; }
            if (no) {
                if (anterior) anterior->proximo[eixo] = no->proximo[eixo];
                else g->inicio = no->proximo[eixo];
                if (g->fim == no) g->fim = anterior;
                achou = true;
            }
        }
    }
    ITEM *item = alvo->item; free(alvo); return item;
}

void lista_cruz_apagar(LISTA_CRUZ *l, bool apagar_itens) {
    if (!l) return;
    /* Libera os nos somente pelo primeiro eixo. */
    for (GRUPO *g = l->eixos[0]; g; g = g->proximo) {
        NO *no = g->inicio;
        while (no) {
            NO *proximo = no->proximo[0];
            if (apagar_itens) item_apagar(&no->item);
            free(no); no = proximo;
        }
    }
    for (size_t i = 0; i < 2; i++) {
        GRUPO *g = l->eixos[i];
        while (g) { GRUPO *proximo = g->proximo; free(g->chave); free(g); g = proximo; }
    }
    free(l);
}
