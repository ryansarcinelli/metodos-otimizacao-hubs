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
    long double dist;
};

struct Solucao {
    double FO; 
    vector<int> hubs;
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
}

long double calcularDistancia(const Node& a, const Node& b) {
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

vector<vector<double>> calcularMatrizDeDistancias(const vector<Node>& nodes) {
    int n = nodes.size();
    vector<vector<double>> distancias(n, vector<double>(n, 0.0)); // Inicializa a matriz com 0.0

    // Calcular apenas a metade superior da matriz (evitando cálculos repetidos)
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            double distancia = calcularDistancia(nodes[i], nodes[j]);
            distancias[i][j] = distancia; // Preenche a parte superior
            distancias[j][i] = distancia; // Espelha na parte inferior
        }
    }

    return distancias;
}

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
        }
    }

    return maxCost;
}

double calculoFOguloso(const std::vector<Node>& nodes, const std::vector<int>& hubs, const std::vector<std::vector<double>>& distancias) {
    const double beta = 1.0;
    const double alpha = 0.75;
    const double lambda = 1.0;

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
        }
    }

    return maxCost;
}

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

    // Inicializando a matriz de distâncias
    vector<vector<long double>> distanciasDeHubParaNos(numHubs, vector<long double>(n)); // Distâncias de cada hub para todos os nós

    // Calculando a menor distância de cada nó para um hub
    for (int i = 0; i < n; ++i) {
        long double menorDistancia = std::numeric_limits<long double>::max();
        int hubMaisProximo = -1;

        // Verifica a menor distância do nó i até qualquer hub
        for (int j = 0; j < numHubs; ++j) {
            int hub = hubs[j];  // Obtém o hub atual da lista de hubs
            long double distancia = calcularDistancia(nodes[i], nodes[hub]); // Distância do nó i até o hub
            if (distancia < menorDistancia) {
                menorDistancia = distancia;
                hubMaisProximo = hub; // Armazena o índice do hub mais próximo
            }

            // Agora, armazena a distância de cada hub para o nó i
            distanciasDeHubParaNos[j][i] = distancia; // Armazenando a distância no vetor
        }

        // Agora, atribui o nó mais próximo ao vetor proximoNo
        proximoNo[i] = nodes[hubMaisProximo]; // Armazenando o nó mais próximo no vetor proximoNo

        // Agora, atribui a distância ao nó mais próximo
        proximoNo[i].dist = menorDistancia; // Modificando diretamente o atributo 'dist' do nó mais próximo
    }

    // Agora, você pode usar o vetor 'proximoNo' e a distância armazenada diretamente nos nós.
    // Para exemplo, você pode calcular a função objetivo:
    double funcaoObjetivo = 0.0;
    for (int i = 0; i < n; ++i) {
        if (proximoNo[i].dist != -1) {  // Verifica se a distância foi realmente calculada
            funcaoObjetivo += proximoNo[i].dist;  // Aqui, você usa diretamente a distância salva
        }
    }

    return funcaoObjetivo; // Retorna o valor da função objetivo
}

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

    // Lê os nós do arquivo
    vector<Node> nos = lerArquivoEntrada(nomeArquivo, numeroDeNos);

    vector<int> hubs = { 3, 5, 13, 16};

    cout << "Número de Nós: " << numeroDeNos << endl;
    cout << "Número de Hubs: " << numeroDeHubs << endl;

    


    // ============================
    // TESTE calculoFOmat2 (Matriz NOVA)
    // ============================
    auto start3 = high_resolution_clock::now();
    double result3 = 0.0;
    for (int i = 0; i < 100; i++) {
        vector<vector<double>> matrizDeDistancias = calcularMatrizDeDistancias(nos);
        result3 = calculoFOmat2(nos, hubs, matrizDeDistancias);
    }
    auto end3 = high_resolution_clock::now();
    auto duration3 = duration_cast<microseconds>(end3 - start3);

    cout << "Maior custo - Matriz NOVA: " << fixed << setprecision(2) << result3 << endl;
    cout << "Tempo - Matriz NOVA: " << duration3.count() << " microssegundos" << endl;

    // ============================
    // TESTE calculoFOmatGuloso (Nova Heurística)
    // ============================
    auto start5 = high_resolution_clock::now();
    double result5 = 0.0;
    for (int i = 0; i < 100; i++) {
        vector<vector<double>> matrizDeDistancias5 = calcularMatrizDeDistancias(nos);
        result5 = calculoFOmatGuloso(nos, hubs, matrizDeDistancias5);
    }
    auto end5 = high_resolution_clock::now();
    auto duration5 = duration_cast<microseconds>(end5 - start5);

    cout << "Maior custo - Guloso DOIS: " << fixed << setprecision(2) << result5 << endl;
    cout << "Tempo - Guloso DOIS: " << duration5.count() << " microssegundos" << endl;

    // ============================
    // TESTE calculoFOmatGuloso (Nova Heurística)
    // ============================
    auto start = high_resolution_clock::now();
    double result = 0.0;
    for (int i = 0; i < 100; i++) {
        result = calculoFOGulosoVetor(nos, hubs);
    }
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);

    cout << "Maior custo - Guloso VETOR: " << fixed << setprecision(2) << result << endl;
    cout << "Tempo - Guloso VETOR: " << duration.count() << " microssegundos" << endl;

    return 0;
}

/*
    Parâmetro: instância a ser considerada e o número de hubs. ---

    Procedimento ou função para leitura da instância. ---

    Estruturas de dados necessárias para armazenar os dados de entrada. ---

    Estruturas de dados para armazenar uma solução. O código deve ser capaz de armazenar ---
várias soluções diferentes.

    Procedimento ou função para criação de uma solução inicial viável para o problema ---
(heurística construtiva). *

    Procedimento ou função para clonar uma solução qualquer. ---

    Procedimento ou função para calcular a função objetivo de uma solução qualquer. ** ---

    Procedimento ou função para “escrever” (em tela e arquivo) uma solução qualquer no
formato descrito na Figura 3. 
*/