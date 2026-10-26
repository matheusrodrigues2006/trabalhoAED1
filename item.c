#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "item.h"
#include "util.h"

struct item {
    char **nomes;
    char **valores;
    TIPO_CAMPO *tipos; // um tipo para cada campo, para saber se é texto, número ou booleano
    size_t quantidade;
    char *registro_original;
    double similaridade;
};

ITEM *item_criar(
    const char *const nomes[],
    const char *const valores[],
    size_t quantidade,
    const char *registro_original
)
{
    if (!nomes || !valores || quantidade == 0 || !registro_original) {
        return NULL;
    }

    ITEM *item = calloc(1, sizeof(ITEM));
    if (!item) {
        return NULL;
    }

    item->nomes = calloc(quantidade, sizeof(char *));
    item->valores = calloc(quantidade, sizeof(char *));
    item->tipos = calloc(quantidade, sizeof(TIPO_CAMPO));
    item->quantidade = quantidade;
    item->registro_original = texto_copiar(registro_original);

    if (!item->nomes || !item->valores ||
        !item->tipos || !item->registro_original) {
        item_apagar(&item);
        return NULL;
    }

    for (size_t i = 0; i < quantidade; i++) {
        item->nomes[i] = texto_copiar(nomes[i]);
        item->valores[i] = texto_copiar(valores[i]);

        if (!item->nomes[i] || !item->valores[i]) {
            item_apagar(&item);
            return NULL;
        }
    }

    return item;
}

void item_apagar(ITEM **item) {
    if (!item || !*item) return;
    for (size_t i = 0; i < (*item)->quantidade; i++) {
        if ((*item)->nomes) free((*item)->nomes[i]);
        if ((*item)->valores) free((*item)->valores[i]);
    }

    free((*item)->nomes);
    free((*item)->valores);
    free((*item)->tipos);
    free((*item)->registro_original);
    free(*item);
    *item = NULL;
}

const char *item_obter_texto(const ITEM *item, const char *campo) {

    if (!item || !campo) return NULL;

    for (size_t i = 0; i < item->quantidade; i++)
        if (!strcmp(item->nomes[i], campo)) return item->valores[i];
    return NULL;
}

// definimos o tipo de um campo específico em um itempara saber se é texto, número ou booleano
void item_definir_tipo(ITEM *item, size_t indice, TIPO_CAMPO tipo) {
    if (item && indice < item->quantidade) {
        item->tipos[indice] = tipo;
    }
}

bool item_obter_numero(const ITEM *item, const char *campo, double *saida) {
    return texto_numero(item_obter_texto(item, campo), saida);
}

bool item_obter_booleano(const ITEM *item, const char *campo, bool *saida) {
    return texto_booleano(item_obter_texto(item, campo), saida);
}

TIPO_CAMPO item_tipo(const ITEM *item, const char *campo) {
    if (item && campo) for (size_t i = 0; i < item->quantidade; i++)
        if (!strcmp(item->nomes[i], campo)) return item->tipos[i];
    return CAMPO_TEXTO;
}

void item_definir_similaridade(ITEM *item, double valor) { if (item) item->similaridade = valor; }

double item_obter_similaridade(const ITEM *item) { return item ? item->similaridade : 0; }


const char *item_obter_registro_original(const ITEM *item) { return item ? item->registro_original : NULL; }

