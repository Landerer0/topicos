#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <fstream>
#include <string>
#include <string.h>

#include "kll.hpp"

using namespace std;

// Parametros globales
unsigned long numElements = pow(2,5); // numero de elementos totales
unsigned long numElements2 = pow(2,7); // numero de elementos del segundo archivo
//unsigned long numElements = 2668026; // numero de elementos totales
//unsigned long numElements2 = 20919376; // numero de elementos del segundo archivo

double epsilon = 1; // tamaño del arreglo mas grande
double numC=1; // factor por el que va disminuyendo el tamaño de cada arreglo a medida pasan los niveles.
bool manualLectura = false;

void quantiles(KLL &kll, int numQuantiles){
    if(numQuantiles <= 0 || numQuantiles>100) return;
    for(int i=0;i<numQuantiles;i++){
        int quantil = 100/numQuantiles*i;
        cout << "quantil " << quantil << ": " << kll.quantile(quantil) << endl;
    }
}

vector<long> crearVectorElements(long numElementosCreate){
    vector<long> elementos;
    for(int i=0;i<numElementosCreate;i++){
        // elementos.push_back(i);
        elementos.push_back(rand()%(10*numElementosCreate));
    }

    auto rng = std::default_random_engine {};
    std::shuffle(std::begin(elementos), std::end(elementos), rng);

    return elementos;
}

void lecturaManual(){
    vector<long> elementos = crearVectorElements(numElements);
    vector<long> elementos2 = crearVectorElements(numElements2);
    
    KLL kll(numElements, epsilon, numC);
    KLL kll2(numElements2, epsilon, numC);
    KLL kll3(numElements2, epsilon, numC);

    cout << "elementos a insertar listos" << endl;

    for(int i=0;i<numElements;i++){
        kll.add(elementos.at(i));
        //cout << "termino" << endl;
    }
    cout << endl;

    cout << "KLL1 Final:" << endl;
    kll.print();

    for(int i=0;i<numElements2;i++){
        kll2.add(elementos2.at(i));
        kll3.add(elementos2.at(i));
        //cout << "termino" << endl;
    }
    cout << endl;

    cout << "KLL2 Final:" << endl;
    kll2.print();

    cout << endl <<  "KLL Merge:" << endl;
    KLL merge = kll.kllMerge(kll2);
    merge.print();
    cout << endl;

    // int numQuery = 10;
    // vector<long> query;
    // for(int i=0;i<numQuery;i++){
    //     query.push_back(elementos.at(rand()%elementos.size()));
    // }
    // sort(query.begin(),query.end());

    // cout << endl << "RESULTADOS:" << endl;
    // // print de rank y select
    // for(int i=0;i<numQuery;i++){
    //     long queryRank = kll.rank(query.at(i));
    //     cout << "rank de " << query.at(i) << ": " << queryRank << endl; 
    //     cout << "select de " << queryRank << ": " << kll.select(queryRank) << endl; 
    //     for(int j=0;j<10;j++){
    //         cout << "- ";
    //     }
    //     cout << endl;
    // }

    cout << "fin lectura manual" << endl;
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

    cout << "fin lectura stream" << endl;
}

int countwords(char* filename){
	ifstream fin;
	fin.open(filename);
	char word[30];
	int count=0;
	while(!fin.eof())
	{
		fin>>word;
		count++;
	}
	fin.close();
    return count;
}

void lecturaDeStream(int numStreams, char* argv[]){
    vector<KLL> kllSketches;
    for(int i=0; i<numStreams;i++){
        int wc = countwords(argv[i+1]);
        cout << "Num of Words in " << argv[i+1] << ": " << wc << endl;
        KLL kll(wc,epsilon,numC);
        lecturaStream(kll,argv[i+1]);
        kll.print();
        kllSketches.push_back(kll);
        quantiles(kll,4);
        cout << "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -" << endl;
    }
    if(numStreams>1){
        KLL merge = kllSketches.at(0).kllMerge(kllSketches.at(1));
        cout << "Kll Merge: " << endl;
        merge.print();
        kllSketches.push_back(merge);
        quantiles(merge,4);    
        cout << "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -" << endl;
    }

    cout << "fin lectura stream" << endl;
}

int main(int argc, char*argv[]){
    // lectura manual o por stream
    if(argc==1) lecturaManual();
    else if(argc>=2) lecturaDeStream(argc-1, argv);
    else{
        cout << "Numero de parametros incorrectos" << endl;
        return 0;
    }

    return 0;
}