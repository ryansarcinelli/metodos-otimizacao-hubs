#include "main.h"
#include <chrono>
#include <cstring>

using namespace std;
using namespace chrono;

// Definição das variáveis globais
int numNos = 0;
int numHubs = 5;  // Número de hubs definido estaticamente (pode ser ajustado)
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

//
// Função: calcularCentro
// Calcula o centro geométrico dos nós
//
Node calcularCentro() {
    double somaX = 0.0, somaY = 0.0;
    for (int i = 0; i < numNos; ++i) {
        somaX += nos[i].x;
        somaY += nos[i].y;
    }
    Node centro;
    centro.x = somaX / numNos;
    centro.y = somaY / numNos;
    return centro;
}

//
// Função: selecionarHubs
// Seleciona hubs com base na distância ao centro (metade dos mais próximos e metade dos mais distantes)
//
void selecionarHubs(int hubsSelecionados[]) {
    // Estrutura auxiliar para armazenar o par (distância, índice)
    struct Par {
        double dist;
        int index;
    };
    Par distancias[MAX_NOS];
    
    Node centro = calcularCentro();
    for (int i = 0; i < numNos; ++i) {
        double d = sqrt(pow(nos[i].x - centro.x, 2) + pow(nos[i].y - centro.y, 2));
        distancias[i].dist = d;
        distancias[i].index = i;
    }
    
    // Ordena o array de distâncias
    sort(distancias, distancias + numNos, [](const Par& a, const Par& b) {
        return a.dist < b.dist;
    });
    
    int metade = numHubs / 2;
    for (int i = 0; i < metade; i++) {
        hubsSelecionados[i] = distancias[i].index;
    }
    for (int i = metade; i < numHubs; i++) {
        hubsSelecionados[i] = distancias[numNos - 1 - (i - metade)].index;
    }
}

//
// Função: calcularMatrizDeDistancias
// Calcula a matriz de distâncias entre todos os nós
//
void calcularMatrizDeDistancias() {
    for (int i = 0; i < numNos; ++i) {
        for (int j = i; j < numNos; ++j) {
            double d = sqrt(pow(nos[i].x - nos[j].x, 2) + pow(nos[i].y - nos[j].y, 2));
            matrizDistancias[i][j] = d;
            matrizDistancias[j][i] = d;
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
    const double alpha = 0.75;
    double maxCost = 0.0;
    for (int i = 0; i < numNos; ++i) {
        for (int j = i + 1; j < numNos; ++j) {
            double minCost = numeric_limits<double>::max();
            for (int k = 0; k < numHubs; ++k) {
                for (int l = 0; l < numHubs; ++l) {
                    double cost = matrizDistancias[i][hubsSelecionados[k]] +
                                  alpha * matrizDistancias[hubsSelecionados[k]][hubsSelecionados[l]] +
                                  matrizDistancias[hubsSelecionados[l]][j];
                    if (cost < minCost)
                        minCost = cost;
                }
            }
            if (minCost > maxCost)
                maxCost = minCost;
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

    string nomeArquivoEntrada = "inst20.txt"; // O arquivo de entrada deve conter: número de nós e, em seguida, as coordenadas
    lerArquivoEntrada(nomeArquivoEntrada);
    
    calcularMatrizDeDistancias();
    
    int hubsSelecionados[MAX_HUBS];
    memset(hubsSelecionados, 0, sizeof(hubsSelecionados));
    
    auto start = high_resolution_clock::now();
    selecionarHubs(hubsSelecionados);
    double FO = calculoFO(hubsSelecionados);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "Execution Time (Hub Selection and FO Calculation): " 
         << duration.count() << " microseconds" << endl;
    
    criarArquivoDeSaida("saida.txt");
    salvarResultados("resultados.txt", hubsSelecionados, FO);
    
}
