#include <iostream>
#include <cmath>
#include <algorithm>
#include <stdio.h>
#include <string.h>

#include "kll.hpp"

using namespace std;
KLL::KLL(unsigned long numElements, double epsilonParam, double numC){
    // usar propiedad de log: log_b(a) = log(a)/log(b) 
    n = numElements;
    c = numC;
    epsilon = epsilonParam;
    k = ceil(1.0/epsilon* ceil(log2(epsilon*n))) +1;
    //k = 200;
    cout<<" k "<<k<<endl;
    if(k%2==1) k+=1; // k sera par

    cerr << "Num Elementos: " << numElements << endl;
    cerr << "K: " << k << endl;
    cerr << "c: " << c << endl;
    cerr << endl;
    numArreglos = ceil(log2(n/k*2));
    numElementosRevisados = 0;
    numArreglosOcupados = 0;
    unsigned long long espacioOcupado = 0;

    vector<long> sizeTemp;
    for(int i=0;i<numArreglos;i++){
        unsigned long long cantElementos = max((int)(k*pow(c,i)),(int)2);
        sizeTemp.push_back(max((int)(k*pow(c,i)),(int)2));
    }

    // inicializar los vectores de tam k*c^lvl
    for(int i=0;i<numArreglos;i++){
        // el valor por defecto es -1, que indica "vacio"
        unsigned long long cantElementos = sizeTemp.at(numArreglos-1-i);
        espacioOcupado += cantElementos;
        cerr << "Cantidad Elementos arreglo " << i <<" :" << cantElementos<< endl;
        long valorElemento = -2;
        vector<long> vectorAtLvlI(cantElementos,valorElemento); 
        pair<vector<long>,long> toInsert;
        toInsert.first=vectorAtLvlI;
        toInsert.second=0; // representa el num de elementos ocupados en el arreglo
        sketch.push_back(toInsert);
        sorted.push_back(false); // indicamos que no se encuentra sorteado el arreglo
    }

    cout << "Cant elementos en Sketch: " << espacioOcupado << " Espacio ocupado: " << espacioOcupado*sizeof(long) << " bytes" <<  endl;
    //print();
}

KLL::~KLL(){
    
}

void KLL::insertElement(long nivel,long &element){
    //cout << "insert " << element << endl;
    long posAInsertar = sketch.at(nivel).second;
    sketch.at(nivel).first.at(posAInsertar) = element;
    sketch.at(nivel).second++;
    sorted.at(nivel)=false;
}

void KLL::insertCompactionElement(long nivel,long &element, bool updating){
    // AVANCE2
    // if(updating) cout << "inicio insert " << element << endl;

    long posAInsertar = sketch.at(nivel).second;
    if(posAInsertar==sketch.at(nivel).first.size()){
        if(nivel==numArreglos && updating){
            //Agregar nuevo compactor de tam k
            long numElementosInsert = 2;
            vector<long> toInsert(numElementosInsert,-2);
            sketch.push_back(make_pair(toInsert,0));
            numArreglos++;
        }
        compaction(nivel,updating);
        posAInsertar = sketch.at(nivel).second;
    }
    
    sketch.at(nivel).first.at(posAInsertar) = element;
    sketch.at(nivel).second++;

    // AVANCE2
    // if(updating){
    //     cout << endl << "Insert element: " << element << endl;
    //     print();
    // }
}

void KLL::compaction(long nivel, bool updating){
    //cout << "Compaction: " << nivel <<  endl;
    //print();

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
        
        sketch.at(nivel).second = 0;
        // insertar la mitad de elementos (los pares o impares) en el sgte. nivel
        for(int i=elementosPares;i<numElementosTotales;i+=2){
            insertCompactionElement(nivel+1,sketch.at(nivel).first.at(i),updating);
        }

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

void KLL::add(long &element){
    numElementosRevisados++; // para metodo quantile
    insertElement(0,element);
    compaction((long) 0, false);

    return;
}

void KLL::addv(long element){
    numElementosRevisados++; // para metodo quantile
    insertElement(0,element);
    compaction((long) 0, false);

    return;
}

unsigned long KLL::rank(long element){
    unsigned long rank = 0;

    vector<long> actual;

    for(int nivel=0;nivel< numArreglos;nivel++){ // por cada arreglo
        actual = sketch.at(nivel).first;
        //if(!sorted.at(nivel)){
            sort(actual.begin(),actual.end());
	    sketch.at(nivel).first = actual;
         //   sorted.at(nivel) = true;
        //} 
        for(int i=0;i<actual.size();i++){ // por cada item dentro del arreglo
            if(actual.at(i) < 0) continue;
            if(element >= actual.at(i)){ // comparo el num elementos menores
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
        //for(int j=0;j<sketch.at(i).second;j++){
        for(int j=0;j<sketch.at(i).first.size();j++){
	    if(sketch.at(i).first.at(j) < 0)continue;
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
	if(elementos.at(i).first < 0)continue;
        rankActual+=elementos.at(i).second;
        if(rank<=rankActual) return elementos.at(i).first;
    }

    return elementos.at(elementos.size()-1).first;
}

long KLL::quantile(double q){
    q = q/100.0;
    cout<<" q "<<q<<" numElementosRevisados "<<numElementosRevisados<<"\n";
    return select(floor(q*numElementosRevisados));
}

long KLL::height(){
    return numArreglos;
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

pair<vector<long>, long> KLL::sketchAtLevel(long nivel){
    if(nivel<0||nivel>=numArreglos) return make_pair(vector<long>(),-1);
    return sketch.at(nivel);
}

bool KLL::sortedAtLevel(long nivel){
    if(nivel<0||nivel>=numArreglos) return false;
    return sorted.at(nivel);
}

void KLL::update(KLL kll2){
    
    // kll2.setSeconds(vector<long>());
    // para cada nivel
    for(int nivel=0; nivel<kll2.height();nivel++){
        pair<vector<long>,long> kll2pair = kll2.sketchAtLevel(nivel);
        // cout << "nivel " << nivel << endl;
        // setSeconds(vector<long>());
        // cerr << "Elementos1 en nivel " << nivel << ": " << sketch.at(nivel).second << endl;
        // cerr << "Elementos2 en nivel " << nivel << ": " << kll2pair.second << endl;
        // para cada elemento dentro del compactor
        for(int i=0; i < kll2pair.second;i++){
            if(kll2pair.first.at(i)<0) continue;
            insertCompactionElement(nivel,kll2pair.first.at(i),true);
        }
    }

    return;
}


void KLL::setSeconds(vector<long> seconds){
    for(int i=0;i<numArreglos;i++){
        //sketch.at(i).second = seconds.at(i);
        cerr << sketch.at(i).second << endl;
    }
    
    return;
}

KLL KLL::kllMerge(KLL &kll2){ 
    // Creamos una copia del kll a devolver
    if(height() < kll2.height()){
        KLL kllCopy2 = kll2.copy();
        kllCopy2.update(*this);
        return kllCopy2;
    } 
    else{
        KLL kllCopy1 = copy();
        kllCopy1.update(kll2);
        return kllCopy1;
    } 

    
}



KLL::KLL(unsigned long numElements, double epsilonParam, double numC, unsigned long long elementosRevisados, KLL* toCopy){
// usar propiedad de log: log_b(a) = log(a)/log(b) 
    n = numElements;
    c = numC;
    epsilon = epsilonParam;
    numElementosRevisados = elementosRevisados;

    k = ceil(1.0/epsilon* ceil(log2(epsilon*n))) +1;
    if(k%2==1) k+=1; // k sera par

    cerr << "Num Elementos: " << numElements << endl;
    cerr << "K: " << k << endl;
    cerr << "c: " << c << endl;
    cerr << endl;
    numArreglos = ceil(log2(n/k*2));

    // copiar los valores del kll a copiar
    for(int i=0;i<numArreglos;i++){
        sketch.push_back(toCopy->sketchAtLevel(i));
        sorted.push_back(toCopy->sortedAtLevel(i));
    }
}

KLL KLL::copy(){
    KLL copia(n,epsilon,c,numElementosRevisados,this);
    return copia;
}


