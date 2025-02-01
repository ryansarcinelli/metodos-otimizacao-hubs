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

long double calculoFOmat(int NUM_HUBS, int hubs[], double matrizDistancias[MAX_NOS][MAX_NOS]) {
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
                     //std::cout << "Custo entre " << i << " e " << j << " com k=" << k << " e l=" << l << ": " << cost << std::endl;

                    minCost = std::min(minCost, cost);
                }
            }
            maxCost = std::max(maxCost, minCost);
        }
    }
    
//    std::cout << "Maior custo calculado: " << maxCost << std::endl;
    return maxCost;
}



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
        result3 = calculoFOmat(NUM_HUBS, hubs, matrizDistancias);
    }
    auto end3 = high_resolution_clock::now();
    auto duration3 = duration_cast<microseconds>(end3 - start3);
    cout << "Maior custo - Matriz NOVA: " << fixed << setprecision(2) << result3 << endl;
    cout << "Tempo - Matriz NOVA: " << duration3.count() << " microssegundos" << endl;

    return 0;
}
