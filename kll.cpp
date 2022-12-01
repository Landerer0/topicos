#include <iostream>
#include <cmath>
#include <algorithm>

#include "kll.hpp"

using namespace std;
KLL::KLL(unsigned long numElements, double epsilon, double numC){
    // usar propiedad de log: log_b(a) = log(a)/log(b) 
    n = numElements;
    c = numC;
    k = ceil(1.0/epsilon* ceil(log2(epsilon*n))) +1;
    if(k%2==1) k+=1;
    numArreglos = ceil(log2(n/k));
    numElementosRevisados = 0;
    unsigned long long espacioOcupado = 0;

    // inicializar los vectores de tam k*c^lvl
    for(int i=0;i<numArreglos;i++){
        // el valor por defecto es -1, que indica "vacio"
        unsigned long long cantElementos = k*pow(c,i);
        espacioOcupado += cantElementos;
        cerr << "Cantidad Elementos arreglo " << i <<" :" << cantElementos<< endl;
        long valorElemento = -2;
        vector<long> vectorAtLvlI(cantElementos,valorElemento); 
        pair<vector<long>,long> toInsert;
        toInsert.first=vectorAtLvlI;
        toInsert.second=0; // representa el num de elementos ocupados en el arreglo
        sketch.push_back(toInsert);
    }

    cout << "Espacio ocupado: " << espacioOcupado*sizeof(long) << " bytes" <<  endl;
}

KLL::~KLL(){
    
}

void KLL::insertElement(long nivel,long &element){
    long posAInsertar = sketch.at(nivel).second;
    sketch.at(nivel).first.at(posAInsertar) = element;
    sketch.at(nivel).second++;
}

void KLL::insertCompactionElement(long nivel,long &element){
    long posAInsertar = sketch.at(nivel).second;
    if(posAInsertar==sketch.at(nivel).first.size()){
        compaction(nivel);
        posAInsertar = sketch.at(nivel).second;
    }
    sketch.at(nivel).first.at(posAInsertar) = element;
    sketch.at(nivel).second++;
}

void KLL::compaction(long nivel){
    long numElementosOcupados = sketch.at(nivel).second;
    long numElementosTotales = sketch.at(nivel).first.size();
    unsigned char elementosPares = 0;
    if(numElementosOcupados==numElementosTotales){
        if(nivel+1==numArreglos) return;
        if(debug) cerr << endl << "compaction " << nivel+1 << endl;
        if(rand()%2==0) elementosPares = 0; // se mantienen los elementos pares
        else elementosPares = 1; // se mantienen los elementos impares
        
        // sort del arreglo
        sort(sketch.at(nivel).first.begin(), sketch.at(nivel).first.end());
        
        if(debug) print();

        // insertar la mitad de elementos (los pares o impares) en el sgte. nivel
        for(int i=elementosPares;i<numElementosTotales;i+=2){
            insertCompactionElement(nivel+1,sketch.at(nivel).first.at(i));
        }


        sketch.at(nivel).second = 0; // se "vacia el arreglo"

        long nullElement = -1;
        for(int i=0;i<numElementosTotales;i++){
            sketch.at(nivel).first.at(i) = nullElement;
        }

        if(debug) cout << "Fin de compaction " << nivel+1 << endl;
        if(debug) print();

        //nivel++;
        //numElementosOcupados = sketch.at(nivel).second;
        //numElementosTotales = sketch.at(nivel).first.size();
    }
}

// discutir si conviene mantener el valor de k*c^i almacenado en memoria o si da "lo mismo" computarlo
void KLL::update(long &element){
    numElementosRevisados++; // para metodo quantile
    insertElement(0,element);
    compaction((long) 0);

    return;
}

unsigned long KLL::rank(long element){
    unsigned long rank = 0;

    // falta decidir de si realizar la operacion hasta el siguiente elemento mayor o no
    for(int nivel=0;nivel< numArreglos;nivel++){ // por cada arreglo
        for(int i=0;i<sketch.at(nivel).second;i++){ // por cada item dentro del arreglo
            if(element >= sketch.at(nivel).first.at(i)){ // comparo el num elementos menores
                rank += pow(2,nivel); // en caso de que existan menores sumar acordemente segun el nivel
            }
        }
    }

    return rank;
}

long KLL::select(long rank){
    vector<pair<long,unsigned long long>> elementos; // par(elemento,peso) 
    
    // llenar el vector con los elementos del sketch 
    for(int i=0;i<numArreglos;i++){
        unsigned long long peso = pow(2,i);
        for(int j=0;j<sketch.at(i).second;j++){
            pair<long, unsigned long long> toInsert;
            toInsert.first = sketch.at(i).first.at(j);
            toInsert.second = peso;
            elementos.push_back(toInsert);
        }
    }

    // realizar el sort
    sort(elementos.begin(),elementos.end());

    long rankActual = 0;
    //retornar segun la suma de elementos
    for(int i=0;i<elementos.size();i++){
        rankActual+=elementos.at(i).second;
        if(rank<=rankActual) return elementos.at(i).first;
    }

    return -1;
}

long KLL::quantile(double q){
    // REVISAR SI ES CEIL O FLOOR, POR EL MOMENTO ME HACE SENTIDO FLOOR
    return select(floor(q*numElementosRevisados));
}

void KLL::print(){
    for(int i=0; i<numArreglos;i++){
        cout << "Nivel " << i+1 << ":" << endl;
        vector<long> nivelI = sketch.at(i).first;
        for(int j=0;j<nivelI.size();j++){
            cout << nivelI.at(j) << " ";
        }
        cout << endl;
    }
}
