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
#include "main.h"

using namespace std;
using namespace chrono;


Solucao clonarSolucao(const Solucao& solucaoOriginal) {
    Solucao novaSolucao;
    novaSolucao.FO = solucaoOriginal.FO;
    memcpy(novaSolucao.hubs, solucaoOriginal.hubs, sizeof(solucaoOriginal.hubs));
    return novaSolucao;
}

void lerArquivoEntrada(const string& nomeArquivo, Node nos[MAX_NOS]) {
    ifstream arquivo(nomeArquivo);
    if (!arquivo.is_open()) exit(1);

    int NUM_NOS;
    arquivo >> NUM_NOS;

    //arquivo >> std::fixed >> std::setprecision(6);

    for (int i = 0; i < NUM_NOS; ++i) {
        arquivo >> nos[i].x >> nos[i].y;
    }

    arquivo.close();
}

long double calcularDistancia(const Node& a, const Node& b) {
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

void criarArquivoDeSaida(Node nos[MAX_NOS], int NUM_NOS) {
    ofstream arquivo("saida.txt");
    if (!arquivo.is_open()) exit(1);

    arquivo << NUM_NOS << endl;

    arquivo << std::fixed << std::setprecision(6);
    for (int i = 0; i <= NUM_NOS; ++i) {
        arquivo << nos[i].x << " " << nos[i].y << endl;
    }

    arquivo.close();
}

Node calcularCentro(Node nos[MAX_NOS]) {
    double somaX = 0.0, somaY = 0.0;
    
    for (int i = 0; i < MAX_NOS; ++i) {
        somaX += nos[i].x;
        somaY += nos[i].y;
    }
    
    return {somaX / MAX_NOS, somaY / MAX_NOS};
}

void selecionarHubs(Node nos[MAX_NOS], int hubs[], int NUM_HUBS) {
    
    Node centro = calcularCentro(nos);

    std::pair<double, int> distancias[MAX_NOS];

    for (int i = 0; i < MAX_NOS; ++i) {
        double distanciaCentro = calcularDistancia(nos[i], centro);
        distancias[i] = {distanciaCentro, i};
    }

    std::sort(distancias, distancias + MAX_NOS);

    for (int i = 0; i < NUM_HUBS / 2; ++i) {
        hubs[i] = distancias[i].second; 
    }
    for (int i = NUM_HUBS / 2; i < NUM_HUBS; ++i) {
        hubs[i] = distancias[MAX_NOS - 1 - (i - NUM_HUBS / 2)].second; 
    }
}

void heuristicaConstrutiva(int hubs[], int NUM_HUBS, int numeroDeHubs) {
    memset(hubs, -1, sizeof(int) * numeroDeHubs);
    for (int i = 0; i < numeroDeHubs; i++) hubs[i] = i;
}

void calcularMatrizDeDistancias(double matrizDistancias[MAX_NOS][MAX_NOS], Node nos[MAX_NOS], int MAX_NOS) {
    for (int i = 0; i <= MAX_NOS; ++i) {
        for (int j = i + 1; j < MAX_NOS; ++j) { 
            matrizDistancias[i][j] = calcularDistancia(nos[i], nos[j]);
            matrizDistancias[j][i] = matrizDistancias[i][j];
        }
    }
}

void imprimirMatriz(double matrizDistancias[MAX_NOS][MAX_NOS], int MAX_NOS) {
    std::cout << std::fixed << std::setprecision(6);

    for (int i = 0; i < MAX_NOS; ++i) {
        for (int j = 0; j < MAX_NOS; ++j) {
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

long double calculoFOmat(int hubs[]) {
    const double alpha = 0.75;
    long double maxCost = 0.0;

    for (int i = 0; i < MAX_NOS; ++i) {
        for (int j = i + 1; j < MAX_NOS; ++j) {
            long double minCost = std::numeric_limits<long double>::max();
            for (int k = 0; k < NUM_HUBS; ++k) {
                for (int l = 0; l < NUM_HUBS; ++l) {
                    long double cost = matrizDistancias[i][hubs[k]] + 
                                       alpha * matrizDistancias[hubs[k]][hubs[l]] + 
                                       matrizDistancias[hubs[l]][j];
                        //std::cout << "OR: " << i << " H1: " << k << " H2: " << l << "DS: " << j << ": " << cost << std::endl;

                    minCost = std::min(minCost, cost);
                }
            }
            maxCost = std::max(maxCost, minCost);
        }
    }
    
//    std::cout << "Maior custo calculado: " << maxCost << std::endl;
    return maxCost;
}
/*
void registrarAtribuicoes(int hubs[], int numHubs, double matrizDistancias[MAX_NOS][MAX_NOS], double atribuicoes[MAX_NOS][5]) {
    const double alpha = 0.75;

    for (int i = 0; i < MAX_NOS; ++i) {  
        long double menorCusto = std::numeric_limits<long double>::max();
        int melhorH1 = -1, melhorH2 = -1, melhorDestino = -1;

        for (int k = 0; k < numHubs; ++k) {
            for (int l = 0; l < numHubs; ++l) {
                for (int j = 0; j < MAX_NOS; ++j) {  // Destino possível
                    long double custo = matrizDistancias[i][hubs[k]] +
                                        alpha * matrizDistancias[hubs[k]][hubs[l]] +
                                        matrizDistancias[hubs[l]][j];

                    if (custo < menorCusto) {
                        menorCusto = custo;
                        melhorH1 = hubs[k];
                        melhorH2 = hubs[l];
                        melhorDestino = j;
                    }
                }
            }
        }

        // Salva a melhor atribuição encontrada para esse nó
        atribuicoes[i][0] = i;               // OR
        atribuicoes[i][1] = melhorH1;        // H1
        atribuicoes[i][2] = melhorH2;        // H2
        atribuicoes[i][3] = melhorDestino;   // DS
        atribuicoes[i][4] = menorCusto;      // CUSTO
    }
}

void escreverSolucao(const char* nomeArquivo, double valorFO, 
                     int hubs[], int atribuicoes[][5], int numAtribuicoes) {
    std::ofstream arquivo(nomeArquivo);

    // Cabeçalho
    std::cout << "n: " << MAX_NOS << "   p: " << NUM_HUBS << std::endl;
    arquivo << "n: " << MAX_NOS << "   p: " << NUM_HUBS << std::endl;

    std::cout << "FO: " << std::fixed << std::setprecision(2) << valorFO << std::endl;
    arquivo << "FO: " << std::fixed << std::setprecision(2) << valorFO << std::endl;

    std::cout << "HUBS: [";
    arquivo << "HUBS: [";
    for (int i = 0; i < NUM_HUBS; ++i) {
        std::cout << hubs[i] << (i < NUM_HUBS - 1 ? ", " : "");
        arquivo << hubs[i] << (i < NUM_HUBS - 1 ? ", " : "");
    }
    std::cout << "]" << std::endl;
    arquivo << "]" << std::endl;

    // Cabeçalho da tabela
    std::cout << std::endl << "OR   H1   H2   DS   CUSTO" << std::endl;
    arquivo << std::endl << "OR   H1   H2   DS   CUSTO" << std::endl;

    // Imprimir as linhas da tabela
    for (int i = 0; i < numAtribuicoes; ++i) {
        std::cout << std::setw(2) << atribuicoes[i][0] << "   "
                  << std::setw(2) << atribuicoes[i][1] << "   "
                  << std::setw(2) << atribuicoes[i][2] << "   "
                  << std::setw(2) << atribuicoes[i][3] << "   "
                  << std::fixed << std::setprecision(2) << atribuicoes[i][4] << std::endl;

        arquivo << std::setw(2) << atribuicoes[i][0] << "   "
                << std::setw(2) << atribuicoes[i][1] << "   "
                << std::setw(2) << atribuicoes[i][2] << "   "
                << std::setw(2) << atribuicoes[i][3] << "   "
                << std::fixed << std::setprecision(2) << atribuicoes[i][4] << std::endl;
    }

    arquivo.close();
}
*/
int main() {
    string nomeArquivo = "inst20.txt";
    int hubs[NUM_HUBS];

    lerArquivoEntrada(nomeArquivo, nos);
    criarArquivoDeSaida(nos, MAX_NOS);
    //heuristicaConstrutiva(hubs, NUM_HUBS, numeroDeHubs);
    //printHubs(hubs, NUM_HUBS);
    calcularMatrizDeDistancias(matrizDistancias, nos, MAX_NOS);
    //imprimirMatriz(matrizDistancias, MAX_NOS);

    selecionarHubs(nos, hubs, NUM_HUBS);

    std::cout << "Hubs selecionados: ";
    for (int i = 0; i < NUM_HUBS; ++i) {
        std::cout << hubs[i] << " ";
    }
    std::cout << std::endl;


    auto start3 = high_resolution_clock::now();
    long double result3 = 0.0;
    for (int i = 0; i < 10000; i++) {
        result3 = calculoFOmat(hubs);
    }
    auto end3 = high_resolution_clock::now();
    auto duration3 = duration_cast<microseconds>(end3 - start3);
    cout << "Maior custo - Matriz NOVA: " << fixed << setprecision(2) << result3 << endl;
    cout << "Tempo - Matriz NOVA: " << duration3.count() << " microssegundos" << endl;

    escreverSolucao("solucao.txt", result3, hubs, atribuicoes, numAtribuicoes);
    return 0;
}
