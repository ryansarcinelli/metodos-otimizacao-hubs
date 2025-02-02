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

void lerArquivoEntrada(const string& nomeArquivo) {
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

void heuristicaConstrutiva() {
    memset(hubs, -1, sizeof(int) * NUM_HUBS);
    for (int i = 0; i < NUM_HUBS; i++) hubs[i] = i;
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
                        //std::cout << "OR: " << i << " H1: " << k << " H2: " << l << "DS: " << j << ": " << cost << std::endl;

                    minCost = std::min(minCost, cost);
                }
            }
            maxCost = std::max(maxCost, minCost);
        }
    }
    
//    std::cout << "Maior custo calculado: " << maxCost << std::endl;
    sol.FO = maxCost;
    for (int i = 0; i < NUM_HUBS; ++i) {
        sol.hubs[i] = hubs[i];
    }
    return maxCost;
}

int main() {
    string nomeArquivo = "inst200.txt";
    std::array<Solucao, MAX_SOLUCOES> solucoes;
    int numsol=0;
    lerArquivoEntrada(nomeArquivo);
    criarArquivoDeSaida();
    //heuristicaConstrutiva();
    //printHubs(hubs, NUM_HUBS);
    calcularMatrizDeDistancias();
    //imprimirMatriz();

    selecionarHubs();

    std::cout << "Hubs selecionados: ";
    for (int i = 0; i < NUM_HUBS; ++i) {
        std::cout << hubs[i] << " ";
    }
    std::cout << std::endl;

    

    auto start3 = high_resolution_clock::now();
    long double result3 = 0.0;
    //for (int i = 0; i < 10000; i++) {
        result3 = calculoFOmat();
    //}
    auto end3 = high_resolution_clock::now();
    auto duration3 = duration_cast<microseconds>(end3 - start3);
    cout << "Maior custo - Matriz : " << fixed << setprecision(2) << result3 << endl;
    cout << "Tempo - Matriz : " << duration3.count() << " microssegundos" << endl;
    
    
    //escreverSolucao("solucao.txt", result3, hubs, atribuicoes, numAtribuicoes);
    return 0;
}

