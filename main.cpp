#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <fstream>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <chrono>

#include "kll.hpp"

using namespace std;

// Parametros globales
unsigned long numElements = pow(2,22); // numero de elementos totales
unsigned long numElements2 = pow(2,7); // numero de elementos del segundo archivo

double epsilon = 1; // tamaño del arreglo mas grande
double numC=0.9; // factor por el que va disminuyendo el tamaño de cada arreglo a medida pasan los niveles.
bool manualLectura = false;

void quantiles(KLL &kll, int numQuantiles){
    if(numQuantiles <= 0 || numQuantiles>100) return;
    for(int i=0;i<numQuantiles;i++){
        int quantil = 100/numQuantiles*i;
        cout << " quantil " << quantil << " : " << kll.quantile(quantil) << endl;
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

void normalElements(int mean, int stdev, int len, int scale){
    std::random_device rd;
    //std::mt19937 gen(rd());
    std::mt19937 gen(1);

    vector<long> elementos;
    std::normal_distribution<> d(mean,stdev);
    for(int n=0; n<len; ++n) {
        elementos.push_back((long)scale*d(gen));
    }
    KLL kll(len,epsilon,numC);
    for(int i=0;i<len;i++){
        kll.add(elementos.at(i));
        //cout << "termino" << endl;
    }

    //cout << endl;
    //cout << "KLL1 Final:" << endl;
    //kll.print();
    cout<<" quantil 0.25 "<<kll.quantile(0.25)<<endl;
    cout<<" quantil 0.5 "<<kll.quantile(0.5)<<endl;
    cout<<" quantil 0.75 "<<kll.quantile(0.75)<<endl;
    cout<<" quantil 0.95 "<<kll.quantile(0.95)<<endl;
    cout<<" rank 100 "<<kll.rank(100)<<endl;
    cout<<" rank 70 "<<kll.rank(70)<<endl;
    cout<<" rank 50 "<<kll.rank(50)<<endl;
    int numQuery = 10;
    vector<long> query;
    for(int i=0;i<numQuery;i++){
        query.push_back(elementos.at(rand()%elementos.size()));
    }
    sort(query.begin(),query.end());
    //sort(elementos.begin(),elementos.end());

    /*
    cout << endl << "RESULTADOS:" << endl;
    unsigned long long sumaErrorRank = 0;
    double duracionTotal = 0; //not sure about the type
    for(int i=0;i<numQuery;i++){
        auto t_start = std::chrono::high_resolution_clock::now();

        long queryRank = kll.rank(query.at(i));
        auto t_end = std::chrono::high_resolution_clock::now();
        double elapsed_time_ms = std::chrono::duration<double, std::nano>(t_end-t_start).count();
        duracionTotal += elapsed_time_ms;
        long rankCorrecto = long(lower_bound(elementos.begin(), elementos.end(), query.at(i)) - elementos.begin());
        cout << "rank de " << query.at(i) << ": " << queryRank << " Tomado en un tiempo de " << endl;
        cout << "rank correcto de " << query.at(i) << ": "
             << rankCorrecto << endl;
        cout << "error de rank: " << abs(queryRank-rankCorrecto) << endl;
        sumaErrorRank+=abs(queryRank-rankCorrecto);
        cout << "select de " << queryRank << ": " << kll.select(queryRank) << endl;
        cout << "select correcto de " << rankCorrecto << ": "
             << elementos.at(rankCorrecto) << endl;

        for(int j=0;j<10;j++){
            cout << "- ";
        }
        cout << endl;
    }
    */
}

void normalDistribution(int mean, int stdev, int len, int scale){
    std::random_device rd;
    std::mt19937 gen(rd());
    KLL kll(numElements, epsilon, numC);
    // if particles decay once per second on average,
    // how much time, in seconds, until the next one?
    std::normal_distribution<> d(mean,stdev);
    for(int n=0; n<len; ++n) {
        kll.addv((long)scale*d(gen));
    }

}

void lecturaManual(){
    vector<long> elementos = crearVectorElements(numElements);
    // vector<long> elementos2 = crearVectorElements(numElements2);
    sort(elementos.begin(),elementos.end()); // para comprobar entrada ordenada

    KLL kll(numElements, epsilon, numC);
    // KLL kll2(numElements2, epsilon, numC);
    // KLL kll3(numElements2, epsilon, numC);

    cout << "elementos a insertar listos" << endl;

    for(int i=0;i<numElements;i++){
        kll.add(elementos.at(i));
        //cout << "termino" << endl;
    }
    cout << endl;

    cout << "KLL1 Final:" << endl;
    //kll.print();

    // for(int i=0;i<numElements2;i++){
    //     kll2.add(elementos2.at(i));
    //     kll3.add(elementos2.at(i));
    //     //cout << "termino" << endl;
    // }
    // cout << endl;

    // cout << "KLL2 Final:" << endl;
    // kll2.print();

    // cout << endl <<  "KLL Merge:" << endl;
    // KLL merge = kll.kllMerge(kll2);
    // merge.print();
    // cout << endl;

    int numQuery = 10;
    vector<long> query;
    for(int i=0;i<numQuery;i++){
        query.push_back(elementos.at(rand()%elementos.size()));
    }
    sort(query.begin(),query.end());
    //sort(elementos.begin(),elementos.end());

    cout << endl << "RESULTADOS:" << endl;
    unsigned long long sumaErrorRank = 0;


    // print de rank y select
    double duracionTotal = 0; //not sure about the type
    for(int i=0;i<numQuery;i++){
        auto t_start = std::chrono::high_resolution_clock::now();

        long queryRank = kll.rank(query.at(i));
        auto t_end = std::chrono::high_resolution_clock::now();
        double elapsed_time_ms = std::chrono::duration<double, std::nano>(t_end-t_start).count();
        duracionTotal += elapsed_time_ms;
        long rankCorrecto = long(lower_bound(elementos.begin(), elementos.end(), query.at(i)) - elementos.begin());
        cout << "rank de " << query.at(i) << ": " << queryRank << "Tomado en un tiempo de " << endl;  
        cout << "rank correcto de " << query.at(i) << ": " 
             << rankCorrecto << endl;
        cout << "error de rank: " << abs(queryRank-rankCorrecto) << endl;
        sumaErrorRank+=abs(queryRank-rankCorrecto);
        cout << "select de " << queryRank << ": " << kll.select(queryRank) << endl; 
        cout << "select correcto de " << rankCorrecto << ": " 
             << elementos.at(rankCorrecto) << endl;
       
        for(int j=0;j<10;j++){
            cout << "- ";
        }
        cout << endl;
    }


    cout << "Error de rank promedio: " << sumaErrorRank/numQuery << " con un total de " 
         << elementos.size() << " elementos" << endl
         << "proporcion de error respecto al tamaño de elementos: " 
         << (double)sumaErrorRank*100.0/(double)numQuery/(double)elementos.size() << "%" << endl
         << "Tiempo promedio de rank: " << duracionTotal/numQuery << " [ns]" << endl;

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
	cout<<" rank 5 "<<kll.rank(5)<<"\n";
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
    srand(time(NULL));
    // lectura manual o por stream
    if(argc==1) lecturaManual();
    else if(argc>=2) lecturaDeStream(argc-1, argv);
	    /*
    if(argc >= 2){
	    int mean = atoi(argv[1]);
	    int stdev = atoi(argv[2]);
	    int len = atoi(argv[3]);
	    int scale = atoi(argv[4]);
	    cout<<" mean "<<mean<<" stdev "<<stdev<<" len "<<len<<" scale "<<scale<<endl;
	    normalElements(mean, stdev, len,scale);
    }
    */
    else{
        cout << "Numero de parametros incorrectos" << endl;
        return 0;
    }

    return 0;
}
