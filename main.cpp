#include "main.h"
#include <chrono>
#include <cstring>
#include <vector>
#include <algorithm>
#include <random>
#include <cfloat>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <limits>

using namespace std;
using namespace chrono;
// Definição das variáveis globais
int numNos = 100;
int numHubs = 4;  

Node nos[MAX_NOS];
double matrizDistancias[MAX_NOS][MAX_NOS];

// Parâmetros do Algoritmo Genético
const int AGmax = 100;      // número máximo de gerações
const int Pop = 100;         // tamanho da população
const int Cro = 20;         // número de cruzamentos por geração
const double Mut = 0.5;     // taxa de mutação (50%)
const double eliteRate = 0.1;  // percentual de indivíduos da elite para seleção de crossover


string ARQUIVO_ENTRADA = "inst" + to_string(numNos) + ".txt";

//
// Função: lerArquivoEntrada
// Formato do arquivo de entrada:
// Linha 1: <número de nós>
// Linhas seguintes: <coordenada x> <coordenada y>
//
void lerArquivoEntrada(const string& nomeArquivo) {
    ifstream arquivo(nomeArquivo);
    if (!arquivo.is_open()){
        cerr << "Erro ao abrir o arquivo de entrada." << endl;
        exit(1);
    }
    
    // Lê somente o número de nós, pois o arquivo não traz o número de hubs
    arquivo >> numNos;
    if(numNos > MAX_NOS){
        cerr << "Erro: número de nós excede o limite máximo (" << MAX_NOS << ")." << endl;
        exit(1);
    }
    
    for (int i = 0; i < numNos; ++i) {
        arquivo >> nos[i].x >> nos[i].y;
    }
    arquivo.close();
}

struct Candidato
{
    double maxDistMin;  // Menor entre as maiores distâncias
    int index;
};


void selecionarHubs(int hubsSelecionados[]) {
    vector<Candidato> candidatos;

    // Inicializar a semente do gerador de números aleatórios apenas uma vez
    static bool seeded = false;
    if (!seeded) {
        srand((unsigned)time(NULL));
        seeded = true;
    }

    // Calcular a maior distância mínima para cada nó
    for (int i = 0; i < numNos; ++i) {
        double maxDist = 0.0;
        for (int j = 0; j < numNos; ++j) {
            if (i != j) {
                maxDist = max(maxDist, matrizDistancias[i][j]);
            }
        }
        candidatos.push_back({maxDist, i});
    }

    // Ordenar os nós pelos menores valores de "maior distância"
    sort(candidatos.begin(), candidatos.end(), [](const Candidato& a, const Candidato& b) {
        return a.maxDistMin < b.maxDistMin;
    });

    // Selecionar aleatoriamente dentro do top %tal melhores candidatos
    double beta = 0.2;
    int limite = max(1, static_cast<int>(beta * numNos));

    for (int i = 0; i < numHubs; i++) {
        int escolhido = rand() % limite;  // Escolhe um índice dentro do top %tal melhores
        hubsSelecionados[i] = candidatos[escolhido].index;

        // Remover o escolhido da lista para evitar repetições
        candidatos.erase(candidatos.begin() + escolhido);

        // Ajustar o limite para evitar estouro
        if (!candidatos.empty()) {
            limite = max(1, static_cast<int>(beta * candidatos.size()));
        }
    }
}

//
// Função: calcularMatrizDeDistancias
// Calcula a matriz de distâncias entre todos os nós
//
void calcularMatrizDeDistancias() {
    for (int i = 0; i < numNos; ++i) {
        for (int j = i ; j < numNos; ++j) { // Começa de i + 1
            double dx = nos[i].x - nos[j].x;
            double dy = nos[i].y - nos[j].y;
            double d = sqrt(dx * dx + dy * dy);
            matrizDistancias[i][j] = matrizDistancias[j][i] = d;
        }
    }
}


//
// Função: imprimirMatriz
// Imprime a matriz de distâncias
//
void imprimirMatriz() {
    cout << fixed << setprecision(6);
    for (int i = 0; i < numNos; ++i) {
        for (int j = 0; j < numNos; ++j) {
            cout << matrizDistancias[i][j] << " ";
        }
        cout << endl;
    }
}

//
// Função: calculoFO
// Calcula a função objetivo (FO) utilizando a matriz pré-computada e os hubs selecionados
//
double calculoFO(const int hubsSelecionados[]) {
    if (numHubs <= 0 || hubsSelecionados == nullptr) {
        printf("Erro: numHubs <= 0 ou hubsSelecionados == nullptr\n");
        return -1;
    }
    
    const double alpha = 0.75;
    double maxCost = 0.0;
    
    //printf("Iniciando calculoFO\n");

    for (int i = 0; i < numNos; ++i) {
        for (int j = i; j < numNos; ++j) { // j inicia em i+1, logo i==j nunca ocorre
            double minCost = DBL_MAX;
            //printf("\nAnalisando par de nós (%d, %d)\n", i, j);

            for (int k = 0; k < numHubs; ++k) {
                int hubK = hubsSelecionados[k];
                if (hubK < 0 || hubK >= numNos) continue;

                for (int l = 0; l < numHubs; ++l) {
                    int hubL = hubsSelecionados[l];
                    if (hubL < 0 || hubL >= numNos) continue;
                    
                    double alphaCost = alpha * matrizDistancias[hubK][hubL];
                    double cost = matrizDistancias[i][hubK] + alphaCost + matrizDistancias[hubL][j];
                    
                    //printf("  Hubs (%d, %d) -> Custo: %lf\n", hubK, hubL, cost);
                    
                    if (cost < minCost) {
                        minCost = cost;
                        //printf("  Novo minCost para (%d, %d): %lf\n", i, j, minCost);
                    }
                }
            }
            
            if (minCost > maxCost) {
                maxCost = minCost;
                //printf("Atualizando maxCost: %lf\n", maxCost);
            }
        }
    }
    
    //printf("Finalizado calculoFO. maxCost = %lf\n", maxCost);
    return maxCost;
}

//
// Função: criarArquivoDeSaida
// Cria um arquivo com os nós (coordenadas)
//
void criarArquivoDeSaida(const string& nomeArquivo) {
    ofstream arquivo(nomeArquivo);
    if (!arquivo.is_open()){
        cerr << "Erro ao abrir o arquivo de saída." << endl;
        exit(1);
    }
    
    arquivo << numNos << "\n";
    arquivo << fixed << setprecision(6);
    for (int i = 0; i < numNos; ++i) {
        arquivo << nos[i].x << " " << nos[i].y << "\n";
    }
    arquivo.close();
}

//
// Função: salvarResultados
// Salva os resultados (nós, hubs, FO e tabela de custos) em um arquivo
//
void salvarResultados(const string &nomeArquivo, const int hubsSelecionados[], double FO) {
    criarArquivoDeSaida(nomeArquivo);

    ofstream arquivo(nomeArquivo);
    if (!arquivo.is_open()){
        cerr << "Erro ao abrir o arquivo para escrita: " << nomeArquivo << endl;
        exit(1);
    }
    
    arquivo << "n: " << numNos << " p: " << numHubs << "\n";
    arquivo << fixed << setprecision(2) << "FO: " << FO << "\n";
    arquivo << "HUBS: [";
    for (int i = 0; i < numHubs; ++i) {
        arquivo << hubsSelecionados[i];
        if(i < numHubs - 1)
            arquivo << ", ";
    }
    arquivo << "]\n\n";
    
    arquivo << "OR\tH1\tH2\tDS\tCUSTO\n";
    const double alpha = 0.75;
    for (int i = 0; i < numNos; ++i) {
        for (int j = 0; j < numNos; ++j) {
            if (i == j) continue;
            double minCost = numeric_limits<double>::max();
            int bestHub1 = -1, bestHub2 = -1;
            for (int k = 0; k < numHubs; ++k) {
                for (int l = 0; l < numHubs; ++l) {
                    double custo = matrizDistancias[i][hubsSelecionados[k]] +
                                   alpha * matrizDistancias[hubsSelecionados[k]][hubsSelecionados[l]] +
                                   matrizDistancias[hubsSelecionados[l]][j];
                    if(custo < minCost){
                        minCost = custo;
                        bestHub1 = hubsSelecionados[k];
                        bestHub2 = hubsSelecionados[l];
                    }
                }
            }
            arquivo << i << "\t" << bestHub1 << "\t" << bestHub2 << "\t" << j << "\t" 
                    << fixed << setprecision(2) << minCost << "\n";
        }
    }
    arquivo.close();
}

//
// Função: lerResultados
// Lê os dados de um arquivo de resultados e os armazena na estrutura Dados
//
Dados lerResultados(const string& nomeArquivo) {
    Dados dados;
    ifstream arquivo(nomeArquivo);
    if (!arquivo) {
        cerr << "Erro ao abrir o arquivo: " << nomeArquivo << endl;
        return dados;
    }
    
    string linha;
    getline(arquivo, linha);
    sscanf(linha.c_str(), "n: %d p: %d", &dados.n, &dados.p);
    
    getline(arquivo, linha);
    sscanf(linha.c_str(), "FO: %lf", &dados.FO);
    
    getline(arquivo, linha);
    size_t pos1 = linha.find("[");
    size_t pos2 = linha.find("]");
    string hubsStr = linha.substr(pos1+1, pos2-pos1-1);
    istringstream iss(hubsStr);
    for (int i = 0; i < dados.p; i++) {
        int hub;
        iss >> hub;
        dados.hubs[i] = hub;
        if(iss.peek() == ',')
            iss.ignore();
    }
    
    // Pula linha em branco e cabeçalho da tabela
    getline(arquivo, linha);
    getline(arquivo, linha);
    
    int entradaIndex = 0;
    while(getline(arquivo, linha)) {
        if(linha.empty()) continue;
        Entrada entrada;
        istringstream issLinha(linha);
        issLinha >> entrada.OR >> entrada.H1 >> entrada.H2 >> entrada.DS >> entrada.custo;
        dados.entradas[entradaIndex++] = entrada;
    }
    dados.numEntradas = entradaIndex;
    arquivo.close();
    return dados;
}

//Estrutura que representa um indivíduo (solução)
struct Individuo {
    int hubs[MAX_HUBS];
    double fitness;

    // Sobrecarga do operador < para comparação com base no fitness
    bool operator<(const Individuo &outro) const {
        return fitness < outro.fitness;
    }
};

// Função para inicializar um indivíduo usando a função aleatória gulosa
void inicializarIndividuo(Individuo &ind) {
    // Inicializa o vetor com zeros
    memset(ind.hubs, 0, sizeof(ind.hubs));

    // Usa a função existente para selecionar os hubs
    selecionarHubs(ind.hubs);

    // Avalia a solução
    ind.fitness = calculoFO(ind.hubs);
}

// Função de comparação para ordenação (menor FO = melhor)
bool comparaIndividuo(const Individuo &a, const Individuo &b){
    return a.fitness < b.fitness;
}

// Função para realizar o crossover multiponto entre dois pais
// Aqui, já que o cromossomo é um vetor de tamanho numHubs, podemos fazer, um crossover de dois pontos
Individuo crossoverMultiponto(const Individuo &pai1, const Individuo &pai2) {
    Individuo filho = pai1;  // Inicia copiando o pai1

    // Para problemas com poucos genes, pode ser que o crossover multiponto seja simples:
    // escolha dois pontos (índices) e troque os genes entre esses pontos
    int ponto1 = 0;
    int ponto2 = 0;

    if (numHubs > 1)
    {
        ponto1 = rand() % numHubs;
        ponto2 = rand() % numHubs;
        if(ponto1 > ponto2) swap(ponto1, ponto2);
    }

    for(int i = ponto1; i <= ponto2; i++) {
        filho.hubs[i] = pai2.hubs[i];
    }
    return filho;
}

// Função para realizar a mutação em um indivíduo
void mutacao(Individuo &ind) {
    // para cada gene(hub), com probabilidade Mut, gera um novo valor aleatóro (garantindo que esteja dentro de 0 a numNos-1)
    for(int i = 0; i < numHubs; i++) {
        double r = ((double) rand()/ RAND_MAX);

        if(r < Mut) {
            // Escolhe um novo huba de forma aleatória dentre os nós (podeira também usar uma abordagem gulosa ou outra heurística)
            int novoHub = rand() % numNos;
            ind.hubs[i] = novoHub;
        }
    }

    // Recalcula o fitness após a mutação
    ind.fitness = calculoFO(ind.hubs);
}

// Função principal do algoritmo genético
Individuo algoritmoGenetico() {
    vector<Individuo> populacao(Pop);
    // Inicializa a população
    for(int i = 0; i < Pop; i++){
        inicializarIndividuo(populacao[i]);
    }

    Individuo melhorSolucao = populacao[0];

    // Loop das gerações
    for(int geracao = 0; geracao < AGmax; geracao++){
        //Ordena a população (melhor FO = menor)
        sort(populacao.begin(), populacao.end(), comparaIndividuo);

        // Atualiza a melhor solução
        if (populacao[0].fitness < melhorSolucao.fitness){
            melhorSolucao = populacao[0];
        }

        vector<Individuo> novosIndividuos;

        // Elite: preserva os melhores indivíduos (20% melhores, atualmente)
        int numElite = max(1, static_cast<int>(eliteRate * Pop));
        for(int i = 0; i < numElite; i++){
            novosIndividuos.push_back(populacao[i]);
        }

        //Gerar novos indivíduos via crossover
        for(int i = 0; i < Cro; i++){
            // Seleciona um pai da elite
            int indicePai1 = rand() % numElite;

            // Seleciona o outro pai de forma aleatória dentre toda a população
            int indicePai2 = rand() % Pop;

            Individuo filho = crossoverMultiponto(populacao[indicePai1], populacao[indicePai2]);

            // Aplica mutação no filho
            mutacao(filho);

            // Adiciona o filho na nova população
            novosIndividuos.push_back(filho);
        }

        // Preenche o restante da nova população com indivíduos escolhidos (poderia se cópia ou seleção aleatória dos que sobraram)
        while(novosIndividuos.size() < Pop) {
            int idx = rand() % Pop;
            novosIndividuos.push_back(populacao[idx]);
        }

        // Atualiza a população: seleciona os Pop melhore dentre os atuais e os novos (pode-se combinar os 2 vetores)
        vector<Individuo> combinados = populacao;
        combinados.insert(combinados.end(), novosIndividuos.begin(), novosIndividuos.end());
        //sort(combinados.begin(), combinados.begin() + Pop);
        sort(combinados.begin(), combinados.end(), comparaIndividuo);
        populacao.assign(combinados.begin(), combinados.begin() + Pop);


        // (Opcional) Exibir informações da geração
        cout << "Generation " << geracao + 1 << " - Best FO: " << populacao[0].fitness << endl;
    }

    return melhorSolucao;
}

//
// Função: main
//
int main() {
    string nomeArquivoEntrada = ARQUIVO_ENTRADA; // O arquivo de entrada deve conter: número de nós e as coordenadas
    
    // Ler os dados do arquivo de entrada
    lerArquivoEntrada(nomeArquivoEntrada);
    
    // Calcular a matriz de distâncias
    calcularMatrizDeDistancias();
    
    // === Heurística Construtiva (Gulosa) ===
    int hubsConstructive[MAX_HUBS];
    memset(hubsConstructive, 0, sizeof(hubsConstructive));
    
    // Seleciona os hubs utilizando a heurística construtiva
    selecionarHubs(hubsConstructive);
    double FOConstructive = calculoFO(hubsConstructive);
    
    // Salvar os resultados da construtiva em "resultados.txt"
    salvarResultados("resultados.txt", hubsConstructive, FOConstructive);
    
    // === Algoritmo Genético (AG) ===
    auto start = high_resolution_clock::now();
    
    // Executar o algoritmo genético para encontrar a melhor solução
    Individuo melhor = algoritmoGenetico();
    
    auto stop = high_resolution_clock::now();
    auto duration_us = duration_cast<microseconds>(stop - start);
    
    cout << "Best solution found (AG): FO = " << melhor.fitness << "\nHubs: ";
    for (int i = 0; i < numHubs; i++) {
        cout << melhor.hubs[i] << " ";
    }
    cout << "\nExecution Time (AG): " << duration_us.count() << " microseconds" << endl;
    
    // Salvar os resultados do AG em "resultados_AG.txt"
    salvarResultados("resultados_AG.txt", melhor.hubs, melhor.fitness);
    
    return 0;
}


