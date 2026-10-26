#ifndef CONSULTA_H
#define CONSULTA_H

#include <stdbool.h>
#include <stddef.h>

#include "item.h"
#include "leitor.h"

typedef struct consulta CONSULTA;

// le o arquivo e cria a consulta, retornando NULL se tiver erro
CONSULTA *consulta_carregar(
    const char *caminho,
    char *erro,
    size_t capacidade_erro
);

// libera a memoria da consulta e coloca o ponteiro em NULL
void consulta_apagar(CONSULTA **consulta);

// Aplica os filtros e o limiar de similaridade; informa a nota calculada.
bool consulta_avaliar(
    const CONSULTA *consulta,
    const ITEM *item,
    double *similaridade
);

// obtem o campo de order
const char *consulta_campo_ordem(const CONSULTA *consulta);

// verifica se a ordem é decrescente, retornando true se sim
bool consulta_ordem_decrescente(const CONSULTA *consulta);

// quantidade de campos de GROUP, retornando 0 se nao houver GROUP
size_t consulta_total_grupos(const CONSULTA *consulta);

// obtem um campo de group pelo indice
const char *consulta_campo_grupo(
    const CONSULTA *consulta,
    size_t indice
);

/*
Obtém um campo de CROSS:
indice 0 = primeira organização;
indice 1 = segunda organização.
Retorna NULL se CROSS estiver ausente ou o índice for inválido.
 */
const char *consulta_campo_cruzamento(
    const CONSULTA *consulta,
    size_t indice
);

bool consulta_validar(
    const CONSULTA *consulta,
    const LEITOR *leitor
);

int consulta_comparar(
    const CONSULTA *consulta,
    const ITEM *a,
    const ITEM *b
);

#endif
