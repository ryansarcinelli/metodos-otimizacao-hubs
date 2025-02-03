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
#include <sstream>

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


void salvarResultados(const std::string &nomeArquivo, int n, int p, double FO, const int hubsSelecionados[]) {
    std::ofstream arquivo(nomeArquivo);
    if (!arquivo.is_open()) {
        std::cerr << "Erro ao abrir o arquivo para escrita: " << nomeArquivo << std::endl;
        exit(1);
    }
    
    arquivo << "n: " << n << " p: " << p << "\n";
    
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << FO;
    std::string foStr = ss.str();
    for (char &c : foStr) if (c == '.') c = ',';
    arquivo << "FO: " << foStr << "\n";
    
    arquivo << "HUBS: [";
    for (int i = 0; i < p; i++) {
        arquivo << hubsSelecionados[i] << (i < p - 1 ? ", " : "");
    }
    arquivo << "]\n\n";
    
    arquivo << "OR\tH1\tH2\tDS\tCUSTO\n";
    const double alpha = 0.75;
    
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i == j) continue; // Remove redundância quando origem e destino são iguais
            
            double minCost = std::numeric_limits<double>::max();
            int bestHub1 = -1, bestHub2 = -1;
            
            for (int k = 0; k < p; k++) {
                for (int l = 0; l < p; l++) {
                    double custo = calcularDistancia(nos[i], nos[hubsSelecionados[k]]) +
                                   alpha * calcularDistancia(nos[hubsSelecionados[k]], nos[hubsSelecionados[l]]) +
                                   calcularDistancia(nos[hubsSelecionados[l]], nos[j]);
                    
                    if (custo < minCost) {
                        minCost = custo;
                        bestHub1 = hubsSelecionados[k];
                        bestHub2 = hubsSelecionados[l];
                    }
                }
            }
            
            std::stringstream ssCusto;
            ssCusto << std::fixed << std::setprecision(2) << minCost;
            std::string custoStr = ssCusto.str();
            
            arquivo << i << "\t" << bestHub1 << "\t" << bestHub2 << "\t" << j << "\t" << custoStr << "\n";
        }
    }
    
    arquivo.close();
}

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>

struct Entrada {
    int OR;
    int H1;
    int H2;
    int DS;
    double CUSTO;  // CUSTO como double para valores decimais
};

struct Dados {
    int n;
    int p;
    double FO;
    std::vector<int> HUBS;
    std::vector<Entrada> entradas;
};

Dados lerResultados(const std::string& nomeArquivo) {
    Dados dados;
    std::ifstream arquivo(nomeArquivo);
    std::string linha;

    if (!arquivo) {
        std::cerr << "Erro ao abrir o arquivo!" << std::endl;
        return dados;
    }

    // Lendo n e p
    std::getline(arquivo, linha);
    std::sscanf(linha.c_str(), "n: %d p: %d", &dados.n, &dados.p);

    // Lendo FO
    std::getline(arquivo, linha);
    std::sscanf(linha.c_str(), "FO: %lf", &dados.FO);

    // Lendo HUBS
    std::getline(arquivo, linha);
    linha = linha.substr(linha.find("[") + 1, linha.find("]") - linha.find("[") - 1);
    std::istringstream fluxoHubs(linha);
    int hub;
    while (fluxoHubs >> hub) {
        dados.HUBS.push_back(hub);
        if (fluxoHubs.peek() == ',') fluxoHubs.ignore();
    }

    // Ignorar cabeçalho da tabela
    std::getline(arquivo, linha);

    // Lendo entradas da tabela
    while (std::getline(arquivo, linha)) {
        Entrada entrada;
        std::istringstream iss(linha);
        
        // Lendo a linha da tabela
        iss >> entrada.OR >> entrada.H1 >> entrada.H2 >> entrada.DS >> entrada.CUSTO;

        // // Verificar se a leitura foi bem-sucedida
        // if (iss.fail()) {
        //     std::cerr << "Erro ao ler a linha: " << linha << std::endl;
        //     continue;  // Ignora a linha com erro
        // }

        dados.entradas.push_back(entrada);
    }

    arquivo.close();
    return dados;
}



int main() {

    // string nomeArquivo = "inst20.txt";
    // int numsol=0;
    // lerArquivoEntrada(nomeArquivo);
    // criarArquivoDeSaida();
    // //heuristicaConstrutiva();
    // //printHubs(hubs, NUM_HUBS);
    // calcularMatrizDeDistancias();
    // //imprimirMatriz();
    // selecionarHubs();

    // std::cout << "Hubs selecionados: ";
    // for (int i = 0; i < NUM_HUBS; ++i) {
    //     std::cout << hubs[i] << " ";
    // }
    // std::cout << std::endl;

    

    // auto start3 = high_resolution_clock::now();
    // long double FO = 0.0;
    // for (int i = 0; i < 1; i++) {
    //     FO = calculoFOmat();
    // }

    // salvarResultados("resultados.txt", NUM_NOS, NUM_HUBS, FO, hubs);
    return 0;
}