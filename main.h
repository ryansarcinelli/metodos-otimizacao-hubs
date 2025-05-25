#ifndef MAIN_H
#define MAIN_H

#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <limits>
#include <cstdlib>
#include <chrono>
#include <algorithm>
#include <sstream>
#include <cstring>

using namespace std;

#define MAX_NOS 1000    // Número máximo de nós permitidos
#define MAX_HUBS 100    // Capacidade máxima para hubs

// O número efetivo de hubs é parametrizado no código (ex.: 4 hubs)

// Estrutura para representar um nó
struct Node {
    double x;
    double y;
};

// Estrutura para representar uma solução

// Estrutura para uma entrada na tabela de resultados
struct Entrada {
    int OR;
    int H1;
    int H2;
    int DS;
    double custo;
};

// Estrutura para armazenar os dados lidos de um arquivo de resultados
// Para simplificar, usamos um array estático para as entradas.


// Arrays globais (vetores estáticos)
extern Node nos[MAX_NOS];
extern double matrizDistancias[MAX_NOS][MAX_NOS];

// Declaração das funções
void lerArquivoEntrada(const string& nomeArquivo);
void criarArquivoDeSaida(const string& nomeArquivo);
Node calcularCentro();
void selecionarHubs(int hubsSelecionados[]);
void calcularMatrizDeDistancias();
void imprimirMatriz();
double calculoFO(const int hubsSelecionados[]);
void salvarResultados(const string &nomeArquivo, const int hubsSelecionados[], double FO);

#endif // MAIN_H
