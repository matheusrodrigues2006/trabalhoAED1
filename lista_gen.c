#include "lista_gen.h"
#include "util.h"
#include <stdlib.h>
#include <string.h>
typedef struct entrada { ITEM *item; struct entrada *proxima; } ENTRADA;
typedef struct grupo {
    char *chave;
    struct grupo *filhos, *proximo;
    ENTRADA *inicio, *fim;
} GRUPO;
struct lista_gen { GRUPO raiz; const CONSULTA *consulta; size_t niveis; };
LISTA_GEN *lista_gen_criar(const CONSULTA *consulta) {
    if (!consulta || !consulta_total_grupos(consulta)) return NULL;
    LISTA_GEN *l = calloc(1,sizeof *l);
    if (l) { l->consulta=consulta; l->niveis=consulta_total_grupos(consulta); }
    return l;
}
bool lista_gen_inserir(LISTA_GEN *l, ITEM *item) {
    if (!l || !item) return false;
    GRUPO *grupo = &l->raiz;
    for (size_t nivel=0;nivel<l->niveis;nivel++) {
        const char *chave = item_obter_texto(item,consulta_campo_grupo(l->consulta,nivel));
        if (!chave) chave="";
        GRUPO **pos=&grupo->filhos;
        while (*pos && strcmp((*pos)->chave,chave)) pos=&(*pos)->proximo;
        if (!*pos) {
            GRUPO *novo=calloc(1,sizeof *novo);
            if (!novo) return false;
            novo->chave=texto_copiar(chave);
            if (!novo->chave) { free(novo); return false; }
            *pos=novo;
        }
        grupo=*pos;
    }
    ENTRADA *nova=malloc(sizeof *nova);
    if (!nova) return false;
    nova->item=item; nova->proxima=NULL;
    if (grupo->fim) grupo->fim->proxima=nova;
    else grupo->inicio=nova;
    grupo->fim=nova; return true;
}
static bool percorrer_grupo(const GRUPO *g, VISITA visitar, void *contexto) {
    for (const GRUPO *filho=g->filhos;filho;filho=filho->proximo)
        if (!percorrer_grupo(filho,visitar,contexto)) return false;
    for (ENTRADA *e=g->inicio;e;e=e->proxima)
        if (!visitar(e->item,contexto)) return false;
    return true;
}
bool lista_gen_percorrer(const LISTA_GEN *l,VISITA visitar,void *contexto) {
    return l && visitar && percorrer_grupo(&l->raiz,visitar,contexto);
}
static ITEM *remover_grupo(GRUPO *g) {
    for (GRUPO *filho=g->filhos;filho;filho=filho->proximo) {
        ITEM *item=remover_grupo(filho);
        if (item) return item;
    }
    if (!g->inicio) return NULL;
    ENTRADA *e=g->inicio; ITEM *item=e->item;
    g->inicio=e->proxima;
    if (!g->inicio) g->fim=NULL;
    free(e); return item;
}
ITEM *lista_gen_remover(LISTA_GEN *l) { return l ? remover_grupo(&l->raiz) : NULL; }
static void apagar_grupo(GRUPO *g,bool apagar_itens) {
    GRUPO *filho=g->filhos;
    while (filho) {
        GRUPO *proximo=filho->proximo;
        apagar_grupo(filho,apagar_itens); free(filho->chave); free(filho); filho=proximo;
    }
    ENTRADA *e=g->inicio;
    while (e) {
        ENTRADA *proxima=e->proxima;
        if (apagar_itens) item_apagar(&e->item);
        free(e); e=proxima;
    }
}
void lista_gen_apagar(LISTA_GEN *l,bool apagar_itens) {
    if (l) { apagar_grupo(&l->raiz,apagar_itens); free(l); }
}
