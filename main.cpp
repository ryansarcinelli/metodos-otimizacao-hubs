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
    for (int i = 0; i <= NUM_NOS; ++i) {
        arquivo << nos[i].x << " " << nos[i].y << endl;
    }

    arquivo.close();
}

void heuristicaConstrutiva(int hubs[], int NUM_HUBS, int numeroDeHubs) {
    memset(hubs, -1, sizeof(int) * numeroDeHubs);
    for (int i = 0; i < numeroDeHubs; i++) hubs[i] = i;
}

long double calcularDistancia(const Node& a, const Node& b) {
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
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
            if(i > j)
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

int main() {
    string nomeArquivo = "inst20.txt";
    int hubs[NUM_HUBS] = {3,5,13,16};

    lerArquivoEntrada(nomeArquivo, nos);
    criarArquivoDeSaida(nos, MAX_NOS);
    //heuristicaConstrutiva(hubs, NUM_HUBS, numeroDeHubs);
    //printHubs(hubs, NUM_HUBS);
    calcularMatrizDeDistancias(matrizDistancias, nos, MAX_NOS);
    imprimirMatriz(matrizDistancias, MAX_NOS);

    auto start3 = high_resolution_clock::now();
    long double result3 = 0.0;
    for (int i = 0; i < 10000; i++) {
        result3 = calculoFOmat(NUM_HUBS, hubs, matrizDistancias);
    }
    auto end3 = high_resolution_clock::now();
    auto duration3 = duration_cast<microseconds>(end3 - start3);
    cout << "-------------\n";
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