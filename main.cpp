#include <iostream>
#include <fstream>
#include <iomanip>  // Para std::fixed e std::setprecision
#include <chrono>   // Para medir tempo de execução
#include <cstring>  // Para memset e memcpy
#include "main.h" // Seu cabeçalho com as declarações

using namespace std;
using namespace std::chrono;

// Solucao melhorSolucao;
// memset(melhorSolucao.hubs, -1, sizeof(melhorSolucao.hubs));
// melhorSolucao.FO = 0.0;

// Solucao clonarSolucao(const Solucao& solucaoOriginal) {
//     Solucao novaSolucao;
//     novaSolucao.FO = solucaoOriginal.FO;
//     memcpy(novaSolucao.hubs, solucaoOriginal.hubs, sizeof(solucaoOriginal.hubs));
//     return novaSolucao;
// }

double matrizDistancias[MAX_NOS][MAX_NOS];
Node nos[MAX_NOS];

void lerArquivoEntrada(const string& nomeArquivo, Node nos[MAX_NOS]) {
    ifstream arquivo(nomeArquivo);
    if (!arquivo.is_open()) exit(1);

    int NUM_NOS;
    arquivo >> NUM_NOS;

    // arquivo >> std::fixed >> std::setprecision(6);

    for (int i = 0; i < NUM_NOS; ++i) {
        arquivo >> nos[i].x >> nos[i].y;
    }

    arquivo.close();
}


// void criarArquivoDeSaida(Node nos[MAX_NOS], int NUM_NOS) {
//     ofstream arquivo("saida.txt");
//     if (!arquivo.is_open()) exit(1);

//     arquivo << NUM_NOS << endl;

    // arquivo << std::fixed << std::setprecision(6);
    // for (int i = 0; i <= NUM_NOS; ++i) {
    //     arquivo << nos[i].x << " " << nos[i].y << endl;
    // }

//     arquivo.close();
// }

long double calcDistEuclidiana(const Node& a, const Node& b) {
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

void calcMatDistancias(double matrizDistancias[MAX_NOS][MAX_NOS], Node nos[MAX_NOS]) {
    for (int i = 0; i <= MAX_NOS; ++i) {
        for (int j = i + 1; j < MAX_NOS; ++j) { 
            matrizDistancias[i][j] = calcDistEuclidiana(nos[i], nos[j]);
            matrizDistancias[j][i] = matrizDistancias[i][j];
        }
    }
}

void imprimirMatriz(double matrizDistancias[MAX_NOS][MAX_NOS]) {
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
                double dist_ik = matrizDistancias[i][hubs[k]];
                
                for (int l = 0; l < NUM_HUBS; ++l) {
                    double dist_k_l = alpha * matrizDistancias[hubs[k]][hubs[l]];
                    double dist_lj = matrizDistancias[hubs[l]][j];

                    long double cost = dist_ik + dist_k_l + dist_lj;
                    minCost = std::min(minCost, cost);
                }
            }

            maxCost = std::max(maxCost, minCost);
        }
    }

    return maxCost;
}


//TERMINAR ISSO APÓS CONCLUIR O CALCULO DA FO, DEPENDE DE OUTRAS VARIÁVEIS
// void resultadoUMApHCP(long double FO){
//     printf("n: %i   ", MAX_NOS);
//     printf("p: %i", NUM_HUBS);
//     printf("FO: %f  ", FO);
//     for(int i = 0; i < NUM_HUBS; i++){
//         printf("H%i  ", )
//     }
//     printf("\n\n");
//     printf("");
// }

void heuristicaConstrutiva(int hubs[], int NUM_HUBS, int totalHubs) {
    memset(hubs, -1, sizeof(int) * totalHubs);
    for (int i = 0; i < totalHubs; i++) hubs[i] = i;
}


int main() {
    string nomeArquivo = "inst20.txt";
    int hubs[NUM_HUBS] = {3,5,13,16};

    lerArquivoEntrada(nomeArquivo, nos);
    //criarArquivoDeSaida(nos, MAX_NOS);
    //heuristicaConstrutiva(hubs, NUM_HUBS, numeroDeHubs);
    //printHubs(hubs, NUM_HUBS);
    calcMatDistancias(matrizDistancias, nos);
    imprimirMatriz(matrizDistancias);

    auto start3 = high_resolution_clock::now();
    long double result3 = 0.0;
    for (int i = 0; i < 10000; i++) {
        result3 = calculoFOmat(NUM_HUBS, hubs, matrizDistancias);
    }

    //resultadoUMApHCP(result3);

    auto end3 = high_resolution_clock::now();
    auto duration3 = duration_cast<microseconds>(end3 - start3);
    cout << "-------------------\n";
    cout << "Maior custo - Matriz NOVA: " << fixed << setprecision(2) << result3 << endl;
    cout << "Tempo - Matriz NOVA: " << duration3.count() << " microssegundos" << endl;

    return 0;
}
