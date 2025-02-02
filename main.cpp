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

void heuristicaConstrutiva() {
    // Determinar o ponto central do conjunto de nós
    double centroX = 0.0, centroY = 0.0;
    for (int i = 0; i < NUM_HUBS; i++) {
        centroX += nos[i].x; // Soma das coordenadas X
        centroY += nos[i].y; // Soma das coordenadas Y
    }
    centroX /= NUM_HUBS; // Média das coordenadas X
    centroY /= NUM_HUBS; // Média das coordenadas Y

    // Determinar o ponto mais distante para definir o raio maior
    double raioMaior = 0.0;
    for (int i = 0; i < NUM_HUBS; i++) {
        double distancia = sqrt(pow(nos[i].x - centroX, 2) + pow(nos[i].y - centroY, 2)); // Distância euclidiana
        if (distancia > raioMaior) {
            raioMaior = distancia; // Atualiza o maior raio encontrado
        }
    }
    double raioMenor = raioMaior / 2.0; // Define o raio menor como metade do maior

    // Divisão dos nós em setores e círculos concêntricos
    vector<int> setores[4][2]; // Matriz de vetores: [setor][círculo] -> 0: menor, 1: maior
    for (int i = 0; i < NUM_HUBS; i++) {
        double dx = nos[i].x - centroX;
        double dy = nos[i].y - centroY;
        double distancia = sqrt(dx * dx + dy * dy); // Distância do nó ao centro
        int setor = (dx >= 0 && dy >= 0) ? 0 : (dx < 0 && dy >= 0) ? 1 : (dx < 0 && dy < 0) ? 2 : 3; // Determina o setor
        int circulo = (distancia <= raioMenor) ? 0 : 1; // Determina se está no círculo menor ou maior
        setores[setor][circulo].push_back(i); // Armazena o índice do nó
    }

    // Selecionar hubs alternando entre círculos e setores
    int count = 0;
    bool maiorPrimeiro = true; // Alterna entre priorizar o círculo maior ou menor
    while (count < NUM_HUBS) {
        // Primeira iteração pelos setores
        for (int s = 0; s < 4 && count < NUM_HUBS; s++) {
            int circuloIdx = maiorPrimeiro ? 1 : 0; // Define se busca no maior ou menor primeiro
            if (!setores[s][circuloIdx].empty()) {
                hubs[count++] = setores[s][circuloIdx].back(); // Seleciona um nó como hub
                setores[s][circuloIdx].pop_back(); // Remove o nó selecionado
            }
        }
        // Segunda iteração pelos setores
        for (int s = 0; s < 4 && count < NUM_HUBS; s++) {
            int circuloIdx = maiorPrimeiro ? 0 : 1; // Alterna a busca entre maior e menor
            if (!setores[s][circuloIdx].empty()) {
                hubs[count++] = setores[s][circuloIdx].back(); // Seleciona outro nó
                setores[s][circuloIdx].pop_back(); // Remove o nó selecionado
            }
        }
        maiorPrimeiro = !maiorPrimeiro; // Inverte a ordem de prioridade
    }
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

                    printf("calculo de fo: %f\n", cost);
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
                for (int l = 0; l < NUM_HUBS; ++l) {
                    long double cost = matrizDistancias[i][hubs[k]] + 
                                       alpha * matrizDistancias[hubs[k]][hubs[l]] + 
                                       matrizDistancias[hubs[l]][j];

                    minCost = MIN(minCost, cost);  // Usando o macro MIN
                }
            }
            #pragma omp critical
            maxCost = MAX(maxCost, minCost);  // Usando o macro MAX
        }
    }
    
    return maxCost;
}

void heuristicaConstrutiva() {
    // Determinar o ponto central do conjunto de nós
    double centroX = 0.0, centroY = 0.0;
    for (int i = 0; i < NUM_HUBS; i++) {
        centroX += nos[i].x; // Soma das coordenadas X
        centroY += nos[i].y; // Soma das coordenadas Y
    }
    centroX /= NUM_HUBS; // Média das coordenadas X
    centroY /= NUM_HUBS; // Média das coordenadas Y

    // Determinar o ponto mais distante para definir o raio maior
    double raioMaior = 0.0;
    for (int i = 0; i < NUM_HUBS; i++) {
        double distancia = sqrt(pow(nos[i].x - centroX, 2) + pow(nos[i].y - centroY, 2)); // Distância euclidiana
        if (distancia > raioMaior) {
            raioMaior = distancia; // Atualiza o maior raio encontrado
        }
    }
    double raioMenor = raioMaior / 2.0; // Define o raio menor como metade do maior

    // Divisão dos nós em setores e círculos concêntricos
    vector<int> setores[4][2]; // Matriz de vetores: [setor][círculo] -> 0: menor, 1: maior
    for (int i = 0; i < NUM_HUBS; i++) {
        double dx = nos[i].x - centroX;
        double dy = nos[i].y - centroY;
        double distancia = sqrt(dx * dx + dy * dy); // Distância do nó ao centro
        int setor = (dx >= 0 && dy >= 0) ? 0 : (dx < 0 && dy >= 0) ? 1 : (dx < 0 && dy < 0) ? 2 : 3; // Determina o setor
        int circulo = (distancia <= raioMenor) ? 0 : 1; // Determina se está no círculo menor ou maior
        setores[setor][circulo].push_back(i); // Armazena o índice do nó
    }

    // Selecionar hubs alternando entre círculos e setores
    int count = 0;
    bool maiorPrimeiro = true; // Alterna entre priorizar o círculo maior ou menor
    while (count < NUM_HUBS) {
        // Primeira iteração pelos setores
        for (int s = 0; s < 4 && count < NUM_HUBS; s++) {
            int circuloIdx = maiorPrimeiro ? 1 : 0; // Define se busca no maior ou menor primeiro
            if (!setores[s][circuloIdx].empty()) {
                hubs[count++] = setores[s][circuloIdx].back(); // Seleciona um nó como hub
                setores[s][circuloIdx].pop_back(); // Remove o nó selecionado
            }
        }
        // Segunda iteração pelos setores
        for (int s = 0; s < 4 && count < NUM_HUBS; s++) {
            int circuloIdx = maiorPrimeiro ? 0 : 1; // Alterna a busca entre maior e menor
            if (!setores[s][circuloIdx].empty()) {
                hubs[count++] = setores[s][circuloIdx].back(); // Seleciona outro nó
                setores[s][circuloIdx].pop_back(); // Remove o nó selecionado
            }
        }
        maiorPrimeiro = !maiorPrimeiro; // Inverte a ordem de prioridade
    }
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

