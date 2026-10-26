# Busca e organização de dados — AED1

Programa em C para carregar registros de um CSV, aplicar consultas e exportar os resultados. A estrutura que armazena a base e a estrutura que armazena os resultados são escolhidas separadamente pela linha de comando.

O mesmo programa atende aos dois conjuntos de dados:

- `nasa`: aproximações de asteroides, usando o arquivo oficial da disciplina.
- `anp`: vendas mensais de combustíveis por unidade da federação, usando o CSV da ANP escolhido pelo grupo.

## Arquivos do projeto

O repositório contém apenas os arquivos `.c` e `.h`, este README, `.gitignore`, Makefile e a licença MIT já adotada pelo projeto.

| Módulo | Responsabilidade |
|---|---|
| `main.c` | Argumentos, carregamento, busca, exportação e tempos |
| `item.c` / `item.h` | Campos de um registro, tipos, similaridade e texto original |
| `leitor.c` / `leitor.h` | Leitura dos CSVs NASA e ANP |
| `consulta.c` / `consulta.h` | Filtros, similaridade e configuração da organização |
| `estrutura.c` / `estrutura.h` | Encaminhamento das operações para o TAD escolhido |
| `pilha`, `fila` e `lista_*` | Implementação das oito estruturas |
| `percurso.h` | Tipo da função chamada para visitar cada item |
| `util.c` / `util.h` | Cópia de texto e conversões de valores |

Os headers `.h` são necessários para compilar. Não há dependência de Python ou de scripts externos para usar o programa.

## Compilar

É necessário ter GCC e Make instalados. Execute os comandos na pasta do projeto. Os exemplos usam um terminal compatível com Bash, como Git Bash com as ferramentas instaladas ou WSL.

```sh
make
```

O comando gera `trabalho` com as opções `-std=c11 -Wall -Wextra -pedantic` e `-lm`. Em Windows, o executável pode receber a extensão `.exe`. O compilador e as opções estão escritos diretamente no Makefile; para alterá-los, edite esse arquivo.

Para remover o executável e os arquivos objeto:

```sh
make clean
```

## Preparar uma execução

Os datasets, arquivos de consulta e resultados são arquivos externos ao código. Não estão incluídos no repositório e podem ficar em uma pasta separada.

Nos exemplos, crie uma pasta irmã chamada `arquivos_aed1`:

```sh
mkdir -p ../arquivos_aed1
```

Coloque nela o CSV oficial de asteroides com o nome `asteroids.csv` e o CSV ANP com o nome `vendas-combustiveis.csv`. Renomear o arquivo não altera seu conteúdo. Use sempre a versão NASA fornecida pela disciplina.

O arquivo ANP recebido, originalmente chamado `vendas-combustiveis-m3-1990-2025.csv`, contém 95.040 registros e inclui dados até agosto de 2026, apesar do nome. O leitor usa `;` como delimitador. `ANO` e `VENDAS` são números; `MÊS`, `GRANDE REGIÃO`, `UNIDADE DA FEDERAÇÃO` e `PRODUTO` são textos. Os seis nomes são obrigatórios, com os mesmos acentos. MÊS não é convertido para uma ordem cronológica.

O leitor normaliza a vírgula decimal de VENDAS apenas para os cálculos internos. O texto original é usado na exportação. Valores negativos são preservados. Cada linha é um registro mensal; o programa não soma volumes por ano ou grupo.

## Executar

Exemplo ANP, depois de criar a consulta descrita abaixo:

```sh
./trabalho --reader anp --input ../arquivos_aed1/vendas-combustiveis.csv --base lista_enc --result lista_ord --query ../arquivos_aed1/anp_q1.txt --output ../arquivos_aed1/resultado_anp.csv --top-k 10 --limit 0
```

Exemplo NASA:

```sh
./trabalho --reader nasa --input ../arquivos_aed1/asteroids.csv --base lista_enc --result lista_ord --query ../arquivos_aed1/nasa_q1.txt --output ../arquivos_aed1/resultado_nasa.csv --top-k 100 --limit 0
```

Também é possível compilar e executar pelo Makefile:

```sh
make run ARGS="--reader anp --input ../arquivos_aed1/vendas-combustiveis.csv --base lista_enc --result lista_ord --query ../arquivos_aed1/anp_q1.txt --output ../arquivos_aed1/resultado_anp.csv --top-k 10 --limit 0"
```

| Argumento | Função |
|---|---|
| `--reader` | Leitor: `nasa` ou `anp` |
| `--input` | Caminho do CSV de entrada |
| `--base` | Estrutura que recebe a base durante a leitura |
| `--result` | Estrutura que recebe os registros aceitos |
| `--query` | Caminho do arquivo de consulta |
| `--output` | Caminho do CSV de saída |
| `--top-k` | Máximo de registros exportados; 0 significa todos |
| `--limit` | Máximo de registros carregados; 0 significa toda a base |

Os seis primeiros argumentos são obrigatórios. `--top-k` e `--limit` são opcionais e assumem 0. A pasta de saída deve existir; utilize um arquivo de saída diferente da entrada e da consulta. `./trabalho --help` mostra a forma de uso.

## Estruturas disponíveis

Todas podem ser usadas como base ou resultado:

| Nome | Organização e percurso |
|---|---|
| `pilha` | Insere no topo e percorre do mais recente para o mais antigo |
| `fila` | Insere no fim e percorre em ordem de chegada |
| `lista_seq` | Vetor dinâmico, em ordem de chegada |
| `lista_enc` | Lista encadeada, com inserção no fim |
| `lista_cabeca` | Lista com nó sentinela real, sem registro |
| `lista_ord` | Mantém a ordem durante cada inserção, usando ORDER |
| `lista_gen` | Grupos e subgrupos definidos por GROUP |
| `lista_cruz` | Um mesmo nó participa de dois eixos definidos por CROSS |

GROUP é obrigatório quando a generalizada for escolhida; CROSS é obrigatório para a cruzada. A consulta configura tanto a base quanto o resultado. Os grupos são percorridos por ordem de primeira ocorrência; dentro das folhas ou grupos conserva-se a chegada. A cruzada exporta pelo primeiro campo de CROSS, e seu TAD permite percorrer os dois eixos.

ORDER afeta somente a lista ordenada. Nos empates, ela preserva a chegada. A ordem padrão é similaridade decrescente. Campos ausentes ou inválidos para a ordenação ficam no final.

## Formato da consulta

Salve as consultas como texto UTF-8. Os nomes e valores textuais diferenciam maiúsculas, minúsculas e acentos. Constantes numéricas usam ponto decimal, mesmo nas consultas ANP.

| Comando | Significado |
|---|---|
| `FILTER\|campo\|operador\|valor` | Condição obrigatória; todos os filtros devem passar |
| `SIM\|campo\|alvo\|tolerancia\|peso` | Similaridade numérica, com tolerância e peso positivos |
| `ACCEPT\|limiar` | Similaridade mínima entre 0 e 1; padrão 0 |
| `ORDER\|campo\|ASC` ou `DESC` | Chave e direção da lista ordenada |
| `GROUP\|campo1\|campo2` | Agrupamento hierárquico; aceita mais níveis |
| `CROSS\|campo1\|campo2` | Dois eixos da lista cruzada |

Operadores de FILTER: `==`, `!=`, `<`, `<=`, `>` e `>=`. Booleanos aceitam apenas `==` e `!=`, com `true` ou `false`. Linhas vazias e comentários que começam por `#` são permitidos.

A similaridade individual é `max(0, 1 - abs(valor - alvo) / tolerancia)`. A nota total é a média ponderada dos critérios SIM. Sem SIM, os filtros bastam para aceitar o registro.

### Consulta ANP 1: maiores volumes mensais de gasolina em São Paulo

Crie `../arquivos_aed1/anp_q1.txt` com:

```text
FILTER|PRODUTO|==|GASOLINA C
FILTER|UNIDADE DA FEDERAÇÃO|==|SÃO PAULO
FILTER|ANO|>=|2020
ORDER|VENDAS|DESC
GROUP|GRANDE REGIÃO|UNIDADE DA FEDERAÇÃO
CROSS|UNIDADE DA FEDERAÇÃO|PRODUTO
```

Com `--result lista_ord --top-k 10`, são exportados os dez maiores registros mensais aceitos.

### Consulta ANP 2: diesel próximo de 100 mil m³ em 2025

Crie `../arquivos_aed1/anp_q2.txt` com:

```text
FILTER|PRODUTO|==|ÓLEO DIESEL
FILTER|ANO|==|2025
SIM|VENDAS|100000|50000|1
ACCEPT|0.60
ORDER|similarity|DESC
GROUP|GRANDE REGIÃO|UNIDADE DA FEDERAÇÃO
CROSS|UNIDADE DA FEDERAÇÃO|PRODUTO
```

Para demonstrar a generalizada, execute:

```sh
./trabalho --reader anp --input ../arquivos_aed1/vendas-combustiveis.csv --base lista_enc --result lista_gen --query ../arquivos_aed1/anp_q2.txt --output ../arquivos_aed1/diesel_agrupado.csv --top-k 0 --limit 0
```

Os resultados ficam agrupados por região e depois UF. Nessa estrutura, ORDER não impõe ordenação por similaridade.

### Consulta NASA: aproximações futuras com similaridade

Crie `../arquivos_aed1/nasa_q1.txt` com:

```text
FILTER|is_future_event|==|true
FILTER|distance_au|<=|0.08
SIM|velocity_km_s|15.0|10.0|1.0
SIM|risk_score|0.70|0.30|2.0
ACCEPT|0.60
ORDER|similarity|DESC
GROUP|threat_category|year
CROSS|threat_category|year
```

## Saída e memória

O CSV exportado conserva cabeçalho, colunas, ordem das colunas e delimitador. Não são acrescentadas colunas de similaridade. Se um registro original terminar sem quebra de linha, a exportação acrescenta uma quebra ao final dele.

A saída padrão informa:

- `STATUS`: OK ou ERROR.
- `INPUT_RECORDS`: quantidade carregada.
- `MATCHED_RECORDS`: quantidade aceita pela consulta.
- `OUTPUT_RECORDS`: quantidade exportada.
- `LOAD_MS`, `SEARCH_MS`, `RESULT_INSERT_MS` e `WRITE_MS`: tempos das fases em milissegundos.

Todos os registros aceitos são inseridos no resultado antes de aplicar top-k. A base é percorrida para realizar a busca; top-k limita apenas a escrita. SEARCH_MS desconta o tempo de inserção dos resultados para evitar contagem dupla. A medição usa `timespec_get(TIME_UTC)`, um relógio não monotônico.

Valores ausentes não interrompem a leitura. Um filtro sobre campo vazio falha; um critério SIM sobre valor ausente ou inválido recebe nota zero. Linhas CSV malformadas geram erro, com mensagem em stderr e retorno de falha.

A base é dona dos ITEMs. Os resultados guardam referências a esses itens. Ao encerrar, primeiro são liberados os nós do resultado e depois os registros e a estrutura da base.

