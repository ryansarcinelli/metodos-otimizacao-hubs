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

const int NUM_NOS = 200;
const int NUM_HUBS = 50;
int MAX_SOLUCOES = 1000;
int hubs[NUM_HUBS];

struct Node {
    double x, y;
    long double dist;
};

struct Solucao {
    double FO;
    int hubs[NUM_HUBS];
};

double matrizDistancias[NUM_NOS][NUM_NOS];
Node nos[NUM_NOS];


Solucao sol;
extern double matrizDistancias[NUM_NOS][NUM_NOS];
extern Node nos[NUM_NOS];

Solucao clonarSolucao(const Solucao& solucaoOriginal);
void lerArquivoEntrada(const string& nomeArquivo);
long double calcularDistancia(const Node& a, const Node& b);
void criarArquivoDeSaida();
Node calcularCentro();
void selecionarHubs();
void heuristicaConstrutiva();
void calcularMatrizDeDistancias();
void imprimirMatriz();
void printHubs(int* hubs, int NUM_HUBS);
long double calculoFOmat();

#endif // MAIN_H