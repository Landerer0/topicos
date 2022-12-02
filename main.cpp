#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <fstream>
#include <string>

#include "kll.hpp"

using namespace std;

// Parametros globales
unsigned long numElements = pow(2,5); // numero de elementos totales
double epsilon = 1; // tamaño del arreglo mas grande
double numC=0.5; // factor por el que va disminuyendo el tamaño de cada arreglo a medida pasan los niveles.
bool manualLectura = false;

void lecturaManual(KLL &kll){
    vector<long> elementos;
    for(int i=0;i<numElements;i++){
        // elementos.push_back(i);
        elementos.push_back(rand()%(10*numElements));
    }

    auto rng = std::default_random_engine {};
    std::shuffle(std::begin(elementos), std::end(elementos), rng);

    cout << "elementos a insertar listos" << endl;

    for(int i=0;i<numElements;i++){
        kll.add(elementos.at(i));
        //cout << "termino" << endl;
    }
    cout << endl;

    cout << "KLL Final:" << endl;
    kll.print();

    int numQuery = 10;
    vector<long> query;
    for(int i=0;i<numQuery;i++){
        query.push_back(elementos.at(rand()%elementos.size()));
    }
    sort(query.begin(),query.end());

    cout << endl << "RESULTADOS:" << endl;
    // print de rank y select
    for(int i=0;i<numQuery;i++){
        long queryRank = kll.rank(query.at(i));
        cout << "rank de " << query.at(i) << ": " << queryRank << endl; 
        cout << "select de " << queryRank << ": " << kll.select(queryRank) << endl; 
        for(int j=0;j<10;j++){
            cout << "- ";
        }
        cout << endl;
    }
}

void lecturaStream(KLL &kll, string nombreFichero){
    cerr << "Nombre fichero: " << nombreFichero << endl;
    ifstream fichero(nombreFichero);

    if(fichero.is_open()) cerr << "Fichero se abrio correctamente" << endl;
    else{
        cerr << "No se detecto un fichero" << endl;
        exit(1);
    }

    long elemento;
    while(!fichero.eof()){
        fichero >> elemento;
        if(fichero.eof()) break;
        //cerr << elemento << endl;
        kll.add(elemento);
    }
}

int main(int argc, char*argv[]){
    KLL kll(numElements, epsilon, numC);
    //kll.print();

    // lectura manual o por stream
    if(argc==1) lecturaManual(kll);
    else if(argc==2) lecturaStream(kll,argv[1]);
    else{
        cout << "Numero de parametros incorrectos" << endl;
        return 0;
    }

    // print de quantiles
    cout << "quantile 0.25: " << kll.quantile(0.25) << endl;
    cout << "quantile 0.50: " << kll.quantile(0.50) << endl;
    cout << "quantile 0.75: " << kll.quantile(0.75) << endl;
    cout << "quantile 1.00: " << kll.quantile(1.00) << endl;

    return 0;
}