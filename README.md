# Busca e organização de dados — AED1

Programa C11 com oito TADs compartilhados entre dois leitores:

- `nasa`: aproximações de asteroides (CSV com vírgula).
- `anp`: vendas mensais de combustíveis (CSV com ponto e vírgula e decimal brasileiro).

## Compilar

Requisitos: GCC ou compilador C11 compatível, Make; Python 3 para testes e experimentos. Execute os comandos na raiz do projeto. Em Windows, use WSL ou um ambiente com GCC e Make.

```sh
make
mkdir -p resultados
```

`make` gera `trabalho` com `-std=c11 -Wall -Wextra -pedantic -O0 -g` e `-lm`. É possível configurar `CC` e `CFLAGS` na chamada de make. Não há dependências Python externas.

## Dados

A base ANP recebida está incluída, sem alterações, em `dados/vendas-combustiveis.csv`. O nome recebido foi `vendas-combustiveis-m3-1990-2025.csv`, mas o conteúdo abrange 1990 a agosto de 2026: 95.040 registros. Há três valores negativos, mantidos; sua causa não foi confirmada nos metadados. Os campos são:

| Campo | Tipo interno |
|---|---|
| ANO | Número |
| MÊS | Texto |
| GRANDE REGIÃO | Texto |
| UNIDADE DA FEDERAÇÃO | Texto |
| PRODUTO | Texto |
| VENDAS | Número (volume em m³ conforme nome do arquivo) |

O leitor exige esses seis nomes; aceita colunas adicionais, mantidas como texto, e outra ordem das colunas. As consultas usam os nomes exatos com acentos, em UTF-8. MÊS não possui ordenação cronológica: é texto. Constantes numéricas nas consultas usam **ponto**, por exemplo `100000.5`; o CSV mantém a vírgula original.

A base oficial NASA não acompanhou esta atualização. Obtenha a versão fornecida no e-Disciplinas e salve-a como `dados/asteroids.csv`. O relatório original informa 89.227 registros e SHA-256 `be86bc6c2f1fb8670faa691d050c7900f67b2f851bd612f7ed5277cd5c3e54d6`. Esses dados e resultados históricos não foram reconferidos com o CSV real nesta atualização.

## Executar

```sh
./trabalho --reader anp --input dados/vendas-combustiveis.csv --base lista_enc --result lista_ord --query consultas/anp_q1.txt --output resultados/anp_q1.csv --top-k 10 --limit 0
```

```sh
./trabalho --reader nasa --input dados/asteroids.csv --base lista_enc --result lista_ord --query consultas/q1.txt --output resultados/q1.csv --top-k 100 --limit 0
```

Os seis argumentos `--reader`, `--input`, `--base`, `--result`, `--query` e `--output` são obrigatórios. `--top-k` e `--limit` são opcionais e assumem 0 (todos). A pasta de saída deve existir. `./trabalho --help` mostra o formato de uso.

Estruturas válidas para base e resultado: `pilha`, `fila`, `lista_seq`, `lista_enc`, `lista_cabeca`, `lista_ord`, `lista_gen`, `lista_cruz`.

## Consultas

```text
FILTER|PRODUTO|==|ÓLEO DIESEL
FILTER|ANO|==|2025
SIM|VENDAS|100000|50000|1
ACCEPT|0.60
ORDER|similarity|DESC
GROUP|GRANDE REGIÃO|UNIDADE DA FEDERAÇÃO
CROSS|UNIDADE DA FEDERAÇÃO|PRODUTO
```

- `FILTER|campo|operador|valor`: todos os filtros devem ser satisfeitos. Operadores: `==`, `!=`, `<`, `<=`, `>`, `>=`; booleanos aceitam apenas igualdade/desigualdade.
- `SIM|campo|alvo|tolerancia|peso`: campo numérico, tolerância e peso positivos. Nota individual `max(0, 1 - abs(x-alvo)/tolerancia)`; nota final é a média ponderada.
- `ACCEPT|limiar`: nota mínima entre 0 e 1; padrão 0. Sem SIM, os filtros bastam.
- `ORDER|campo|ASC` ou `DESC`: configura **somente a lista ordenada**. Padrão: similaridade decrescente.
- `GROUP|campo1|campo2|...`: níveis da lista generalizada; obrigatório se ela for base ou resultado.
- `CROSS|campo1|campo2`: dois eixos da cruzada; obrigatório se ela for base ou resultado.

Linhas vazias e comentários iniciados por `#` são aceitos. Textos são comparados exatamente, incluindo caixa e acentos. Não há soma, média por grupo ou agregação estatística: cada linha permanece um ITEM.

## Ordem e propriedade da memória

Pilha percorre do topo; fila e listas simples conservam a ordem de chegada. A ordenada mantém a ordem a cada inserção e preserva a chegada nos empates. Ausentes/inválidos numéricos ficam ao final em ambas as direções de ordenação. A generalizada percorre grupos por ordem de primeira ocorrência e subgrupos recursivamente. A cruzada exporta pelo primeiro campo de CROSS; o TAD oferece percurso dos dois eixos.

A consulta configura tanto a base quanto o resultado. A ordem de percurso da base determina a chegada ao resultado. Todos os aceitos são inseridos antes de aplicar top-k; ele limita apenas a exportação.

A base é dona dos ITEMs. O resultado guarda referências; `estrutura_apagar(&resultado, false)` libera seus nós sem liberar os ITEMs. Em seguida, a base é destruída com `true`. A consulta permanece viva até a destruição das estruturas.

Campos vazios não interrompem a leitura: filtros que dependem deles falham, SIM atribui nota zero ao critério ausente/inválido e grupos usam chave vazia. CSV malformado ou com quantidade de campos incompatível gera erro. Negativos são números válidos. O texto original de cada registro é preservado; se a última linha não tiver quebra final, a saída acrescenta uma quebra.

## Testes

```sh
make test
```

Inclui teste dos TADs, 256 execuções sintéticas NASA, 256 ANP, três consultas na ANP real e round-trip byte a byte da base ANP completa. Para a base NASA oficial, depois de disponibilizá-la:

```sh
python3 testes/base_real.py
```

Verificação adicional de memória, em ambiente compatível:

```sh
make clean
make CFLAGS='-std=c11 -Wall -Wextra -pedantic -O0 -g -fsanitize=address,undefined' test
```

No ambiente desta entrega, LeakSanitizer não pôde executar devido à restrição de ptrace; ASan/UBSan foram executados com `ASAN_OPTIONS=detect_leaks=0`. Isso não certifica ausência de vazamentos.

## Exemplos e experimentos

```sh
python3 scripts/exemplos_anp.py
python3 scripts/experimentos.py --reader anp
python3 scripts/relatorio_anp.py
```

O script de experimentos recompila o programa, executa cada configuração três vezes e registra o compilador/flags efetivos via Makefile. Inclui oito pares representativos, tamanhos progressivos e top-k com N fixo. A ordenada como base usa 1.000 e 5.000 registros pelo custo quadrático. Medir as 64 combinações não é necessário: elas são verificadas funcionalmente.

Após obter a NASA, também é possível executar `python3 scripts/experimentos.py --reader nasa`. Os novos resultados vão para `relatorios/nasa/`; os dados históricos da versão anterior permanecem na raiz de `relatorios/` e precisam ser reconciliados no relatório após a nova medição.

## Documentos de entrega

- `relatorios/projeto.md`: organização e três exemplos históricos da NASA; inclui referência à Parte 2.
- `relatorios/parte2.md`: leitor ANP e três exemplos efetivamente executados (exemplos 4–6).
- `relatorios/experimental.md`: análise assintótica compartilhada e experimento histórico NASA.
- `relatorios/experimental_anp.md`: experimento atual ANP, tabelas e discussão.
- `relatorios/anp/`: medições brutas, médias e ambiente da ANP.
- `resultados/anp_q*.csv` e `.txt`: saídas reais dos três exemplos ANP.

Antes de submeter: conferir identificação dos integrantes e cadastro do conjunto ANP na disciplina; obter o CSV NASA oficial e reconferir seus exemplos/experimentos. A página da organização ANP foi informada pelo grupo; acrescentar o endereço específico do recurso, se exigido pela disciplina.
