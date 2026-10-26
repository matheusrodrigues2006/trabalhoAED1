#ifndef PERCURSO_H
#define PERCURSO_H
#include "item.h"

// A visita retorna false para interromper o percurso.
typedef bool (*VISITA)(ITEM *item, void *contexto);
#endif
