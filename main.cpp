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
#include <algorithm>
#include <cstring>
#include <vector>
#include "main.h"

#define MAX(X,Y) ((X > Y) ? X : Y)
#define MIN(X,Y) ((X < Y) ? X : Y)


using namespace std;
using namespace chrono;

<<<<<<< Updated upstream
Solucao clonarSolucao(const Solucao& solucaoOriginal) {
    Solucao novaSolucao;
    novaSolucao.FO = solucaoOriginal.FO;
    memcpy(novaSolucao.hubs, solucaoOriginal.hubs, sizeof(solucaoOriginal.hubs));
    return novaSolucao;
}

=======
// Definição das variáveis globais
const string nomeArquivoEntrada = "inst20.txt";
int numNos = 20;
int numHubs = 2;  // Número de hubs definido estaticamente (pode ser ajustado)
Node nos[MAX_NOS];
double matrizDistancias[MAX_NOS][MAX_NOS];


>>>>>>> Stashed changes
void lerArquivoEntrada(const string& nomeArquivo) {

    ifstream arquivo(nomeArquivo);
    if (!arquivo.is_open()) exit(1);

    int NUM_NOS;
    arquivo >> NUM_NOS;
    for (int i = 0; i < NUM_NOS; ++i) {
        arquivo >> nos[i].x >> nos[i].y;
    }

    arquivo.close();
}

<<<<<<< Updated upstream
long double calcularDistancia(const Node& a, const Node& b) {
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

void criarArquivoDeSaida() {
    ofstream arquivo("saida.txt");
    if (!arquivo.is_open()) exit(1);

     arquivo << NUM_NOS << endl;

    arquivo << std::fixed << std::setprecision(6);
     for (int i = 0; i <= NUM_NOS; ++i) {
         arquivo << nos[i].x << " " << nos[i].y << endl;
     }
     arquivo.close();
}


Node calcularCentro() {
    double somaX = 0.0, somaY = 0.0;
    
    for (int i = 0; i < NUM_NOS; ++i) {
        somaX += nos[i].x;
        somaY += nos[i].y;
    }
    
    return {somaX / NUM_NOS, somaY / NUM_NOS};
}
=======
struct Candidato
{
    double maxDistMin;  // Menor entre as maiores distâncias
    int index;
};



void selecionarHubs(Solucao &sol) {
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
    sort(candidatos.begin(), candidatos.end(), [](const Candidato &a, const Candidato &b) {
        return a.maxDistMin < b.maxDistMin;
    });

    // Selecionar aleatoriamente dentro do top %tal melhores candidatos
    double beta = 0.2;
    int limite = max(1, static_cast<int>(beta * numNos));

    for (int i = 0; i < numHubs; i++) {
        int escolhido = rand() % limite; // Escolhe um índice dentro do top %tal melhores
        sol.hubs[i] = candidatos[escolhido].index;

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
        for (int j = i + 1; j < numNos; ++j) { // Começa de i + 1
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
double calculoFO(Solucao &sol) {
    if (numHubs <= 0) {
        return -1;
    }

    const double alpha = 0.75;
    double maxCost = 0.0;
>>>>>>> Stashed changes

void selecionarHubs() {
    
    Node centro = calcularCentro();

<<<<<<< Updated upstream
    std::pair<double, int> distancias[NUM_NOS];
=======
            for (int k = 0; k < numHubs; ++k) {
                int hubK = sol.hubs[k];
>>>>>>> Stashed changes

    for (int i = 0; i < NUM_NOS; ++i) {
        double distanciaCentro = calcularDistancia(nos[i], centro);
        distancias[i] = {distanciaCentro, i};
    }

<<<<<<< Updated upstream
    std::sort(distancias, distancias + NUM_NOS);
=======
                for (int l = 0; l < numHubs; ++l) {
                    int hubL = sol.hubs[l];
>>>>>>> Stashed changes

    for (int i = 0; i < NUM_HUBS / 2; ++i) {
        hubs[i] = distancias[i].second; 
    }
    for (int i = NUM_HUBS / 2; i < NUM_HUBS; ++i) {
        hubs[i] = distancias[NUM_NOS - 1 - (i - NUM_HUBS / 2)].second; 
    }
}

<<<<<<< Updated upstream
void calcularMatrizDeDistancias() {
    for (int i = 0; i <= NUM_NOS; ++i) {
        for (int j = i + 1; j < NUM_NOS; ++j) { 
            matrizDistancias[i][j] = calcularDistancia(nos[i], nos[j]);
            matrizDistancias[j][i] = matrizDistancias[i][j];
        }
    }
}

void imprimirMatriz() {
    std::cout << std::fixed << std::setprecision(6);

    for (int i = 0; i < NUM_NOS; ++i) {
        for (int j = 0; j < NUM_NOS; ++j) {
            std::cout << matrizDistancias[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

void printHubs(int* hubs, int NUM_HUBS) {
    std::cout << "Hubs selecionados: ";
    for (int i = 0; i < NUM_HUBS; i++) {
        std::cout << hubs[i] << " ";
    }
    std::cout << std::endl;
}

long double calculoFOmat() {
    const double alpha = 0.75;
    long double maxCost = 0.0;

    for (int i = 0; i < NUM_NOS; ++i) {
        for (int j = i + 1; j < NUM_NOS; ++j) {
            long double minCost = std::numeric_limits<long double>::max();
            for (int k = 0; k < NUM_HUBS; ++k) {
                for (int l = 0; l < NUM_HUBS; ++l) {
                    long double cost = matrizDistancias[i][hubs[k]] + 
                                       alpha * matrizDistancias[hubs[k]][hubs[l]] + 
                                       matrizDistancias[hubs[l]][j];

                    
                    minCost = MIN(minCost, cost);  
                }
            }
            maxCost = MAX(maxCost, minCost);  // Equivalente ao std::max
        }
    }
    
    return maxCost;
}

long double calculoFOmatParalelo() {
    const double alpha = 0.75;
    long double maxCost = 0.0;

    #pragma omp parallel for reduction(max:maxCost)  // Paraleliza o loop e usa redução para maxCost
    for (int i = 0; i < NUM_NOS; ++i) {
        for (int j = i + 1; j < NUM_NOS; ++j) {
            long double minCost = std::numeric_limits<long double>::max();

            for (int k = 0; k < NUM_HUBS; ++k) {
                double dist_ik = matrizDistancias[i][hubs[k]];
                
                for (int l = 0; l < NUM_HUBS; ++l) {

                    double cost = matrizDistancias[i][hubs[k]] + 
                                       alpha * matrizDistancias[hubs[k]][hubs[l]] + 
                                       matrizDistancias[hubs[l]][j];
                    printf( "OR: %d H1: %d H2: %d DS: %d: %.2lf\n", i, hubs[k], hubs[l], j, cost);
                    minCost = MIN(minCost, cost);  // Usando o macro MIN
                }
            }
            #pragma omp critical
            maxCost = MAX(maxCost, minCost);  // Usando o macro MAX
=======
                    double cost = matrizDistancias[i][hubK] +
                                  alpha * matrizDistancias[hubK][hubL] +
                                  matrizDistancias[hubL][j];

                    if (cost < minCost) {
                        minCost = cost;
                    }
                }
            }

            if (minCost > maxCost) {
                maxCost = minCost;
            }
        }
    }

    sol.FO = maxCost;
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
void salvarResultados(const string& nomeArquivo, const Solucao& sol) {
    criarArquivoDeSaida(nomeArquivo);

    ofstream arquivo(nomeArquivo);
    if (!arquivo.is_open()) {
        cerr << "Erro ao abrir o arquivo para escrita: " << nomeArquivo << endl;
        exit(1);
    }
    
    arquivo << "n: " << numNos << " p: " << numHubs << "\n";
    arquivo << fixed << setprecision(2) << "FO: " << sol.FO << "\n";
    arquivo << "HUBS: [";
    for (int i = 0; i < numHubs; ++i) {
        arquivo << sol.hubs[i];
        if (i < numHubs - 1)
            arquivo << ", ";
    }
    arquivo << "]\n\n";
    
    arquivo << "OR\tH1\tH2\tDS\tCUSTO\n";
    const double alpha = 0.75;
    for (int i = 0; i < numNos; ++i) {
        for (int j = 0; j < numNos; ++j) {
            double minCost;
            int bestHub1 = -1, bestHub2 = -1;

            if (i == j) {
                minCost = 0.0;
            } else {
                minCost = numeric_limits<double>::max();
                for (int k = 0; k < numHubs; ++k) {
                    for (int l = 0; l < numHubs; ++l) {
                        double custo = matrizDistancias[i][sol.hubs[k]] +
                                       alpha * matrizDistancias[sol.hubs[k]][sol.hubs[l]] +
                                       matrizDistancias[sol.hubs[l]][j];
                        if (custo < minCost) {
                            minCost = custo;
                            bestHub1 = sol.hubs[k];
                            bestHub2 = sol.hubs[l];
                        }
                    }
                }
            }

            arquivo << i << "\t" << bestHub1 << "\t" << bestHub2 << "\t" << j << "\t" 
                    << fixed << setprecision(2) << minCost << "\n";
>>>>>>> Stashed changes
        }
    }
    
    return maxCost;
}

<<<<<<< Updated upstream
void salvaCaminhos() {
    const double alpha = 0.75;
    double maxCost = 0.0;
    int optimalOR = -1, optimalH1 = -1, optimalH2 = -1, optimalDS = -1;
    double optimalCost = -1.0; // Inicializado com um valor baixo

    // Abrindo o arquivo para escrita
    FILE *outFile = fopen("resultados.txt", "w");

    if (outFile == nullptr) {
        std::cerr << "Erro ao abrir o arquivo para escrita!" << std::endl;
        return;
    }

    for (int i = 0; i < NUM_NOS; ++i) {
        for (int j = i + 1; j < NUM_NOS; ++j) {
            double minCost = std::numeric_limits<double>::max();
            for (int k = 0; k < NUM_HUBS; ++k) {
                for (int l = 0; l < NUM_HUBS; ++l) {
                    double cost = matrizDistancias[i][hubs[k]] + 
                                  alpha * matrizDistancias[hubs[k]][hubs[l]] + 
                                  matrizDistancias[hubs[l]][j];

                    // Salvando os resultados no arquivo
                    if(k%16==0){
                        fprintf(outFile, "OR: %d H1: %d H2: %d DS: %d: %.2lf\n", i, hubs[k], hubs[l], j, cost);
                    }
                    
                    // Verifica se é a solução com maior custo até agora
                    if (cost > optimalCost) {  
                        optimalCost = cost;
                        optimalOR = i;
                        optimalH1 = hubs[k];  // Pegando os valores reais dos hubs
                        optimalH2 = hubs[l];
                        optimalDS = j;
                    }

                    minCost = MIN(minCost, cost);
                }
            }
            maxCost = MAX(maxCost, minCost);
        }
    }

    // Escrevendo a solução ótima no final do arquivo
    fprintf(outFile, "\nSolução Ótima:\n");
    fprintf(outFile, "OR: %d H1: %d H2: %d DS: %d FO: %.2lf\n", 
            optimalOR, optimalH1, optimalH2, optimalDS, optimalCost);

    // Fechando o arquivo após escrever os dados
    fclose(outFile);
}

int main() {
    string nomeArquivo = "inst200.txt";
    int numsol=0;
    lerArquivoEntrada(nomeArquivo);
    criarArquivoDeSaida();
    //heuristicaConstrutiva();
    //printHubs(hubs, NUM_HUBS);
=======

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

Solucao criarNovaSolucao() {
    Solucao novaSolucao;

    selecionarHubs(novaSolucao);

    novaSolucao.FO = calculoFO(novaSolucao);

    return novaSolucao;
}

int main() {

    lerArquivoEntrada(nomeArquivoEntrada);
>>>>>>> Stashed changes
    calcularMatrizDeDistancias();
    //imprimirMatriz();
    salvaCaminhos();
    selecionarHubs();

<<<<<<< Updated upstream
    std::cout << "Hubs selecionados: ";
    for (int i = 0; i < NUM_HUBS; ++i) {
        std::cout << hubs[i] << " ";
    }
    std::cout << std::endl;

    

    auto start3 = high_resolution_clock::now();
    long double result3 = 0.0;
    for (int i = 0; i < 1; i++) {
        result3 = calculoFOmat();
    }

    //resultadoUMApHCP(result3);

    auto end3 = high_resolution_clock::now();
    auto duration3 = duration_cast<microseconds>(end3 - start3);

    cout << "Maior custo - Matriz : " << fixed << setprecision(2) << result3 << endl;
    cout << "Tempo - Matriz : " << duration3.count() << " microssegundos" << endl;
    
    auto start = high_resolution_clock::now();
    long double result = 0.0;
    for (int i = 0; i < 1; i++) {
        result = calculoFOmatParalelo();
    }
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    cout << "Maior custo - Matriz : " << fixed << setprecision(2) << result << endl;
    cout << "Tempo - Matriz : " << duration.count() << " microssegundos" << endl;
    
    //escreverSolucao("solucao.txt", result3, hubs, atribuicoes, numAtribuicoes);
    return 0;
}
=======
    Solucao solucoes[50]; // Array para armazenar 50 soluções

    auto start = high_resolution_clock::now();

    // Criar 50 soluções usando a função criarNovaSolucao()
    for (int i = 0; i < 50; ++i) {
        solucoes[i] = criarNovaSolucao(); // Chamada da função para gerar a solução
    }

    auto stop = high_resolution_clock::now();
    auto duration_ns = duration_cast<nanoseconds>(stop - start);

    // Imprimir todas as FO's e os hubs selecionados
    cout << "Funções Objetivo e Hubs das 50 soluções:\n";
    for (int i = 0; i < 50; ++i) {
        printf("Solução %d: FO = %.2f | Hubs: [", i + 1, solucoes[i].FO);
        
        for (int j = 0; j < numHubs; ++j) {
            cout << solucoes[i].hubs[j];
            if (j < numHubs - 1) {
                cout << ", ";
            }
        }
        cout << "]\n";
    }

    cout << "Execution Time (Generating 50 solutions): " 
         << duration_ns.count() << " nanoseconds" << endl;

    // Encontrar a melhor solução (menor FO)
    int melhorIndice = 0;
    for (int i = 1; i < 50; ++i) {
        if (solucoes[i].FO < solucoes[melhorIndice].FO) {
            melhorIndice = i;
        }
    }

    // Salvar a melhor solução
    salvarResultados("melhor_resultado.txt", solucoes[melhorIndice]);

    return 0;
}


>>>>>>> Stashed changes
