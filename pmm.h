#ifndef PMM_H_INCLUDED
#define PMM_H_INCLUDED

// Constantes para limites máximos
#define MAX_OBJ 500  // Número máximo de objetos
#define MAX_MOC 50   // Número máximo de mochilas


// Estrutura para representar uma solução (vetor de alocações)
typedef struct tSolucao {
    int vet_sol[MAX_OBJ]; // Vetor de alocações: vet_sol[i] = mochila do objeto i
    int fo;               // Valor da função objetivo
} Solucao;

// Estrutura para representar uma solução binária (matriz de alocações)
typedef struct tSolucaoBIN {
    int mat_sol[MAX_MOC][MAX_OBJ]; // Matriz de alocações: mat_sol[i][j] = 1 se o objeto j está na mochila i
    int fo;                        // Valor da função objetivo
} SolucaoBIN;

// Dados de entrada
extern int num_obj;                // Número de objetos
extern int num_moc;                // Número de mochilas
extern int vet_val_obj[MAX_OBJ];   // Valores dos objetos
extern int vet_pes_obj[MAX_OBJ];   // Pesos dos objetos
extern int vet_cap_moc[MAX_MOC];   // Capacidades das mochilas

// Estruturas auxiliares
extern int vet_uso_moc[MAX_MOC];   // Uso atual de cada mochila
extern int vet_qtd_obj[MAX_OBJ];   // Quantidade de vezes que cada objeto foi alocado
extern int vet_ind_ord_obj[MAX_OBJ]; // Índices dos objetos ordenados por valor/peso

// Protótipos das funções

// Buscas locais
void heu_bl_pm(Solucao& s);        // Busca local com perturbação mínima
void heu_bl_mm(Solucao& s);        // Busca local com movimento máximo
void heu_bl_ra(Solucao& s, const int fator); // Busca local com reinício aleatório

// Testes e heurísticas construtivas
void testar_construtivas();        // Testa as heurísticas construtivas
void heu_cons_ale_gul(Solucao& s); // Heurística construtiva aleatória-gulosa
void ordenar_objetos();            // Ordena os objetos por valor/peso
void heu_cons_gulosa(Solucao& s);  // Heurística construtiva gulosa
void testar_estruturas();          // Testa as estruturas de dados

// Funções para cálculo e escrita da solução
void calc_fo_solucao(Solucao& s);  // Calcula a função objetivo de uma solução
void escrever_solucao(Solucao& s); // Escreve a solução na tela
void heu_cons_aleatoria(Solucao& s); // Heurística construtiva aleatória

// Funções para solução binária
void calc_fo_solucaoBIN(SolucaoBIN& s); // Calcula a função objetivo de uma solução binária
void escrever_solucaoBIN(SolucaoBIN& s); // Escreve a solução binária na tela
void criar_solucaoBIN(SolucaoBIN& s);   // Cria uma solução binária aleatória

// Funções para leitura e teste de dados
void testar_dados(char* arq);      // Testa os dados de entrada
void ler_dados(char* arq);         // Lê os dados de entrada de um arquivo
void testar_alocacao();            // Testa a alocação de memória

#endif // PMM_H_INCLUDED