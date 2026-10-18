#ifndef ITEM_H
#define ITEM_H

#include <stdbool.h>
#include <stddef.h>

typedef struct item ITEM;
typedef enum tipo_campo { CAMPO_TEXTO, CAMPO_NUMERO, CAMPO_BOOLEANO } TIPO_CAMPO;


// cria um item copiando os nomes e os valores do registro
ITEM *item_criar(
    const char *const nomes[],
    const char *const valores[],
    size_t quantidade,
    const char *registro_original
);

// libera o item
void item_apagar(ITEM * *item);

// retorna o texto de um campo ou null se nao existir
// aspas vazias se existir mas tiver vazio
const char *item_obter_texto(
    const ITEM *item,
    const char *campo
);

// pega os campos numehricos
bool item_obter_numero(
    const ITEM *item,
    const char *campo,
    double *saida
);

// obtem um campo booleano e transforma no true ou false da linguagem c
bool item_obter_booleano(
    const ITEM *item,
    const char *campo,
    bool *saida
);

// define a similaridade
void item_definir_similaridade(ITEM *item, double similaridade);
//verifica a similaridade
double item_obter_similaridade(const ITEM *item);

// retorna o texto original
const char *item_obter_registro_original(const ITEM *item);

#endif