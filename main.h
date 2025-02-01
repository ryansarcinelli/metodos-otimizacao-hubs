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

extern Solucao melhorSolucao;
extern double matrizDistancias[MAX_NOS][MAX_NOS];
extern Node nos[MAX_NOS];

void lerArquivoEntrada(const string& nomeArquivo, Node nos[MAX_NOS]);
long double calcDistEuclidiana(const Node& a, const Node& b);
void calcMatDistancias(double matrizDistancias[MAX_NOS][MAX_NOS], Node nos[MAX_NOS]);
void imprimirMatriz(double matrizDistancias[MAX_NOS][MAX_NOS]);
void printHubs(int* hubs, int NUM_HUBS);
long double calculoFOmat(int NUM_HUBS, int hubs[], double matrizDistancias[MAX_NOS][MAX_NOS]);
void heuristicaConstrutiva(int hubs[], int NUM_HUBS, int totalHubs);
Solucao clonarSolucao(const Solucao& solucaoOriginal);

#endif // MAIN_H