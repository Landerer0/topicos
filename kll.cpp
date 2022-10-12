#include <iostream>
#include <cmath>

#include "kll.hpp"

using namespace std;

KLL::KLL(int numElements, int numK, double numC){
    // usar propiedad de log: log_b(a) = log(a)/log(b) 
    n = numElements;
    c = numC;
    k = numK;
    numArreglos = ceil(log(k)/log(c));

    // inicializar los vectores de tam k*c^lvl
    for(int i=0;i<numArreglos;i++){
        // el valor por defecto es -1, que indica "vacio"
        vector<long> vectorAtLvlI(-1,k*pow(c,i));
        pair<vector<long>,long> toInsert;
        toInsert.first=vectorAtLvlI;
        toInsert.second=0; // representa el num de elementos ocupados en el arreglo
        sketch.push_back(toInsert);
    }
}

KLL::~KLL(){
    
}

void KLL::insertElement(long nivel,long &element){
    vector<long> vectorToInsert = sketch.at(nivel).first;
    long posAInsertar = sketch.at(nivel).second;
    vectorToInsert.at(posAInsertar) = element;
    sketch.at(nivel).second++;
}

// en las notas de MRL aqui es donde se realiza el sort
void KLL::compaction(){
    long nivel = 0;
    long numElementosOcupados = sketch.at(nivel).second;
    long numElementosTotales = sketch.at(nivel).first.size();
    unsigned char elementosPares = 0;
    while(numElementosOcupados==numElementosTotales){
        if(rand()%2==0) elementosPares = 0; // se mantienen los elementos pares
        else elementosPares = 1; // se mantienen los elementos impares
        
        //!IMPORTANTE
        // AGREGAR EL SORT DEL ARREGLO AQUI

        // insertar la mitad de elementos (los pares o impares) en el sgte. nivel
        for(int i=elementosPares;i<numElementosTotales;i+=2){
            insertElement(nivel+1,sketch.at(nivel).first.at(i));
        }

        sketch.at(nivel).second = 0; // se "vacia el arreglo"
        // ¿ES POSIBLE QUE EN UN FUTURO SEA NECESARIO HACER LOS VALORES DEL NIVEL -1?

        nivel++;
        numElementosOcupados = sketch.at(nivel).second;
        numElementosTotales = sketch.at(nivel).first.size();
    }
}

// discutir si conviene mantener el valor de k*c^i almacenado en memoria o si da "lo mismo" computarlo
void KLL::update(long &element){
    insertElement(0,element);
    compaction();

    return;
}

unsigned long KLL::rank(long &element){
    unsigned long rank = 0;

    for(int nivel=0;nivel< numArreglos;nivel++){ // por cada arreglo
        for(int i=0;i<sketch.at(nivel).second;i++){ // por cada item dentro del arreglo
            if(element < sketch.at(nivel).first.at(i)){ // comparo el num elementos menores
                rank += pow(2,nivel); // en caso de que existan menores sumar acordemente segun el nivel
            }
        }
    }

    return rank;
}

long KLL::select(long &rank){

}

long KLL::quantile(double &q){
    // REVISAR SI ES CEIL O FLOOR, POR EL MOMENTO ME HACE SENTIDO FLOOR
    return select(floor(q*n));
}

