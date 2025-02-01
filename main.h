#ifndef MAIN_H
#define MAIN_H

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

// Definição de constantes globais
const int MAX_NOS = 20;   // Número máximo de nós na rede
const int NUM_HUBS = 4;   // Número de hubs a serem selecionados

// Estrutura que representa um nó na rede
typedef struct Node {
    double x, y;   // Coordenadas do nó
    long double dist; // Distância auxiliar para cálculos
} Node;

// Estrutura que representa uma solução para o problema
typedef struct Solucao {
    double FO;         // Função objetivo da solução
    int hubs[NUM_HUBS]; // Array que armazena os hubs selecionados
} Solucao;

// Declaração de variáveis globais externas
extern Solucao melhorSolucao;  // Melhor solução encontrada
extern double matrizDistancias[MAX_NOS][MAX_NOS]; // Matriz de distâncias entre os nós
extern Node nos[MAX_NOS];  // Lista de nós

// Função que lê um arquivo de entrada e armazena os dados nos nós
void lerArquivoEntrada(const string& nomeArquivo, Node nos[MAX_NOS]);

// Função que calcula a distância euclidiana entre dois nós
long double calcDistEuclidiana(const Node& a, const Node& b);

// Função que calcula a matriz de distâncias entre todos os nós
void calcMatDistancias(double matrizDistancias[MAX_NOS][MAX_NOS], Node nos[MAX_NOS]);

// Função que imprime a matriz de distâncias
void imprimirMatriz(double matrizDistancias[MAX_NOS][MAX_NOS]);

// Função que imprime os hubs selecionados
void printHubs(int* hubs, int NUM_HUBS);

// Função que calcula a função objetivo baseada na matriz de distâncias
long double calculoFOmat(int NUM_HUBS, int hubs[], double matrizDistancias[MAX_NOS][MAX_NOS]);

// Função que inicializa uma solução heurística construtiva para os hubs
void heuristicaConstrutiva(int hubs[], int NUM_HUBS, int totalHubs);

// Função que clona uma solução existente
Solucao clonarSolucao(const Solucao& solucaoOriginal);

#endif // MAIN_H