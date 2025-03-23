#include "main.h"
#include <chrono>
#include <cstring>
#include <vector>
#include <algorithm>
#include <random>
#include <cfloat>

using namespace std;
using namespace chrono;

// Definição das variáveis globais
int numNos = 20;
int numHubs = 2;  // Número de hubs definido estaticamente (pode ser ajustado)
Node nos[MAX_NOS];
double matrizDistancias[MAX_NOS][MAX_NOS];

//
// Função: lerArquivoEntrada
// Formato do arquivo de entrada:
// Linha 1: <número de nós>
// Linhas seguintes: <coordenada x> <coordenada y>
//
void lerArquivoEntrada(const string& nomeArquivo) {
    ifstream arquivo(nomeArquivo);
    if (!arquivo.is_open()){
        cerr << "Erro ao abrir o arquivo de entrada." << endl;
        exit(1);
    }
    
    // Lê somente o número de nós, pois o arquivo não traz o número de hubs
    arquivo >> numNos;
    if(numNos > MAX_NOS){
        cerr << "Erro: número de nós excede o limite máximo (" << MAX_NOS << ")." << endl;
        exit(1);
    }
    
    for (int i = 0; i < numNos; ++i) {
        arquivo >> nos[i].x >> nos[i].y;
    }
    arquivo.close();
}

struct Candidato
{
    double maxDistMin;  // Menor entre as maiores distâncias
    int index;
};


void selecionarHubs(int hubsSelecionados[]) {
    vector<Candidato> candidatos;

    // Inicializar a semente do gerador de números aleatórios apenas uma vez
    static bool seeded = false;
    if (!seeded) {
        srand((unsigned)time(NULL));
        seeded = true;
    }

    // Calcular a maior distância mínima para cada nó
    for (int i = 0; i < numNos; ++i) {
        double maxDist = 0.0;
        for (int j = 0; j < numNos; ++j) {
            if (i != j) {
                maxDist = max(maxDist, matrizDistancias[i][j]);
            }
        }
        candidatos.push_back({maxDist, i});
    }

    // Ordenar os nós pelos menores valores de "maior distância"
    sort(candidatos.begin(), candidatos.end(), [](const Candidato& a, const Candidato& b) {
        return a.maxDistMin < b.maxDistMin;
    });

    // Selecionar aleatoriamente dentro do top %tal melhores candidatos
    double beta = 0.2;
    int limite = max(1, static_cast<int>(beta * numNos));

    for (int i = 0; i < numHubs; i++) {
        int escolhido = rand() % limite;  // Escolhe um índice dentro do top %tal melhores
        hubsSelecionados[i] = candidatos[escolhido].index;

        // Remover o escolhido da lista para evitar repetições
        candidatos.erase(candidatos.begin() + escolhido);

        // Ajustar o limite para evitar estouro
        if (!candidatos.empty()) {
            limite = max(1, static_cast<int>(beta * candidatos.size()));
        }
    }
}

//
// Função: calcularMatrizDeDistancias
// Calcula a matriz de distâncias entre todos os nós
//
void calcularMatrizDeDistancias() {
    for (int i = 0; i < numNos; ++i) {
        for (int j = i + 1; j < numNos; ++j) { // Começa de i + 1
            double dx = nos[i].x - nos[j].x;
            double dy = nos[i].y - nos[j].y;
            double d = sqrt(dx * dx + dy * dy);
            matrizDistancias[i][j] = matrizDistancias[j][i] = d;
        }
    }
}


//
// Função: imprimirMatriz
// Imprime a matriz de distâncias
//
void imprimirMatriz() {
    cout << fixed << setprecision(6);
    for (int i = 0; i < numNos; ++i) {
        for (int j = 0; j < numNos; ++j) {
            cout << matrizDistancias[i][j] << " ";
        }
        cout << endl;
    }
}

//
// Função: calculoFO
// Calcula a função objetivo (FO) utilizando a matriz pré-computada e os hubs selecionados
//
double calculoFO(const int hubsSelecionados[]) {
    if (numHubs <= 0 || hubsSelecionados == nullptr)
    {
        return -1;  
    }
    
    const double alpha = 0.75;
    double maxCost = 0.0;

    for (int i = 0; i < numNos; ++i) {
        for (int j = i + 1; j < numNos; ++j) {
            double minCost = DBL_MAX;

            for (int k = 0; k < numHubs; ++k) {
                int hubK = hubsSelecionados[k];

                // Verifica se o índice do hub está dentro dos limites da matriz
                if (hubK < 0 || hubK >= numNos) continue;

                for (int l = 0; l < numHubs; ++l) {
                    int hubL = hubsSelecionados[l];

                    if (hubL < 0 || hubL >= numNos) continue;

                    double cost = matrizDistancias[i][hubK] +
                                  alpha * matrizDistancias[hubK][hubL] +
                                  matrizDistancias[hubL][j];

                    if (cost < minCost){
                        minCost = cost;
                    }
                }
            }

            if (minCost > maxCost){
                maxCost = minCost;
            }
        }
    }
    return maxCost;
}

//
// Função: criarArquivoDeSaida
// Cria um arquivo com os nós (coordenadas)
//
void criarArquivoDeSaida(const string& nomeArquivo) {
    ofstream arquivo(nomeArquivo);
    if (!arquivo.is_open()){
        cerr << "Erro ao abrir o arquivo de saída." << endl;
        exit(1);
    }
    
    arquivo << numNos << "\n";
    arquivo << fixed << setprecision(6);
    for (int i = 0; i < numNos; ++i) {
        arquivo << nos[i].x << " " << nos[i].y << "\n";
    }
    arquivo.close();
}

//
// Função: salvarResultados
// Salva os resultados (nós, hubs, FO e tabela de custos) em um arquivo
//
void salvarResultados(const string &nomeArquivo, const int hubsSelecionados[], double FO) {
    criarArquivoDeSaida(nomeArquivo);

    ofstream arquivo(nomeArquivo);
    if (!arquivo.is_open()){
        cerr << "Erro ao abrir o arquivo para escrita: " << nomeArquivo << endl;
        exit(1);
    }
    
    arquivo << "n: " << numNos << " p: " << numHubs << "\n";
    arquivo << fixed << setprecision(2) << "FO: " << FO << "\n";
    arquivo << "HUBS: [";
    for (int i = 0; i < numHubs; ++i) {
        arquivo << hubsSelecionados[i];
        if(i < numHubs - 1)
            arquivo << ", ";
    }
    arquivo << "]\n\n";
    
    arquivo << "OR\tH1\tH2\tDS\tCUSTO\n";
    const double alpha = 0.75;
    for (int i = 0; i < numNos; ++i) {
        for (int j = 0; j < numNos; ++j) {
            if (i == j) continue;
            double minCost = numeric_limits<double>::max();
            int bestHub1 = -1, bestHub2 = -1;
            for (int k = 0; k < numHubs; ++k) {
                for (int l = 0; l < numHubs; ++l) {
                    double custo = matrizDistancias[i][hubsSelecionados[k]] +
                                   alpha * matrizDistancias[hubsSelecionados[k]][hubsSelecionados[l]] +
                                   matrizDistancias[hubsSelecionados[l]][j];
                    if(custo < minCost){
                        minCost = custo;
                        bestHub1 = hubsSelecionados[k];
                        bestHub2 = hubsSelecionados[l];
                    }
                }
            }
            arquivo << i << "\t" << bestHub1 << "\t" << bestHub2 << "\t" << j << "\t" 
                    << fixed << setprecision(2) << minCost << "\n";
        }
    }
    arquivo.close();
}

//
// Função: lerResultados
// Lê os dados de um arquivo de resultados e os armazena na estrutura Dados
//
Dados lerResultados(const string& nomeArquivo) {
    Dados dados;
    ifstream arquivo(nomeArquivo);
    if (!arquivo) {
        cerr << "Erro ao abrir o arquivo: " << nomeArquivo << endl;
        return dados;
    }
    
    string linha;
    getline(arquivo, linha);
    sscanf(linha.c_str(), "n: %d p: %d", &dados.n, &dados.p);
    
    getline(arquivo, linha);
    sscanf(linha.c_str(), "FO: %lf", &dados.FO);
    
    getline(arquivo, linha);
    size_t pos1 = linha.find("[");
    size_t pos2 = linha.find("]");
    string hubsStr = linha.substr(pos1+1, pos2-pos1-1);
    istringstream iss(hubsStr);
    for (int i = 0; i < dados.p; i++) {
        int hub;
        iss >> hub;
        dados.hubs[i] = hub;
        if(iss.peek() == ',')
            iss.ignore();
    }
    
    // Pula linha em branco e cabeçalho da tabela
    getline(arquivo, linha);
    getline(arquivo, linha);
    
    int entradaIndex = 0;
    while(getline(arquivo, linha)) {
        if(linha.empty()) continue;
        Entrada entrada;
        istringstream issLinha(linha);
        issLinha >> entrada.OR >> entrada.H1 >> entrada.H2 >> entrada.DS >> entrada.custo;
        dados.entradas[entradaIndex++] = entrada;
    }
    dados.numEntradas = entradaIndex;
    arquivo.close();
    return dados;
}

//
// Função: main
//

int main() {
    string nomeArquivoEntrada = "inst20.txt"; // O arquivo de entrada deve conter: número de nós e as coordenadas
    
    // Ler os dados do arquivo de entrada
    lerArquivoEntrada(nomeArquivoEntrada);
    
    // Calcular a matriz de distâncias
    calcularMatrizDeDistancias();

    // Criar um array para armazenar os hubs selecionados
    int hubsSelecionados[MAX_HUBS];
    memset(hubsSelecionados, 0, sizeof(hubsSelecionados));

    // Inicia a medição do tempo com alta precisão
    auto start = high_resolution_clock::now();

    // Selecionar os hubs
    selecionarHubs(hubsSelecionados);

    // Calcular a função objetivo
    double FO = calculoFO(hubsSelecionados);

    // Finaliza a medição do tempo
    auto stop = high_resolution_clock::now();
    
    // Mede a duração em nanosegundos para maior precisão
    auto duration_ns = duration_cast<nanoseconds>(stop - start);
    
    cout << "Execution Time (Hub Selection and FO Calculation): " 
         << duration_ns.count() << " nanoseconds" << endl;

    // Salvar os resultados
    salvarResultados("resultados.txt", hubsSelecionados, FO);
    
    return 0;
}
