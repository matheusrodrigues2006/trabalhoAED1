#include "estrutura.h"
#include "leitor.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <stdint.h>


// 
typedef struct {
    const char *reader, *input, *base, *result, *query, *output;
    size_t top_k, limit;

} ARGUMENTOS;


typedef struct {
    const CONSULTA *consulta;
    ESTRUTURA *resultado;
    size_t encontrados;
    double insercao_ms;
    bool erro;
} BUSCA;



typedef struct { FILE *arquivo; size_t limite, escritos; bool erro; } SAIDA;


static double agora_ms(void) {
    
    struct timespec t;
    timespec_get(&t, TIME_UTC);
    return (double)t.tv_sec * 1000 + (double)t.tv_nsec / 1000000;
}

static bool ler_tamanho(const char *texto, size_t *valor) {
    if (!texto || !*texto) return false;

    for (const char *p = texto; *p; p++) if (*p < '0' || *p > '9') return false;

    char *fim; errno = 0;

    unsigned long long n = strtoull(texto, &fim, 10);

    if (*fim || errno || n > SIZE_MAX) return false;

    *valor = (size_t)n; return true;
}


static bool argumentos(int argc, char * *argv, ARGUMENTOS *a) {
    const char *nomes[] = {"--reader", "--input", "--base", "--result", "--query", "--output", "--top-k", "--limit"};

    bool usados[8] = {false};

    for (int i = 1 ; i < argc ; i += 2) {
        if (i + 1 >= argc ) return false;
        size_t j = 0;

        while (j < 8 && strcmp(argv[i], nomes[j])) j++;

        if (j == 8 || usados[j]) return false;

        usados[j] = true;

        const char *v = argv[i + 1];

        switch (j) {
            case 0: a->reader = v; break;
            case 1: a->input = v; break;
            case 2: a->base = v; break;
            case 3: a->result = v; break;
            case 4: a->query = v; break;
            case 5: a->output = v; break;
            case 6: if (!ler_tamanho(v, &a->top_k)) return false; break;
            case 7: if (!ler_tamanho(v, &a->limit)) return false; break;
        }
    }
    for (size_t i = 0; i < 6; i++) if (!usados[i]) return false;

    return strcmp(a->input, a->output) != 0 && strcmp(a->query, a->output) != 0;
}


static bool buscar(ITEM *item, void *contexto) {
    BUSCA *b = contexto;
    double similaridade;


    if (consulta_avaliar(b->consulta, item, &similaridade)) {
        item_definir_similaridade(item, similaridade);

        double inicio = agora_ms();

        bool ok = estrutura_inserir(b->resultado, item);
        b->insercao_ms += agora_ms() - inicio;
        if (!ok) { b->erro = true; return false; }
        b->encontrados++;
    }
    return true;
}


static bool escrever(ITEM *item, void *contexto) {
    SAIDA *s = contexto;

    if (s->limite && s->escritos >= s->limite) return false;

    const char *texto = item_obter_registro_original(item);

    size_t n = strlen(texto);

    if ( fputs(texto, s->arquivo) == EOF ||
        (n && texto[n - 1] != '\n' && fputc('\n', s->arquivo) == EOF)) {
        s->erro = true; return false;
    }

    s->escritos++;
    return !s->limite || s->escritos < s->limite;
}


int main(int argc, char * *argv) {

    ARGUMENTOS a = {0};

    CONSULTA *consulta = NULL;
    LEITOR *leitor = NULL;
    ESTRUTURA *base = NULL, *resultado = NULL;
    FILE *saida = NULL;
    char erro[256] = "Argumentos invalidos";
    int retorno = EXIT_FAILURE;
    if (argc == 2 && !strcmp(argv[1], "--help")) {
        puts("./trabalho --reader nasa --input dados.csv --base lista_enc --result lista_ord --query consulta.txt --output resultado.csv [--top-k 0] [--limit 0]");
        return EXIT_SUCCESS;
    }
    if (!argumentos(argc, argv, &a)) goto fim;

    consulta = consulta_carregar(a.query, erro, sizeof erro);

    if (!consulta) goto fim;

    double inicio = agora_ms();

    leitor = leitor_abrir(a.input, a.reader);

    if (!leitor) { strcpy(erro, "Falha no leitor, arquivo ou cabecalho CSV"); goto fim; }

    if (!consulta_validar(consulta, leitor)) { strcpy(erro, "Campo ou valor invalido na consulta"); goto fim; }

    base = estrutura_criar(a.base, consulta);

    resultado = estrutura_criar(a.result, consulta);

    if (!base || !resultado) { strcpy( erro, "Estrutura invalida, falta de memoria ou GROUP/CROSS ausente"); goto fim; }

    size_t carregados = 0;

    while (!a.limit || carregados < a.limit) {
        ITEM *item = NULL;

        int status = leitor_proximo(leitor, &item);
        if (!status) break;

        if (status < 0 ) { strcpy(erro, "Falha ao ler registro CSV"); goto fim; }
        const char *ordem = consulta_campo_ordem(consulta);
        if (!strcmp(a.base, "lista_ord") && (!ordem || !strcmp(ordem, "similarity"))) {
            double nota;
            consulta_avaliar(consulta, item, &nota);
            item_definir_similaridade(item, nota);
        }

        if (!estrutura_inserir(base, item)) {
            item_apagar(&item); strcpy(erro, "Sem memoria para carregar a base"); goto fim;
        }
        carregados++;
    }

    double carga = agora_ms() - inicio;

    BUSCA busca = {consulta, resultado, 0, 0, false};
    inicio = agora_ms();
    estrutura_percorrer(base, buscar, &busca);
    double tempo_busca = agora_ms() - inicio - busca.insercao_ms;

    if (tempo_busca < 0) tempo_busca = 0;

    if (busca.erro) { strcpy(erro, "Sem memoria para resultados"); goto fim; }
    inicio = agora_ms();
    saida = fopen(a.output, "wb");

    if (!saida || !leitor_cabecalho(leitor, saida)) { strcpy(erro, "Falha ao abrir ou escrever saida"); goto fim; }

    SAIDA escrita = {saida, a.top_k, 0, false};

    estrutura_percorrer(resultado, escrever, &escrita);
    int fechou = fclose(saida); saida = NULL;
    if (escrita.erro || fechou) { strcpy(erro, "Falha ao salvar saida"); goto fim; }
    double tempo_saida = agora_ms() - inicio;

printf("STATUS=OK\n"
       "INPUT_RECORDS=%zu\n"
       "MATCHED_RECORDS=%zu\n"
       "OUTPUT_RECORDS=%zu\n",
       carregados, busca.encontrados, escrita.escritos);

printf("LOAD_MS=%.3f\n"
       "SEARCH_MS=%.3f\n"
       "RESULT_INSERT_MS=%.3f\n"
       "WRITE_MS=%.3f\n",
       carga, tempo_busca, busca.insercao_ms, tempo_saida);    retorno = EXIT_SUCCESS;
fim:

    if (retorno != EXIT_SUCCESS) { puts("STATUS=ERROR"); fprintf(stderr, "%s\n", erro); }
    if (saida) fclose(saida);
 // libera a estrutura de resultados, mas nao os itens, pois eles estao na base
    estrutura_apagar(&resultado, false);
    estrutura_apagar(&base, true);
    leitor_fechar(&leitor);
    consulta_apagar(&consulta);
    return retorno;
}
