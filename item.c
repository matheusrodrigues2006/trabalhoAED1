#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "item.h"

struct item {
    char **nomes;
    char **valores;
    size_t quantidade;

    char *registro_original;
    double similaridade;
};

ITEM *item_criar(
    const char *const nomes[],
    const char *const valores[],
    size_t quantidade,
    const char *registro_original
) {
    ITEM *item = malloc(sizeof(ITEM));
    if (!item) {
        return NULL;
    }

    item->nomes = malloc(quantidade * sizeof(char * ));
    item->valores = malloc(quantidade * sizeof(char * ));
    if (!item->nomes || !item->valores) {
        free(item->nomes);
        free(item->valores);
        free(item);
        return NULL;
    }

    for (size_t i = 0; i < quantidade; i++) {
        item->nomes[i] = strdup(nomes[i]);
        item->valores[i] = strdup(valores[i]);
        if (!item->nomes[i] || !item->valores[i]) {
            for (size_t j = 0; j <= i; j++) {
                free(item->nomes[j]);
                free(item->valores[j]);
            }
            free(item->nomes);
            free(item->valores);
            free(item);
            return NULL;
        }
    }

    item->quantidade = quantidade;
    item->registro_original = strdup(registro_original);
    item->similaridade = 0.0;

    return item;
}

void item_apagar(ITEM * *item) {
    if (item && *item) {
        for (size_t i = 0; i < (*item)->quantidade; i++) {
            free((*item)->nomes[i]);
            free((*item)->valores[i]);
        }
        free((*item)->nomes);
        free((*item)->valores);
        free((*item)->registro_original);
        free(*item);
        *item = NULL;
    }
}

const char *item_obter_texto(const ITEM *item, const char *campo) {
    if (!item || !campo) {
        return NULL;
    }

    for (size_t i = 0; i < item->quantidade; i++) {
        if (strcmp(item->nomes[i], campo) == 0) {
            return item->valores[i];
        }
    }

    return NULL;
}

bool item_obter_numero(const ITEM *item, const char *campo, double *saida) {
    if (!item || !campo || !saida) {
        return false;
    }

    const char *valor = item_obter_texto(item, campo);
    if (!valor) {
        return false;
    }

    char *endptr;
    double numero = strtod(valor, &endptr);
    if (endptr == valor) {
        return false; // não é um número válido
    }

    * saida = numero;
    return true;
}

bool item_obter_booleano(const ITEM *item, const char *campo, bool *saida) {
    if (!item || !campo || !saida) {
        return false;
    }

    const char *valor = item_obter_texto(item, campo);
    if (!valor) {
        return false;
    }

    if (strcmp(valor, "true") == 0) {
        *saida = true;
        return true;
    } else if (strcmp(valor, "false") == 0) {
        *saida = false;
        return true;
    }

    return false; // não é um valor booleano válido
}

void definir_similaridade(ITEM *item, double similaridade) {
    if (item) {
        item->similaridade = similaridade;
    }
}

double item_obter_similaridade(const ITEM *item) {
    if (item) {
        return item->similaridade;
    }
    return 0.0; // valor padrão se o item for nulo
}

const char *item_obter_registro_original(const ITEM *item) {
    if (item) {
        return item->registro_original;
    }
    return NULL;
}




