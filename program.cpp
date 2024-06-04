#include "main.cpp"
#include "k2_tree.hpp"
#include "functions.cpp"
#include <iostream>
#include <fstream>


using namespace std;

typedef uint64_t idx_type;

string help(){
    string v = "Para usar este programa tiene 4 opciones:\n1. Sumar 2 matrices\n2. Multiplicar 2 matrices\n3. Este mensaje\n 4. Salir\n";
    return v;
}

int cmd(){
    cout << "Bienvenido a este programa de algebra basica de matrices:\n Para que este programa funcione, debe tener 2 archivos de texto con las matrices que desea operar con el nombre 'matrix1.txt' y 'matrix2.txt'\nIngrese una opcion (para ver los comandos ingrese opcion 4)" << endl;
    int v;
    cin >> v;
    if(v == 1){
        return 1;
    }
    if(v == 2){
        return 2;
    }
    if(v == 3){
        return 3;
    }
    if(v == 4){
        return 4;
    }
    else{
        return 0;
    }
}

int main(){
    //reads an archive and creates a k2 tree
    k2_tree<2> t1;

    ifstream file("matrix1.txt");
    int n;
    file >> n;
    vector<tuple<idx_type,idx_type,int>> values;
    for(int i=0; i<n; i++){
        //split string into a tuple of values
        int a,b,c;
        file >> a >> b >> c;
        values.push_back(make_tuple(a,b,c));
    }
    t1 = k2_tree<2>(values,n);

    file.close();

    //reads an archive and creates a k2 tree
    
    k2_tree<2> t2;

    ifstream file2("matrix2.txt");

    file2 >> n;

    vector<tuple<idx_type,idx_type,int>> values2;
    for(int i=0; i<n; i++){
        //split string into a tuple of values
        int a,b,c;
        file2 >> a >> b >> c;
        values2.push_back(make_tuple(a,b,c));
    }
    t2 = k2_tree<2>(values2,n);

    file2.close();

    while(true){
        int opt = cmd();
        if(opt == 1){
            k2_tree<2> t3 = sumaImpl(t1,t2);
        }
        if(opt == 2){
            k2_tree<2> t3 = multiplicarImpl(t1,t2);
        }
        if(opt == 3){
            cout << help() << endl;
        }
        if(opt == 4){
            break;
        }
        else{
            cout << "Opcion invalida, por favor ingrese una opcion valida" << endl;
        }
    }

}