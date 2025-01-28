#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <cmath>
#include <limits>
#include <cstdlib>
#include <ctime>
#include <chrono>

using namespace std;
using namespace chrono;

struct Node {
    double x, y; // Coordenadas x e y do nó
};

struct Solucao {
    double FO; // Função objetivo
    vector<int> hubs; // Hubs selecionados
};

Solucao clonarSolucao(const Solucao& solucaoOriginal) {
    Solucao novaSolucao;
    novaSolucao.FO = solucaoOriginal.FO; // Clonando a FO
    novaSolucao.hubs = solucaoOriginal.hubs; // Clonando os hubs
    return novaSolucao;
}

// Função para ler a instância do arquivo
vector<Node> lerArquivoEntrada(const string& nomeArquivo, int& numeroDeNos) {
    ifstream arquivo(nomeArquivo);
    if (!arquivo.is_open()) {
        cerr << "Erro ao abrir o arquivo: " << nomeArquivo << endl;
        exit(1);
    }

    arquivo >> numeroDeNos; // Lê o número de nós
    vector<Node> nos;

    Node no;
    for (int i = 0; i < numeroDeNos; ++i) {
        arquivo >> no.x >> no.y; // Lê as coordenadas
        nos.push_back(no);
    }

    arquivo.close();
    return nos;
}

// Função para exibir os nós
void exibirNos(const vector<Node>& nos) {
    cout << fixed << setprecision(8); // Define precisão de 8 casas decimais
    for (size_t i = 0; i < nos.size(); ++i) {
        cout << "Nó " << i + 1 << ": (" << nos[i].x << ", " << nos[i].y << ")" << endl;
    }
}

// Função para selecionar 4 hubs aleatórios
vector<int> selecionarHubsAleatorios(int numeroDeNos, int numeroDeHubs) {
    vector<int> hubs;
    vector<bool> usado(numeroDeNos, false); // Marca se o nó já foi escolhido como hub
    srand(time(0)); // Inicializa a semente aleatória com base no tempo

    while (hubs.size() < numeroDeHubs) {
        int candidato = rand() % numeroDeNos; // Gera um índice aleatório
        if (!usado[candidato]) {
            hubs.push_back(candidato);
            usado[candidato] = true; // Marca o nó como usado
        }
    }

    return hubs;
}

// Função para exibir os hubs
void exibirHubs(const vector<int>& hubs) {
    cout << "Hubs selecionados: ";
    for (size_t i = 0; i < hubs.size(); ++i) {
        cout << hubs[i] << (i < hubs.size() - 1 ? ", " : "\n");
    }
}

// Função para calcular a distância euclidiana entre dois nós
double calculateDistance(const Node& a, const Node& b) {
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

vector<vector<double>> calcularMatrizDeDistancias(const vector<Node>& nodes) {
    int n = nodes.size();
    vector<vector<double>> distancias(n, vector<double>(n));

    // Preencher a matriz de distâncias
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (i != j) {
                distancias[i][j] = calculateDistance(nodes[i], nodes[j]);
            } else {
                distancias[i][j] = 0.0; // Distância de um nó para ele mesmo
            }
        }
    }

    return distancias;
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
                    double cost = beta * calculateDistance(nodes[i], nodes[k]) +
                                  alpha * calculateDistance(nodes[k], nodes[l]) +
                                  lambda * calculateDistance(nodes[l], nodes[j]);

                    minCost = std::min(minCost, cost); // Atualizar o menor custo para o par (i, j)
                }
            }

            maxCost = std::max(maxCost, minCost); // Atualizar o maior custo t_ij encontrado
        }
    }

    return maxCost;
}

// Função para calcular a função objetivo (maior custo t_ij)
double calculoFOmat(const std::vector<Node>& nodes, const std::vector<int>& hubs, const vector<vector<double>>& distancias) {
    const double beta = 1.0;
    const double alpha = 0.75;
    const double lambda = 1.0;
    
    double maxCost = 0.0; // Armazena o maior custo t_ij

    // Iterar sobre todos os pares de nós (i, j)
    for (int i = 0; i < nodes.size(); ++i) {
        for (int j = 0; j < nodes.size(); ++j) {
            if (i == j) continue; // Ignorar par (i, i)

            double minCost = std::numeric_limits<double>::max(); // Armazena o menor custo para este par

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
                    double cost = beta * distIK + alpha * distKL + lambda * distLJ;
                    minCost = std::min(minCost, cost); // Atualizar o menor custo para o par (i, j)
                }
            }

            maxCost = std::max(maxCost, minCost); // Atualizar o maior custo t_ij encontrado
        }
    }

    return maxCost;
}


int main() {
    string nomeArquivo = "inst20.txt"; 
    int numeroDeNos;
    int numeroDeHubs = 4;

    // Lê os nós do arquivo
    vector<Node> nos = lerArquivoEntrada(nomeArquivo, numeroDeNos);


    vector<int> hubs = { 3, 5, 13, 16 };

    cout << "Número de Nós: " << numeroDeNos << endl;
    cout << "Número de Hubs: " << numeroDeHubs << endl;

    // Medir o tempo para calculoFOvetor
    auto start1 = high_resolution_clock::now();

    double result1 = calculoFOvetor(nos, hubs);

    auto end1 = high_resolution_clock::now();
    auto duration1 = duration_cast<microseconds>(end1 - start1);
    cout << "Maior custo de transporte (função objetivo) - Vetor: " << result1 << endl;
    cout << "Tempo de execução - Vetor: " << duration1.count() << " microssegundos" << endl;

    // Medir o tempo para calculoFOMat
    auto start2 = high_resolution_clock::now();

    vector<vector<double>> matrizDeDistancias = calcularMatrizDeDistancias(nos);
    double result2 = calculoFOmat(nos, hubs, matrizDeDistancias);
    
    auto end2 = high_resolution_clock::now();
    auto duration2 = duration_cast<microseconds>(end2 - start2);
    cout << "Maior custo de transporte (função objetivo) - Matriz: " << result2 << endl <<;
    cout << "Tempo de execução - Matriz: " << duration2.count() << " microssegundos" << endl;

    return 0;
}