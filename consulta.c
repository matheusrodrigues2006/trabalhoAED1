#include "consulta.h"
#include "util.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <ctype.h>

typedef struct { char *campo, *operador, *valor; } FILTRO;
typedef struct { char *campo; double alvo, tolerancia, peso; } SIM;
struct consulta {
    FILTRO *filtros;
    size_t quantidade_filtros;
    SIM *sim;
    size_t quantidade_sim;
    double limiar;
    char *campo_ordem;
    bool decrescente;
    char **grupos;
    size_t quantidade_grupos;
    char *cruzamento[2];
};

static bool operador_valido(const char *op) {
    return !strcmp(op, "==") || !strcmp(op, "!=") || !strcmp(op, "<") ||
           !strcmp(op, "<=") || !strcmp(op, ">") || !strcmp(op, ">=");
}

static bool comparar_operador(int cmp, const char *op) {
    if (!strcmp(op, "==")) return cmp == 0;
    if (!strcmp(op, "!=")) return cmp != 0;
    if (!strcmp(op, "<")) return cmp < 0;
    if (!strcmp(op, "<=")) return cmp <= 0;
    if (!strcmp(op, ">")) return cmp > 0;
    return cmp >= 0;
}

static bool processar(CONSULTA *c, char * *p, size_t n) {
    if (!strcmp(p[0], "FILTER") && n == 4 && operador_valido(p[2])) {
        FILTRO *novo = realloc(c->filtros, (c->quantidade_filtros + 1) * sizeof *novo);
        if (!novo) return false;
        c->filtros = novo;
        FILTRO *f = &novo[c->quantidade_filtros++];
        f->campo = texto_copiar(p[1]); f->operador = texto_copiar(p[2]); f->valor = texto_copiar(p[3]);
        return f->campo && f->operador && f->valor;
    }
    if (!strcmp(p[0], "SIM") && n == 5) {
        double alvo, tol, peso;
        if (!texto_numero(p[2], &alvo) || !texto_numero(p[3], &tol) ||
            !texto_numero(p[4], &peso) || tol <= 0 || peso <= 0) return false;
        SIM *novo = realloc(c->sim, (c->quantidade_sim + 1) * sizeof *novo);
        if (!novo) return false;
        c->sim = novo;
        SIM *s = &novo[c->quantidade_sim++];
        s->campo = texto_copiar(p[1]); s->alvo = alvo; s->tolerancia = tol; s->peso = peso;
        return s->campo != NULL;
    }
    if (!strcmp(p[0], "ACCEPT") && n == 2) {
        return texto_numero(p[1], &c->limiar) && c->limiar >= 0 && c->limiar <= 1;
    }
    if (!strcmp(p[0], "ORDER") && n == 3 && (!strcmp(p[2], "ASC") || !strcmp(p[2], "DESC"))) {
        free(c->campo_ordem); c->campo_ordem = texto_copiar(p[1]);
        c->decrescente = !strcmp(p[2], "DESC"); return c->campo_ordem != NULL;
    }
    if (!strcmp(p[0], "GROUP") && n >= 2 && !c->grupos) {
        c->grupos = calloc(n - 1, sizeof *c->grupos);
        if (!c->grupos) return false;
        c->quantidade_grupos = n - 1;
        for (size_t i = 1; i < n; i++) if (!(c->grupos[i - 1] = texto_copiar(p[i]))) return false;
        return true;
    }
    if (!strcmp(p[0], "CROSS") && n == 3 && !c->cruzamento[0]) {
        c->cruzamento[0] = texto_copiar(p[1]); c->cruzamento[1] = texto_copiar(p[2]);
        return c->cruzamento[0] && c->cruzamento[1];
    }
    return false;
}

CONSULTA *consulta_carregar(const char *caminho, char *erro, size_t capacidade) {
    FILE *arq = fopen(caminho, "r");
    CONSULTA *c = calloc(1, sizeof *c);
    size_t linha = 0;
    if (!arq || !c) goto falha;
    c->decrescente = true;
    char buffer[4096];
    while (fgets(buffer, sizeof buffer, arq)) {
        linha++;
        size_t tam = strlen(buffer);
        if (tam == sizeof buffer - 1 && buffer[tam - 1] != '\n') goto falha;
        while (tam && (buffer[tam - 1] == '\n' || buffer[tam - 1] == '\r')) buffer[--tam] = '\0';
        if (!tam || buffer[0] == '#') continue;
        char *partes[32]; size_t n = 1; partes[0] = buffer;
        for (size_t i = 0; i < tam; i++) if (buffer[i] == '|') {
            if (n == 32) goto falha;
            buffer[i] = '\0'; partes[n++] = &buffer[i + 1];
        }
        for (size_t i = 0; i < n; i++) {
            while (isspace((unsigned char) * partes[i])) partes[i]++;
            size_t k = strlen(partes[i]);
            while (k && isspace((unsigned char)partes[i][k - 1])) partes[i][--k] = '\0';
            if (!k) goto falha;
        }
        if (!processar(c, partes, n)) goto falha;
    }
    if (ferror(arq)) goto falha;
    fclose(arq); return c;
falha:
    if (erro && capacidade) snprintf(erro, capacidade, "Consulta invalida ou falha de leitura (linha %zu)", linha);
    if (arq) fclose(arq);
    consulta_apagar(&c); return NULL;
}

bool consulta_validar(const CONSULTA *c, const LEITOR *l) {
    if (!c || !l) return false;
    for (size_t i = 0; i < c->quantidade_filtros; i++) {
        FILTRO *f = &c->filtros[i]; double x; bool b;
        if (!leitor_tem_campo(l, f->campo)) return false;
        TIPO_CAMPO tipo = leitor_tipo(l, f->campo);
        if (tipo == CAMPO_NUMERO && !texto_numero(f->valor, &x)) return false;
        if (tipo == CAMPO_BOOLEANO && (!texto_booleano(f->valor, &b) ||
            (strcmp(f->operador, "==") && strcmp(f->operador, "!=")))) return false;
    }
    double pesos = 0;
    for (size_t i = 0; i < c->quantidade_sim; i++) {
        if (!leitor_tem_campo(l, c->sim[i].campo) || leitor_tipo(l, c->sim[i].campo) != CAMPO_NUMERO) return false;
        pesos += c->sim[i].peso;
    }
    if (!isfinite(pesos)) return false;
    if (c->campo_ordem && strcmp(c->campo_ordem, "similarity") && !leitor_tem_campo(l, c->campo_ordem)) return false;
    for (size_t i = 0; i < c->quantidade_grupos; i++) if (!leitor_tem_campo(l, c->grupos[i])) return false;
    for (size_t i = 0; i < 2; i++) if (c->cruzamento[i] && !leitor_tem_campo(l, c->cruzamento[i])) return false;
    return true;
}

bool consulta_avaliar(const CONSULTA *c, const ITEM *item, double *similaridade) {
    if (similaridade) * similaridade = 0;
    if (!c || !item) return false;
    for (size_t i = 0; i < c->quantidade_filtros; i++) {
        FILTRO *f = &c->filtros[i];
        const char *texto = item_obter_texto(item, f->campo);
        if (!texto || !*texto) return false;
        int cmp;
        TIPO_CAMPO tipo = item_tipo(item, f->campo);
        if (tipo == CAMPO_NUMERO) {
            double x, y;
            if (!item_obter_numero(item, f->campo, &x) || !texto_numero(f->valor, &y)) return false;
            cmp = (x > y) - (x < y);
        } else if (tipo == CAMPO_BOOLEANO) {
            bool x, y;
            if (!item_obter_booleano(item, f->campo, &x) || !texto_booleano(f->valor, &y)) return false;
            cmp = (int)x - (int)y;
        } else cmp = strcmp(texto, f->valor);
        if (!comparar_operador(cmp, f->operador)) return false;
    }
    double soma = 0, pesos = 0;
    for (size_t i = 0; i < c->quantidade_sim; i++) {
        SIM *s = &c->sim[i]; double x, nota = 0;
        if (item_obter_numero(item, s->campo, &x)) nota = fmax(0, 1 - fabs(x - s->alvo) / s->tolerancia);
        soma += nota * s->peso; pesos += s->peso;
    }
    double nota = pesos ? soma / pesos : 1;
    if (similaridade) * similaridade = nota;
    return !c->quantidade_sim || nota >= c->limiar;
}

const char *consulta_campo_ordem(const CONSULTA *c) { return c ? c->campo_ordem : NULL; }

bool consulta_ordem_decrescente(const CONSULTA *c) { return c && c->decrescente; }

size_t consulta_total_grupos(const CONSULTA *c) { return c ? c->quantidade_grupos : 0; }

const char *consulta_campo_grupo(const CONSULTA *c, size_t i) { return c && i < c->quantidade_grupos ? c->grupos[i] : NULL; }

const char *consulta_campo_cruzamento(const CONSULTA *c, size_t i) { return c && i < 2 ? c->cruzamento[i] : NULL; }

int consulta_comparar(const CONSULTA *c, const ITEM *a, const ITEM *b) {
    const char *campo = c->campo_ordem;
    int cmp;
    if (!campo || !strcmp(campo, "similarity")) {
        double x = item_obter_similaridade(a), y = item_obter_similaridade(b);
        cmp = (x > y) - (x < y);
    } else {
        const char *x = item_obter_texto(a, campo), *y = item_obter_texto(b, campo);
        bool vx = x && *x, vy = y && *y;
        TIPO_CAMPO tipo = item_tipo(a, campo);
        if (tipo == CAMPO_NUMERO) {
            double nx = 0, ny = 0; vx = item_obter_numero(a, campo, &nx); vy = item_obter_numero(b, campo, &ny);
            cmp = (nx > ny) - (nx < ny);
        } else if (tipo == CAMPO_BOOLEANO) {
            bool bx = false, by = false; vx = item_obter_booleano(a, campo, &bx); vy = item_obter_booleano(b, campo, &by);
            cmp = (int)bx - (int)by;
        } else cmp = vx && vy ? strcmp(x, y) : 0;
        if (!vx || !vy) return vx ? -1 : vy ? 1 : 0;
    }
    return c->decrescente ? -cmp : cmp;
}

void consulta_apagar(CONSULTA * *c) {
    if (!c || !*c) return;
    for (size_t i = 0; i < (*c)->quantidade_filtros; i++) {
        free((*c)->filtros[i].campo); free((*c)->filtros[i].operador); free((*c)->filtros[i].valor);
    }
    for (size_t i = 0; i < (*c)->quantidade_sim; i++) free((*c)->sim[i].campo);
    for (size_t i = 0; i < (*c)->quantidade_grupos; i++) free((*c)->grupos[i]);
    free((*c)->filtros); free((*c)->sim); free((*c)->grupos); free((*c)->campo_ordem);
    free((*c)->cruzamento[0]); free((*c)->cruzamento[1]); free(*c); *c = NULL;
}
