#include "leitor.h"
#include "util.h"
#include <stdlib.h>
#include <string.h>

struct leitor {
    FILE *arquivo;
    char *cabecalho;
    char **nomes;
    size_t quantidade;
    bool anp;
    char delimitador;
};

// Le um registro completo, incluindo quebras dentro de aspas
static int ler_registro(FILE *arq, char **saida) {

    size_t n = 0, capacidade = 256;

    char *texto = malloc(capacidade);
    bool aspas = false;
    int c;
    if (!texto) return -1;

    while ((c = fgetc(arq)) != EOF) {
        if (n + 1 >= capacidade) {
            capacidade *= 2;
            char *novo = realloc(texto, capacidade);
            if (!novo) { free(texto); return -1; }
            texto = novo;
        }
        texto[n++] = (char)c;
        if (c == '"') aspas = !aspas;
        if (c == '\n' && !aspas) break;
    }
    if (ferror(arq) || aspas) { free(texto); return -1; }
    if (!n) { free(texto); return 0; }
    texto[n] = '\0'; *saida = texto; return 1;
}

static void apagar_campos(char **campos, size_t n) {
    for (size_t i = 0; i < n; i++) free(campos[i]);
    free(campos);
}

static bool separar(const char *texto, char delimitador, char ***saida, size_t *quantidade) {
    size_t n = strlen(texto), pos = 0, total = 0;
    while (n && (texto[n - 1] == '\n' || texto[n - 1] == '\r')) n--;
    char **campos = NULL;
    do {
        char *valor = malloc(n + 1);
        size_t k = 0;
        if (!valor) goto erro;
        if (pos < n && texto[pos] == '"') {
            bool fechou = false; pos++;
            while (pos < n) {
                if (texto[pos] == '"') {
                    if (pos + 1 < n && texto[pos + 1] == '"') { valor[k++] = '"'; pos += 2; }
                    else { pos++; fechou = true; break; }
                } else valor[k++] = texto[pos++];
            }
            if (!fechou || (pos < n && texto[pos] != delimitador)) { free(valor); goto erro; }
        } else {
            while (pos < n && texto[pos] != delimitador) {
                if (texto[pos] == '"') { free(valor); goto erro; }
                valor[k++] = texto[pos++];
            }
        }
        valor[k] = '\0';
        char **novo = realloc(campos, (total + 1) * sizeof *campos);
        if (!novo) { free(valor); goto erro; }
        campos = novo; campos[total++] = valor;
        if (pos == n) break;
        pos++;
    } while (pos <= n);
    *saida = campos; *quantidade = total; return true;
erro:
    apagar_campos(campos, total); return false;
}

TIPO_CAMPO leitor_tipo(const LEITOR *leitor, const char *campo) {
    if (!leitor || !campo) return CAMPO_TEXTO;
    if (leitor->anp) {
        return !strcmp(campo, "ANO") || !strcmp(campo, "VENDAS")
            ? CAMPO_NUMERO : CAMPO_TEXTO;
    }
    const char *numeros[] = {"year", "month", "distance_au", "velocity_km_s", "absolute_magnitude",
        "days_until_approach", "risk_score", "panic_level", "sentry_impact_prob",
        "sentry_torino_scale", "sentry_palermo_scale", "sentry_diameter_km"};
    const char *booleanos[] = {"is_past_event", "is_future_event", "on_sentry_list"};

    for (size_t i = 0; i < sizeof numeros / sizeof *numeros; i++)
        if (!strcmp(campo, numeros[i])) return CAMPO_NUMERO;
    for (size_t i = 0; i < sizeof booleanos / sizeof *booleanos; i++)
        if (!strcmp(campo, booleanos[i])) return CAMPO_BOOLEANO;
    return CAMPO_TEXTO;
}

LEITOR *leitor_abrir(const char *caminho, const char *nome) {
    if (!caminho || !nome || (strcmp(nome, "nasa") && strcmp(nome, "anp"))) return NULL;
    LEITOR *l = calloc(1, sizeof *l);
    if (!l) return NULL;
    l->anp = !strcmp(nome, "anp");
    l->delimitador = l->anp ? ';' : ',';
    l->arquivo = fopen(caminho, "rb");
    if (!l->arquivo || ler_registro(l->arquivo, &l->cabecalho) != 1) goto erro;
    const char *inicio = l->cabecalho;
    if (strlen(inicio) >= 3 && !memcmp(inicio, "\xef\xbb\xbf", 3)) inicio += 3;
    if (!separar(inicio, l->delimitador, &l->nomes, &l->quantidade)) goto erro;
    for (size_t i = 0; i < l->quantidade; i++) {
        if (!*l->nomes[i]) goto erro;
        for (size_t j = 0; j < i; j++) if (!strcmp(l->nomes[i], l->nomes[j])) goto erro;
    }
    if (l->anp) {
        const char *obrigatorios[] = {"ANO", "MÊS", "GRANDE REGIÃO",
            "UNIDADE DA FEDERAÇÃO", "PRODUTO", "VENDAS"};
        for (size_t i = 0; i < sizeof obrigatorios / sizeof *obrigatorios; i++)
            if (!leitor_tem_campo(l, obrigatorios[i])) goto erro;
    }
    return l;
erro:
    leitor_fechar(&l); return NULL;
}

int leitor_proximo(LEITOR *l, ITEM **item) {

    char *texto = NULL, **valores = NULL;
    size_t n = 0;
    *item = NULL;
    int status = ler_registro(l->arquivo, &texto);
    if (status != 1) return status;
    if (!separar(texto, l->delimitador, &valores, &n) || n != l->quantidade) {
        free(texto); apagar_campos(valores, n); return -1;
    }
    /* Normaliza apenas a copia dos campos; o registro exportado permanece original. */
    if (l->anp) {
        for (size_t i = 0; i < n; i++) {
            if (!strcmp(l->nomes[i], "VENDAS")) {
                for (char *p = valores[i]; *p; p++)
                    if (*p == ',') *p = '.';
            }
        }
    }
    * item = item_criar((const char *const * )l->nomes, (const char *const * )valores, n, texto);
    if (*item) for (size_t i = 0; i < n; i++) item_definir_tipo(*item, i, leitor_tipo(l, l->nomes[i]));
    free(texto); apagar_campos(valores, n);
    return *item ? 1 : -1;
}

bool leitor_tem_campo(const LEITOR *l, const char *campo) {
    if (!l || !campo) return false;
    for (size_t i = 0; i < l->quantidade; i++) if (!strcmp(l->nomes[i], campo)) return true;
    return false;
}

bool leitor_cabecalho(const LEITOR *l, FILE *saida) {

    if (fputs(l->cabecalho, saida) == EOF) return false;
    size_t n = strlen(l->cabecalho);
    return !n || l->cabecalho[n - 1] == '\n' || fputc('\n', saida) != EOF;
}

void leitor_fechar(LEITOR **l) {

    if (!l || !*l) return;
    if ((*l)->arquivo) fclose((*l)->arquivo);
    apagar_campos((*l)->nomes, (*l)->quantidade);
    free((*l)->cabecalho); free(*l); *l = NULL;
}
