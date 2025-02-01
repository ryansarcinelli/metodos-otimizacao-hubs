#include <iostream>
#include <fstream>
#include <array>
#include <string>
#include <iomanip>
#include <cmath>
#include <limits>
#include <cstdlib>
#include <ctime>
#include <chrono>
<<<<<<< Updated upstream

=======
#include <algorithm>
#include <cstring>
>>>>>>> Stashed changes

using namespace std;
using namespace chrono;

const int MAX_NOS = 20;
const int NUM_HUBS = 4;

struct Node {
    double x, y;
    long double dist;
};

struct Solucao {
    double FO;
    int hubs[NUM_HUBS];
};

Solucao clonarSolucao(const Solucao& solucaoOriginal) {
    Solucao novaSolucao;
    novaSolucao.FO = solucaoOriginal.FO;
    memcpy(novaSolucao.hubs, solucaoOriginal.hubs, sizeof(solucaoOriginal.hubs));
    return novaSolucao;
}


double matrizDistancias[MAX_NOS][MAX_NOS];
Node nos[MAX_NOS];

void lerArquivoEntrada(const string& nomeArquivo, Node nos[MAX_NOS]) {
    ifstream arquivo(nomeArquivo);
    if (!arquivo.is_open()) exit(1);

    int NUM_NOS;
    arquivo >> NUM_NOS;

    arquivo >> std::fixed >> std::setprecision(6);

    for (int i = 0; i < NUM_NOS; ++i) {
        arquivo >> nos[i].x >> nos[i].y;
    }

    arquivo.close();
}


void criarArquivoDeSaida(Node nos[MAX_NOS], int NUM_NOS) {
    ofstream arquivo("saida.txt");
    if (!arquivo.is_open()) exit(1);

    arquivo << NUM_NOS << endl;

    arquivo << std::fixed << std::setprecision(6);
    for (int i = 0; i < NUM_NOS; ++i) {
        arquivo << nos[i].x << " " << nos[i].y << endl;
    }

    arquivo.close();
}

<<<<<<< Updated upstream
// Função para selecionar 4 hubs aleatórios
vector<int> selecionarHubs(int numeroDeNos, int numeroDeHubs) {
    vector<int> hubs;

    for (int i = 0; i < numeroDeHubs; ++i) {
        hubs.push_back(i); // Seleciona os primeiros 'numeroDeHubs' nós
    }

    return hubs;
}

// Função para exibir os hubs
void exibirHubs(const vector<int>& hubs) {
    cout << "Hubs selecionados: ";
    for (size_t i = 0; i < hubs.size(); ++i) {
        cout << hubs[i] << (i < hubs.size() - 1 ? ", " : "\n");
    }
=======
void heuristicaConstrutiva(int hubs[], int NUM_HUBS, int numeroDeHubs) {
    memset(hubs, -1, sizeof(int) * numeroDeHubs);
    for (int i = 0; i < numeroDeHubs; i++) hubs[i] = i;
>>>>>>> Stashed changes
}

long double calcularDistancia(const Node& a, const Node& b) {
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

void calcularMatrizDeDistancias(double matrizDistancias[MAX_NOS][MAX_NOS], Node nos[MAX_NOS], int MAX_NOS) {
    for (int i = 0; i < MAX_NOS; ++i) {
        for (int j = i + 1; j < MAX_NOS; ++j) { 
            matrizDistancias[i][j] = calcularDistancia(nos[i], nos[j]);
            matrizDistancias[j][i] = matrizDistancias[i][j];
        }
    }
}

<<<<<<< Updated upstream
vector<double> dijkstra(int origem, const vector<vector<Edge>>& grafo) {
    int n = grafo.size();
    vector<double> distancia(n, numeric_limits<double>::max());
    priority_queue<pair<double, int>, vector<pair<double, int>>, greater<>> pq;

    // Inicializa a distância do nó origem como 0 e coloca na fila de prioridade
    distancia[origem] = 0.0;
    pq.push({0.0, origem});

    while (!pq.empty()) {
        auto [distAtual, noAtual] = pq.top();
        pq.pop();

        // Verifica todos os vizinhos do nó atual
        for (const auto& vizinho : grafo[noAtual]) {
            double novaDistancia = distAtual + vizinho.peso;

            // Se encontramos um caminho menor, atualizamos a distância
            if (novaDistancia < distancia[vizinho.destino]) {
                distancia[vizinho.destino] = novaDistancia;
                pq.push({novaDistancia, vizinho.destino});
            }
        }
    }

    return distancia;  // Retorna o vetor de distâncias da origem para todos os outros nós
}

// Calcula a matriz de distâncias entre todos os pares de nós usando Dijkstra
vector<vector<double>> calcularMatrizDeDistancias(const vector<Node>& nodes) {
    int n = nodes.size();
    vector<vector<Edge>> grafo(n);

    // Criar o grafo: todos os nós conectados com todos (Grafo Completo)
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            double distancia = calcularDistancia(nodes[i], nodes[j]);
            grafo[i].push_back({j, distancia});
            grafo[j].push_back({i, distancia});  // Grafo não-direcionado
        }
    }

    // Calcular a matriz de distâncias rodando Dijkstra para cada nó
    vector<vector<double>> matrizDist(n, vector<double>(n, 0.0));

    for (int i = 0; i < n; i++) {
        matrizDist[i] = dijkstra(i, grafo);  // Calcula as distâncias do nó i para todos os outros
    }

    return matrizDist;
}

double calculoFOvetor(const std::vector<Node>& nodes, const std::vector<int>& hubs) {
    const double beta = 1.0;
    const double alpha = 0.75;
    const double lambda = 1.0;
    
    double maxCost = 0.0; // Armazena o maior custo t_ij

    // Iterar sobre todos os pares de nós (i, j)
    for (size_t i = 0; i < nodes.size(); ++i) {
        for (size_t j = 0; j < nodes.size(); ++j) {
            if (i == j) continue; // Ignorar par (i, i)

            double minCost = std::numeric_limits<double>::max(); // Armazena o menor custo para este par

            // Iterar sobre todas as combinações de hubs (k, l)
            for (int k : hubs) {
                for (int l : hubs) {
                    double cost = beta * calcularDistancia(nodes[i], nodes[k]) +
                                  alpha * calcularDistancia(nodes[k], nodes[l]) +
                                  lambda * calcularDistancia(nodes[l], nodes[j]);

                    minCost = std::min(minCost, cost); // Atualizar o menor custo para o par (i, j)
                }
            }

            maxCost = std::max(maxCost, minCost); // Atualizar o maior custo t_ij encontrado
        }
    }

    return maxCost;
}

double calculoFOmat(const std::vector<Node>& nodes, const std::vector<int>& hubs, const vector<vector<double>>& distancias) {
    const double beta = 1.0;
    const double alpha = 0.75;
    const double lambda = 1.0;
    
    long double maxCost = 0.0; // Armazena o maior custo t_ij

    // Iterar sobre todos os pares de nós (i, j)
    for (int i = 0; i < nodes.size(); ++i) {
        for (int j = 0; j < nodes.size(); ++j) {
            if (i == j) continue; // Ignorar par (i, i)

            long double minCost = std::numeric_limits<long double>::max(); // Armazena o menor custo para este par

            // Iterar sobre todas as combinações de hubs (k, l)
            for (int k : hubs) {
                for (int l : hubs) {
                    // Utiliza a matriz de distâncias para acessar diretamente as distâncias entre os nós
                    long double distIK = distancias[i][k];
                    long double distKL = distancias[k][l];
                    long double distLJ = distancias[l][j];

                    /*
                    cout << "Distância (i, k): Nó " << i + 1 << " -> Hub " << k + 1 << ": " << distIK << endl;
                    cout << "Distância (k, l): Hub " << k + 1 << " -> Hub " << l + 1 << ": " << distKL << endl;
                    cout << "Distância (l, j): Hub " << l + 1 << " -> Nó " << j + 1 << ": " << distLJ << endl;
                    */
                    long double cost = beta * distIK + alpha * distKL + lambda * distLJ;
                    minCost = std::min(minCost, cost); // Atualizar o menor custo para o par (i, j)
                }
            }

            maxCost = std::max(maxCost, minCost); // Atualizar o maior custo t_ij encontrado
=======
void imprimirMatriz(double matrizDistancias[MAX_NOS][MAX_NOS], int MAX_NOS) {
    std::cout << std::fixed << std::setprecision(6);

    for (int i = 0; i < MAX_NOS; ++i) {
        for (int j = 0; j < MAX_NOS; ++j) {
            std::cout << matrizDistancias[i][j] << " ";
>>>>>>> Stashed changes
        }
        std::cout << std::endl;
    }
}

<<<<<<< Updated upstream
double calculoFOguloso(const std::vector<Node>& nodes, const std::vector<int>& hubs, const std::vector<std::vector<double>>& distancias) {
    const double beta = 1.0;
=======
void printHubs(int* hubs, int NUM_HUBS) {
    std::cout << "Hubs selecionados: ";
    for (int i = 0; i < NUM_HUBS; i++) {
        std::cout << hubs[i] << " ";
    }
    std::cout << std::endl;
}

long double calculoFOmat2(int NUM_HUBS, int hubs[], double matrizDistancias[MAX_NOS][MAX_NOS]) {
>>>>>>> Stashed changes
    const double alpha = 0.75;
    long double maxCost = 0.0;

<<<<<<< Updated upstream
    long double maxCost = 0.0; // Armazena o maior custo t_ij

    for (int i = 0; i < nodes.size(); ++i) {
        for (int j = i + 1; j < nodes.size(); ++j) { // Começa do próximo nó

            long double minCost = std::numeric_limits<long double>::max(); // Melhor custo encontrado

            // Estratégia Gulosa: Encontrar o melhor hub `k` para `i` e melhor hub `l` para `j`
            int melhorK = -1, melhorL = -1;
            long double menorDistIK = std::numeric_limits<long double>::max();
            long double menorDistLJ = std::numeric_limits<long double>::max();

            for (int k : hubs) {
                if (distancias[i][k] < menorDistIK) { // Escolhe o melhor hub `k` para `i`
                    menorDistIK = distancias[i][k];
                    melhorK = k;
                }
                if (distancias[j][k] < menorDistLJ) { // Escolhe o melhor hub `l` para `j`
                    menorDistLJ = distancias[j][k];
                    melhorL = k;
                }
            }

            // Se houver pelo menos dois hubs disponíveis, calcula o custo com a melhor conexão
            if (melhorK != -1 && melhorL != -1) {
                long double distKL = distancias[melhorK][melhorL]; // Distância entre os dois hubs
                long double cost = beta * menorDistIK + alpha * distKL + lambda * menorDistLJ;
                minCost = cost;
            }

            maxCost = std::max(maxCost, minCost); // Atualiza o maior custo encontrado
=======
    for (int i = 0; i < NUM_HUBS; ++i) {
        for (int j = i + 1; j < NUM_HUBS; ++j) {
            long double minCost = std::numeric_limits<long double>::max();
            for (int k = 0; k < NUM_HUBS; ++k) {
                for (int l = 0; l < NUM_HUBS; ++l) {
                    // Calcular o custo considerando os hubs e as distâncias
                    long double cost = matrizDistancias[i][hubs[k]] + 
                                       alpha * matrizDistancias[hubs[k]][hubs[l]] + 
                                       matrizDistancias[hubs[l]][j];
                    minCost = std::min(minCost, cost);
                }
            }
            maxCost = std::max(maxCost, minCost);
>>>>>>> Stashed changes
        }
    }
    return maxCost;
}

<<<<<<< Updated upstream
double calculoFOgulosoAleatorio(const vector<Node>& nodes, int numeroDeHubs) {
    int numeroDeNos = nodes.size();
    vector<int> hubs;
    vector<bool> usado(numeroDeNos, false); // Para rastrear quais nós já são hubs

    // 1. Seleção aleatória dos hubs iniciais
    while (hubs.size() < numeroDeHubs) {
        int candidato = rand() % numeroDeNos;
        if (!usado[candidato]) {
            hubs.push_back(candidato);
            usado[candidato] = true;
        }
    }

    // 2. Calcular a FO com heurística gulosa
    long double maxCost = 0.0;
    for (int i = 0; i < numeroDeNos; ++i) {
        for (int j = i + 1; j < numeroDeNos; ++j) {
            if (i == j) continue; // Ignorar distâncias de um nó para ele mesmo

            long double minCost = numeric_limits<long double>::max();

            // Iterar apenas sobre hubs selecionados (reduz complexidade)
            for (int k : hubs) {
                for (int l : hubs) {
                    double distIK = calcularDistancia(nodes[i], nodes[k]);
                    double distKL = calcularDistancia(nodes[k], nodes[l]);
                    double distLJ = calcularDistancia(nodes[l], nodes[j]);

                    long double cost = distIK + 0.75 * distKL + distLJ; // FO com peso
                    minCost = min(minCost, cost); // Menor custo para o par (i, j)
                }
            }

            maxCost = max(maxCost, minCost); // Atualizar o maior custo (FO)
        }
    }

    return maxCost;
}
//-----------------------------//
double calculoFOGulosoVetor(vector<Node>& nodes, const vector<int>& hubs) { 
    int n = nodes.size();  // Número de nós
    int numHubs = hubs.size();  // Número de hubs
    vector<Node> proximoNo(n); // Vetor para armazenar o nó mais próximo de cada nó
=======
>>>>>>> Stashed changes

double calculoFOGulosoVetor(int NUM_HUBS, int hubs[]) {
    double funcaoObjetivo = 0.0;
    for (int i = 0; i < NUM_HUBS; ++i) {
        long double menorDistancia = numeric_limits<long double>::max();
        for (int j = 0; j < NUM_HUBS; ++j) {
            menorDistancia = std::min(menorDistancia, static_cast<long double>(matrizDistancias[i][hubs[j]]));
        }
        funcaoObjetivo += menorDistancia;
    }
    return funcaoObjetivo;
}

<<<<<<< Updated upstream
double calculoFOmat2(const std::vector<Node>& nodes, const std::vector<int>& hubs, const std::vector<std::vector<double>>& distancias) {
    const double beta = 1.0;
    const double alpha = 0.75;
    const double lambda = 1.0;

    long double maxCost = 0.0; // Armazena o maior custo t_ij

    // Iterar sobre os pares de nós (i, j), mas começando de i+1 para evitar repetições
    for (int i = 0; i < nodes.size(); ++i) {
        for (int j = i + 1; j < nodes.size(); ++j) { // Começa do próximo nó (j = i + 1)
            long double minCost = std::numeric_limits<long double>::max(); // Armazena o menor custo para este par

            // Iterar sobre todas as combinações de hubs (k, l)
            for (int k : hubs) {
                for (int l : hubs) {
                    // Utiliza a matriz de distâncias para acessar diretamente as distâncias entre os nós
                    long double distIK = distancias[i][k];
                    long double distKL = distancias[k][l];
                    long double distLJ = distancias[l][j];

                    // Calcula o custo usando os pesos beta, alpha e lambda
                    long double cost = beta * distIK + alpha * distKL + lambda * distLJ;
                    minCost = std::min(minCost, cost); // Atualiza o menor custo para o par (i, j)
                }
            }

            maxCost = std::max(maxCost, minCost); // Atualiza o maior custo t_ij encontrado
        }
    }

    return maxCost;
}

double calculoFOmatGuloso(const std::vector<Node>& nodes, const std::vector<int>& hubs, const std::vector<std::vector<double>>& distancias) {
    const double beta = 1.0;
    const double alpha = 0.75;
    const double lambda = 1.0;

    long double maxCost = 0.0; // Armazena o maior custo t_ij

    // Iterar sobre os pares de nós (i, j) com j > i (evita repetição)
    for (int i = 0; i < nodes.size(); ++i) {
        for (int j = i + 1; j < nodes.size(); ++j) { // Garante j > i
            
            long double minCost = std::numeric_limits<long double>::max(); // Custo mínimo inicializado alto
            
            int melhorHubK = -1, melhorHubL = -1; // Para armazenar os melhores hubs encontrados

            // Escolher hubs de maneira gulosa: pegamos os que minimizam localmente o custo
            for (int k : hubs) {
                for (int l : hubs) {
                    long double distIK = distancias[i][k];
                    long double distKL = distancias[k][l];
                    long double distLJ = distancias[l][j];

                    long double cost = beta * distIK + alpha * distKL + lambda * distLJ;

                    // Se encontramos um custo menor, atualizamos
                    if (cost < minCost) {
                        minCost = cost;
                        melhorHubK = k;
                        melhorHubL = l;
                    }
                }
            }

            // Atualizar o maior custo encontrado
            maxCost = std::max(maxCost, minCost);

            }
    }

    return maxCost;
}


int main() {
    string nomeArquivo = "inst20.txt"; 
    int numeroDeNos;
    int numeroDeHubs = 5;
=======
int main() {
    string nomeArquivo = "inst20.txt";
    int hubs[NUM_HUBS] = {3,5,13,16};
>>>>>>> Stashed changes

    lerArquivoEntrada(nomeArquivo, nos);
    criarArquivoDeSaida(nos, MAX_NOS);
    //heuristicaConstrutiva(hubs, NUM_HUBS, numeroDeHubs);
    //printHubs(hubs, NUM_HUBS);
    calcularMatrizDeDistancias(matrizDistancias, nos, MAX_NOS);
    imprimirMatriz(matrizDistancias, MAX_NOS);

<<<<<<< Updated upstream
    vector<int> hubs = { 3, 5, 13, 16};

    cout << "Número de Nós: " << numeroDeNos << endl;
    cout << "Número de Hubs: " << numeroDeHubs << endl;

    


    // ============================
    // TESTE calculoFOmat2 (Matriz NOVA)
    // ============================
=======
>>>>>>> Stashed changes
    auto start3 = high_resolution_clock::now();
    long double result3 = 0.0;
    for (int i = 0; i < 10000; i++) {
        result3 = calculoFOmat2(NUM_HUBS, hubs, matrizDistancias);
    }
    auto end3 = high_resolution_clock::now();
    auto duration3 = duration_cast<microseconds>(end3 - start3);
    cout << "Maior custo - Matriz NOVA: " << fixed << setprecision(2) << result3 << endl;
    cout << "Tempo - Matriz NOVA: " << duration3.count() << " microssegundos" << endl;

    auto start = high_resolution_clock::now();
    double result = 0.0;
    for (int i = 0; i < 10000; i++) {
        result = calculoFOGulosoVetor(NUM_HUBS, hubs);
    }
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    cout << "Maior custo - Guloso VETOR: " << fixed << setprecision(2) << result << endl;
    cout << "Tempo - Guloso VETOR: " << duration.count() << " microssegundos" << endl;

    return 0;
}
/*
void resultadoUMApHCP(){
    printf("n: %i   ", MAX_NOS);
    printf("p: %i", NUM_HUBS);
    printf("FO: %f  ", );
    printf("\n\n");
    printf("");
}
*/