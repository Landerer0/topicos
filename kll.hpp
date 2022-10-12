#include <iostream>
#include <vector>

using namespace std;

class KLL{
    public:
        KLL(unsigned long,unsigned long long,double);
        ~KLL();
        void update(long &element); // agregar element al sketch
        // Operaciones asociadas al problema
        unsigned long rank(long &element); // indica el rank del elemento proporcionado
        long select(long &rank); // retorna el elemento cuyo rank es el indicado
        long quantile(double &q); // retorna elemento encontrado en el quantil q

    private:
        vector<pair<vector<long>, long> > sketch; // arreglo de arreglos con tamaño decreciente
                                                  // long es para mantener el num de elementos 
                                                  // ocupados en el arreglo de dicho nivel
        unsigned int numArreglos;

        // Operaciones
        void insertElement(long nivel,long &element);
        void compaction();

        // variables k y c son ctes. entregadas por el usuario, c esta en rango [0.1,1]
        unsigned long long k; // capacidad del arreglo de mayor tamaño
        double c; // factor por el que cada arreglo va disminuyendo al aumentar la altura del arreglo
        unsigned long long n;

        void compaction(int level);
};