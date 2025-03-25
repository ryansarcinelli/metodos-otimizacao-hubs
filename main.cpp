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

int numNos = 0;

// Parâmetros do problema
const int numHubs = 2;  
const int TEMPO_MAXIMO = 60;
string ARQUIVO_ENTRADA = "inst10.txt";


Node nos[MAX_NOS];
double matrizDistancias[MAX_NOS][MAX_NOS];

// Parâmetros do Algoritmo Genético
const int AGmax = 100;      // número máximo de gerações
const int Pop = 100;         // tamanho da população
const int Cro = 20;         // número de cruzamentos por geração
const double Mut = 0.5;     // taxa de mutação (50%)
const double eliteRate = 0.1;  // percentual de indivíduos da elite para seleção de crossover


struct Solucao {
    double fo;
    int hubs[numHubs];
    double tempoMelhorSolucao;  
};

struct Candidato
{
    double maxDistMin;  // Menor entre as maiores distâncias
    int index;
};

struct Individuo {
    int hubs[MAX_HUBS];
    double fitness;

    // Sobrecarga do operador < para comparação com base no fitness
    bool operator<(const Individuo &outro) const {
        return fitness < outro.fitness;
    }
};

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

void selecionarHubs(int hubsSelecionados[]) {
    vector<Candidato> candidatos;

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

void imprimirMatriz() {
    cout << fixed << setprecision(6);
    for (int i = 0; i < numNos; ++i) {
        for (int j = 0; j < numNos; ++j) {
            cout << matrizDistancias[i][j] << " ";
        }
        cout << endl;
    }
}

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

Solucao clonarSolucao(Individuo &ind) {
    Solucao sol;

    for (int i = 0; i < numHubs; i++) {
        sol.hubs[i] = ind.hubs[i];  // Copia os hubs do indivíduo para a solução
    }
    
    sol.fo = ind.fitness;  // Copia a função objetivo

    return sol;
}

void SalvarSolucao(const std::vector<Solucao>& solucoes, const std::string& nomeArquivoEntrada, int numHubs) {
    std::string nomeArquivo = "solucoes.txt"; // Nome fixo para armazenar todas as soluções
    std::ofstream arquivo(nomeArquivo, std::ios::trunc); // Abre o arquivo em modo de sobrescrita

    if (!arquivo.is_open()) {
        std::cerr << "Erro ao abrir o arquivo para salvar soluções." << std::endl;
        return;
    }

    for (size_t i = 0; i < solucoes.size(); ++i) {
        std::string nomeSolucao = "solucao_" + std::to_string(TEMPO_MAXIMO) + "_" + std::to_string(numHubs) + "hubs" + nomeArquivoEntrada;
        arquivo << "Solução: " << nomeSolucao << std::endl;
        arquivo << "Hubs: ";
        
        for (int hub : solucoes[i].hubs) {
            arquivo << hub << " ";
        }
        
        arquivo << "\nFO: " << solucoes[i].fo << "\n\n";
    }

    arquivo.close();
    std::cout << "Soluções salvas em " << nomeArquivo << " (sobrescrito)" << std::endl;
}

void CarregarSolucoes(std::vector<Solucao>& solucoes) {
    std::string nomeArquivo = "solucoes.txt"; // Nome fixo do arquivo

    std::ifstream arquivo(nomeArquivo);
    if (!arquivo.is_open()) {
        std::cerr << "Erro ao abrir o arquivo: " << nomeArquivo << std::endl;
        return;
    }

    std::string linha;
    Solucao sol;
    int hubIndex = 0;

    while (std::getline(arquivo, linha)) {
        if (linha.rfind("Solução:", 0) == 0) {
            // Nova solução detectada, reinicializar os índices
            hubIndex = 0;
        } 
        else if (linha.rfind("Hubs:", 0) == 0) {
            // Lendo os hubs e armazenando no array fixo
            std::istringstream iss(linha.substr(5)); // Ignora "Hubs:"
            int hub;
            while (iss >> hub && hubIndex < numHubs) {
                sol.hubs[hubIndex++] = hub;
            }
        } 
        else if (linha.rfind("FO:", 0) == 0) {
            // Lendo a função objetivo
            sol.fo = std::stod(linha.substr(3)); // Ignora "FO:"
            solucoes.push_back(sol); // Adiciona a solução ao vetor
        }
    }

    arquivo.close();
    std::cout << "Soluções carregadas de: " << nomeArquivo << std::endl;
}

Solucao lerArquivoSaida(const std::string& nomeArquivo) {
    std::ifstream arquivo(nomeArquivo);
    Solucao solucao = {0, {0}}; // Initialize all fields
    std::string linha;

    if (!arquivo.is_open()) {
        std::cerr << "Erro ao abrir o arquivo " << nomeArquivo << std::endl;
        return solucao;
    }

    // Ignore first line
    if (!std::getline(arquivo, linha)) {
        std::cerr << "Arquivo vazio ou erro de leitura" << std::endl;
        return solucao;
    }

    // Read FO from second line
    if (!std::getline(arquivo, linha) || linha.substr(0, 4) != "FO: ") {
        std::cerr << "Formato inválido para linha FO" << std::endl;
        return solucao;
    }
    std::istringstream(linha.substr(4)) >> solucao.fo;

    // Read hubs from third line
    if (!std::getline(arquivo, linha)) {
        std::cerr << "Não foi possível ler linha de hubs" << std::endl;
        return solucao;
    }

    size_t inicio = linha.find('[');
    size_t fim = linha.find(']');

    if (inicio == std::string::npos || fim == std::string::npos || fim <= inicio) {
        std::cerr << "Formato inválido para lista de hubs" << std::endl;
        return solucao;
    }

    std::istringstream stream(linha.substr(inicio + 1, fim - inicio - 1));
    std::string hub;
    int i = 0;
    
    while (i < numHubs && std::getline(stream, hub, ',')) {
        try {
            solucao.hubs[i++] = std::stoi(hub);
        } catch (const std::exception& e) {
            std::cerr << "Erro ao converter hub: " << e.what() << std::endl;
            solucao.hubs[i++] = -1; // or some error value
        }
    }

    // Check if we read exactly numHubs
    if (i != numHubs) {
        std::cerr << "Aviso: número de hubs lidos (" << i << ") diferente de numHubs (" << numHubs << ")" << std::endl;
    }

    return solucao;
}

void salvarResultados(const string &nomeArquivo, const int hubsSelecionados[], double FO) {


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

void inicializarIndividuo(Individuo &ind) {
    // Inicializa o vetor com zeros
    memset(ind.hubs, 0, sizeof(ind.hubs));

    // Usa a função existente para selecionar os hubs
    selecionarHubs(ind.hubs);

    // Avalia a solução
    ind.fitness = calculoFO(ind.hubs);
}

bool comparaIndividuo(const Individuo &a, const Individuo &b){
    return a.fitness < b.fitness;
}

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

// Individuo algoritmoGenetico() {
//     const int TEMPO_MAXIMO_MS = TEMPO_MAXIMO * 1000;
//     vector<Individuo> populacao(Pop);
//     auto inicio = high_resolution_clock::now();
//     double tempoMelhorFO = 0.0;
    
//     // Initialize population
//     for(int i = 0; i < Pop; i++) {
//         inicializarIndividuo(populacao[i]);
//     }

//     Individuo melhorSolucao = populacao[0];
//     int geracao = 0;

//     while(true) {
//         auto agora = high_resolution_clock::now();
//         auto decorrido = duration_cast<milliseconds>(agora - inicio).count();
        
//         if(decorrido >= TEMPO_MAXIMO_MS) {
//             cout << "\nTempo limite de " << TEMPO_MAXIMO << " segundos atingido." << endl;
//             break;
//         }

//         sort(populacao.begin(), populacao.end(), comparaIndividuo);

//         // Update best solution and record time
//         if(populacao[0].fitness < melhorSolucao.fitness) {
//             melhorSolucao = populacao[0];
//             tempoMelhorFO = duration_cast<duration<double>>(agora - inicio).count();
//         }

//         vector<Individuo> novosIndividuos;
//         int numElite = max(1, static_cast<int>(eliteRate * Pop));
        
//         // Elitism
//         for(int i = 0; i < numElite; i++) {
//             novosIndividuos.push_back(populacao[i]);
//         }

//         // Crossover
//         for(int i = 0; i < Cro; i++) {
//             int indicePai1 = rand() % numElite;
//             int indicePai2 = rand() % Pop;
//             Individuo filho = crossoverMultiponto(populacao[indicePai1], populacao[indicePai2]);
//             mutacao(filho);
//             novosIndividuos.push_back(filho);
//         }

//         // Fill remaining population
//         while(novosIndividuos.size() < Pop) {
//             int idx = rand() % Pop;
//             novosIndividuos.push_back(populacao[idx]);
//         }

//         // Update population
//         vector<Individuo> combinados = populacao;
//         combinados.insert(combinados.end(), novosIndividuos.begin(), novosIndividuos.end());
//         sort(combinados.begin(), combinados.end(), comparaIndividuo);
//         populacao.assign(combinados.begin(), combinados.begin() + Pop);

//         geracao++;
//     }

//     auto fim = high_resolution_clock::now();
//     auto tempo_total = duration_cast<milliseconds>(fim - inicio);

//     cout << "\n=== Resultado Final ===" << endl;
    
//     cout << "Total generations: " << geracao << endl;
//     cout << "Best FO: " << fixed << setprecision(2) << melhorSolucao.fitness << endl;
//     cout << "Found at: " << fixed << setprecision(2) << tempoMelhorFO << " segundos" << endl;
//     cout << "Hubs: ";
//     for(int i = 0; i < numHubs; i++) {
//         cout << melhorSolucao.hubs[i] << " ";
//     }
//     cout << "\nTempo total de execução: " << tempo_total.count()/1000.0 << " segundos" << endl;

//     return melhorSolucao;
// }

// ALGORITMO GENÉTICO SEM LIMITE DE TEMPO
Individuo algoritmoGenetico() {
    vector<Individuo> populacao(Pop);
    double tempoMelhorFO = 0.0;
    auto inicio = high_resolution_clock::now();
    
    // Inicializa a população
    for (int i = 0; i < Pop; i++) {
        inicializarIndividuo(populacao[i]);
    }

    Individuo melhorSolucao = populacao[0];
    int geracao = 0;

    // Loop baseado no número máximo de gerações
    while(geracao < AGmax) {
        sort(populacao.begin(), populacao.end(), comparaIndividuo);

        // Atualiza a melhor solução e registra o tempo dela
        if (populacao[0].fitness < melhorSolucao.fitness) {
            melhorSolucao = populacao[0];
            auto agora = high_resolution_clock::now();
            tempoMelhorFO = duration_cast<duration<double>>(agora - inicio).count();
        }

        vector<Individuo> novosIndividuos;
        int numElite = max(1, static_cast<int>(eliteRate * Pop));
        
        // Elitismo
        for (int i = 0; i < numElite; i++) {
            novosIndividuos.push_back(populacao[i]);
        }

        // Crossover
        for (int i = 0; i < Cro; i++) {
            int indicePai1 = rand() % numElite;
            int indicePai2 = rand() % Pop;
            Individuo filho = crossoverMultiponto(populacao[indicePai1], populacao[indicePai2]);
            mutacao(filho);
            novosIndividuos.push_back(filho);
        }

        // Completa a população restante
        while (novosIndividuos.size() < Pop) {
            int idx = rand() % Pop;
            novosIndividuos.push_back(populacao[idx]);
        }

        // Atualiza a população
        vector<Individuo> combinados = populacao;
        combinados.insert(combinados.end(), novosIndividuos.begin(), novosIndividuos.end());
        sort(combinados.begin(), combinados.end(), comparaIndividuo);
        populacao.assign(combinados.begin(), combinados.begin() + Pop);

        geracao++;
    }

    auto fim = high_resolution_clock::now();
    auto tempo_total = duration_cast<milliseconds>(fim - inicio);

    cout << "\n=== Final Result ===" << endl;
    cout << "Generations (Total): " << geracao << endl;
    cout << "Best FO: " << fixed << setprecision(2) << melhorSolucao.fitness << endl;
    cout << "Find at: " << fixed << setprecision(2) << tempoMelhorFO << " segundos" << endl;
    cout << "Hubs: ";
    for (int i = 0; i < numHubs; i++) {
        cout << melhorSolucao.hubs[i] << " ";
    }
    cout << "\nExecution Time: " << tempo_total.count()/1000.0 << " seconds" << endl;

    return melhorSolucao;
}


int main() {
    unsigned int seed = time(0);
    srand(seed); // Configura o gerador de números aleatórios
    string nomeArquivoEntrada = ARQUIVO_ENTRADA;
    
    lerArquivoEntrada(nomeArquivoEntrada);
    calcularMatrizDeDistancias();
    vector<Solucao> solucoes;
    cout << "solucao_" << TEMPO_MAXIMO << "s_" << numHubs << "hubs_" << nomeArquivoEntrada << endl;
    auto start = high_resolution_clock::now();

    // Executar o algoritmo genético
    Individuo melhor = algoritmoGenetico();
    
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<seconds>(stop - start);
    
    
    salvarResultados("resultados_AG.txt", melhor.hubs, melhor.fitness);
    
    return 0;
}


/*
int main() {
    string nomeArquivoEntrada = ARQUIVO_ENTRADA; // O arquivo de entrada deve conter: número de nós e as coordenadas
    
    // Ler os dados do arquivo de entrada
    lerArquivoEntrada(nomeArquivoEntrada);
    
    // Calcular a matriz de distâncias
    calcularMatrizDeDistancias();
    vector<Solucao> solucoes;


    // === Algoritmo Genético (AG) ===
    auto start = high_resolution_clock::now();
    Solucao antigo = lerArquivoSaida("resultados_AG.txt");
    solucoes.push_back(antigo);
    // Executar o algoritmo genético para encontrar a melhor solução
    Individuo melhor = algoritmoGenetico();
    //CarregarSolucoes(solucoes);
    //Solucao melhorsolucao=clonarSolucao(melhor);
    //solucoes.push_back(melhorsolucao);
    SalvarSolucao(solucoes, nomeArquivoEntrada, numHubs);

    //aqui printa o vector Solucoes
    for (size_t i = 0; i < solucoes.size(); ++i) {
        cout << "Solução " << i << ": FO = " << solucoes[i].fo << "\nHubs: ";
        for (int j = 0; j < numHubs; j++) {
            cout << solucoes[i].hubs[j] << " ";
        }
        cout << endl;
    }
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

*/