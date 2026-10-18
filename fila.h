#ifndef FILA_H
#define FILA_H

#include <stdbool.h>
#include <stddef.h>

#include "item.h"
#include "consulta.h"
#include "percurso.h"

typedef struct fila FILA;

// cria uma fila vazia e retorna null se nao tiver memoria
FILA *fila_criar(const CONSULTA *consulta);

// insere no fim da fila, retorna false se nao tiver memoria ou fila invalida
bool fila_inserir(FILA *fila, ITEM *item);

// remove e retorna o primeiro item da fila
ITEM *fila_remover(FILA *fila);

// apenas consulta o primiero item sem remover
ITEM *fila_frente(const FILA *fila);
// verifica e retorna true se a fila estiver vazia
bool fila_vazia(const FILA *fila);

//retorna o tamanho da fila
size_t fila_tamanho(const FILA *fila);

// visita todos os itens
bool fila_percorrer(
    const FILA *fila,
    VISITA visitar,
    void *contexto
);

// libera os nos e a fila
void fila_apagar(FILA *fila, bool apagar_itens);

#endif