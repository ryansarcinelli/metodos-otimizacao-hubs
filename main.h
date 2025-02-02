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

<<<<<<< Updated upstream
const int MAX_NOS = 20;
const int NUM_HUBS = 4;
=======
const int NUM_NOS = 20;
const int NUM_HUBS = 4;
int MAX_SOLUCOES = 1000;
int hubs[NUM_HUBS];
>>>>>>> Stashed changes

struct Node {
    double x, y;
    long double dist;
};

struct Solucao {
    double FO;
    int hubs[NUM_HUBS];
};

extern Solucao melhorSolucao;
extern double matrizDistancias[MAX_NOS][MAX_NOS];
extern Node nos[MAX_NOS];

<<<<<<< Updated upstream
void lerArquivoEntrada(const string& nomeArquivo, Node nos[MAX_NOS]);
long double calcDistEuclidiana(const Node& a, const Node& b);
void calcMatDistancias(double matrizDistancias[MAX_NOS][MAX_NOS], Node nos[MAX_NOS]);
void imprimirMatriz(double matrizDistancias[MAX_NOS][MAX_NOS]);
void printHubs(int* hubs, int NUM_HUBS);
long double calculoFOmat(int NUM_HUBS, int hubs[], double matrizDistancias[MAX_NOS][MAX_NOS]);
void heuristicaConstrutiva(int hubs[], int NUM_HUBS, int totalHubs);
Solucao clonarSolucao(const Solucao& solucaoOriginal);
=======

Solucao sol;
extern double matrizDistancias[NUM_NOS][NUM_NOS];
extern Node nos[NUM_NOS];

Solucao clonarSolucao(const Solucao& solucaoOriginal);
void lerArquivoEntrada(const string& nomeArquivo);
long double calcularDistancia(const Node& a, const Node& b);
void criarArquivoDeSaida();
Node calcularCentro();
void selecionarHubs();
void calcularMatrizDeDistancias();
void imprimirMatriz();
void printHubs(int* hubs, int NUM_HUBS);
long double calculoFOmat();
void salvaCaminhos();
>>>>>>> Stashed changes

#endif // MAIN_H