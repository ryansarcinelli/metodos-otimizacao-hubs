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

Solucao clonarSolucao(const Solucao& solucaoOriginal) {
    Solucao novaSolucao;
    novaSolucao.FO = solucaoOriginal.FO;
    memcpy(novaSolucao.hubs, solucaoOriginal.hubs, sizeof(solucaoOriginal.hubs));
    return novaSolucao;
}

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

void selecionarHubs() {
    
    Node centro = calcularCentro();

    std::pair<double, int> distancias[NUM_NOS];

    for (int i = 0; i < NUM_NOS; ++i) {
        double distanciaCentro = calcularDistancia(nos[i], centro);
        distancias[i] = {distanciaCentro, i};
    }

    std::sort(distancias, distancias + NUM_NOS);

    for (int i = 0; i < NUM_HUBS / 2; ++i) {
        hubs[i] = distancias[i].second; 
    }
    for (int i = NUM_HUBS / 2; i < NUM_HUBS; ++i) {
        hubs[i] = distancias[NUM_NOS - 1 - (i - NUM_HUBS / 2)].second; 
    }
}

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
        }
    }
    
    return maxCost;
}

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
    calcularMatrizDeDistancias();
    //imprimirMatriz();
    salvaCaminhos();
    selecionarHubs();

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