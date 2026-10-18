#ifndef PERCURSO_H
#define PERCURSO_H
#include "item.h"

// retornar flase interrompe o percurso, true continua
typedef bool (*VISITA)(ITEM *item, void *contexto);
#endif
