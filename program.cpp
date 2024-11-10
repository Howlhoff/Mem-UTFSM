#include "memoria.cpp"
#include "k2_tree.hpp"
#include <iostream>
#include <fstream>

using namespace std;

typedef uint64_t idx_type;

string help(){
    string v = "Para usar este programa tiene 3 opciones:\n\n1. Sumar 2 matrices\n2. Este mensaje\n3. Salir\n\nEste programa requiere que usted disponga de 2 archivos de texto llamados 'matrix1.txt' y 'matrix2.txt' con las matrices que desea operar en el siguiente formato.\n\nPor cada linea deben de haber 3 valores indicando la fila de la matriz, la columna y el valor de la matriz en esa posicion\n\nEjemplo:\n\n0 0 1\n0 1 2\n1 0 3\n1 1 4\n\nNota: Solo se deben de ingresar los valores no nulos de la matriz\n";
    return v;
}

int cmd(){
    cout << "Ingrese una opcion (para ver los comandos ingrese opcion 2)" << endl;
    cout << "> ";
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

void program(){
    ifstream file1("matrix1.txt");
    vector<tuple<idx_type,idx_type,int>> values1;
    string line1;
    while (getline(file1, line1)) {
        // Split line by ' '
        vector<string> v1;
        string s1 = "";
        for (int i = 0; i < line1.size(); i++) {
            if (line1[i] == ' ') {
                v1.push_back(s1);
                s1 = "";
            } else {
                s1 += line1[i];
            }
        }
        if (s1.size() > 0) {
            v1.push_back(s1);
        }
        if (v1.size() < 3) {
            continue;
        }
        // Create matrix from v1
        idx_type i = static_cast<idx_type>(stoi(v1[0])), j = static_cast<idx_type>(stoi(v1[1]));
        int val = stoi(v1[2]);
        tuple<idx_type, idx_type, int> t(i, j, val);
        values1.push_back(t);
    }
    k2_tree<2> t1(values1);

    file1.close();

    //reads an archive and creates a k2 tree

    ifstream file2("matrix2.txt");

    vector<tuple<idx_type,idx_type,int>> values2;
    string line2;
    while (getline(file2, line2)) {
        // Split line by ' '
        vector<string> v2;
        string s2 = "";
        for (int i = 0; i < line2.size(); i++) {
            if (line2[i] == ' ') {
                v2.push_back(s2);
                s2 = "";
            } else {
                s2 += line2[i];
            }
        }
        if (s2.size() > 0) {
            v2.push_back(s2);
        }
        if (v2.size() < 3) {
            continue;
        }
        // Create matrix from v2
        idx_type i = static_cast<idx_type>(stoi(v2[0])), j = static_cast<idx_type>(stoi(v2[1]));
        int val = stoi(v2[2]);
        tuple<idx_type, idx_type, int> t(i, j, val);
        values2.push_back(t);
    }

    k2_tree<2> t2(values2);

    cout << "Bienvenido a este programa de algebra basica de matrices:\nPara que este programa funcione, debe tener 2 archivos de texto con las matrices que desea operar con el nombre 'matrix1.txt' y 'matrix2.txt'\n" << endl;

    while(true){
        int opt = cmd();
        if(opt == 1){
            sumaImpl(t1,t2);
        }
        //if(opt == 2){
        //    multiplicarImpl(t1,t2);
        //}
        else if(opt == 2){
            cout << help() << endl;
        }
        else if(opt == 3){
            cout << "Gracias por usar este programa!! Que tenga buen dia" << endl;
            break;
        }
        else{
            cout << "Opcion invalida, por favor ingrese una opcion valida" << endl;
        }
    }
}

int main(){
    //reads an archive and creates a k2 tree

    program();
    
    return 0;

}