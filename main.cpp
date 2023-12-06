#include <sdsl/k2_tree.hpp>
#include <iostream>
#include <vector>

using namespace sdsl;
using namespace std;

typedef struct {
    int i;
    int j;
    int v;
} matrix;


vector<vector<int>> multiplicacion(vector<vector<int>> m1,vector<vector<int>> m2){
    vector<vector<int>> m3;
    int n = m1.size();
    for(int i=0; i<n; i++){
        vector<int> fila;
        for(int j=0; j<n; j++){
            int sum = 0;
            for(int k=0; k<n; k++){
                sum += m1[i][k]*m2[k][j];
            }
            fila.push_back(sum);
        }
        m3.push_back(fila);
    }
    return m3;
}

vector<vector<int>> submatrix(vector<vector<int>> m, int i, int j, int n){
    //dividir matriz en 4 submatrices
    vector<vector<int>> m2;
    for(int k=i; k<i+n; k++){
        vector<int> fila;
        for(int l=j; l<j+n; l++){
            fila.push_back(m[k][l]);
        }
        m2.push_back(fila);
    }
    return m2;
}

vector<vector<int>> addMatrices(const vector<vector<int>>& A, const vector<vector<int>>& B) {
    int n = A.size();
    vector<vector<int>> result(n, vector<int>(n, 0));

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            result[i][j] = A[i][j] + B[i][j];
        }
    }

    return result;
}

// Función para restar dos matrices
vector<vector<int>> subtractMatrices(const vector<vector<int>>& A, const vector<vector<int>>& B) {
    int n = A.size();
    vector<vector<int>> result(n, vector<int>(n, 0));

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            result[i][j] = A[i][j] - B[i][j];
        }
    }

    return result;
}

vector<vector<int>> zMultiplication(const vector<vector<int>>& A, const vector<vector<int>>& B){
    int n = A.size();

    // Caso base: matrices de 1x1
    if (n == 1) {
        vector<vector<int>> result(1, vector<int>(1, 0));
        result[0][0] = A[0][0] * B[0][0];
        return result;
    }

    // Dividir las matrices en 4 submatrices
    int mid = n / 2;
    vector<vector<int>> A11(mid, vector<int>(mid, 0));
    vector<vector<int>> A12(mid, vector<int>(mid, 0));
    vector<vector<int>> A21(mid, vector<int>(mid, 0));
    vector<vector<int>> A22(mid, vector<int>(mid, 0));

    vector<vector<int>> B11(mid, vector<int>(mid, 0));
    vector<vector<int>> B12(mid, vector<int>(mid, 0));
    vector<vector<int>> B21(mid, vector<int>(mid, 0));
    vector<vector<int>> B22(mid, vector<int>(mid, 0));

    for (int i = 0; i < mid; ++i) {
        for (int j = 0; j < mid; ++j) {
            A11[i][j] = A[i][j];
            A12[i][j] = A[i][j + mid];
            A21[i][j] = A[i + mid][j];
            A22[i][j] = A[i + mid][j + mid];

            B11[i][j] = B[i][j];
            B12[i][j] = B[i][j + mid];
            B21[i][j] = B[i + mid][j];
            B22[i][j] = B[i + mid][j + mid];
        }
    }

    vector<vector<int>> C11 = addMatrices(zMultiplication(A11, B11), zMultiplication(A12, B21));
    vector<vector<int>> C12 = addMatrices(zMultiplication(A11, B12), zMultiplication(A12, B22));
    vector<vector<int>> C21 = addMatrices(zMultiplication(A21, B11), zMultiplication(A22, B21));
    vector<vector<int>> C22 = addMatrices(zMultiplication(A21, B12), zMultiplication(A22, B22));

    vector<vector<int>> result(n, vector<int>(n, 0));
    for (int i = 0; i < mid; ++i) {
        for (int j = 0; j < mid; ++j) {
            result[i][j] = C11[i][j];
            result[i][j + mid] = C12[i][j];
            result[i + mid][j] = C21[i][j];
            result[i + mid][j + mid] = C22[i][j];
        }
    }

    return result;
}

// Función principal de multiplicación de matrices usando el algoritmo de dividir y conquistar
vector<vector<int>> strassen(const vector<vector<int>>& A, const vector<vector<int>>& B) {
    int n = A.size();

    // Caso base: matrices de 1x1
    if (n == 1) {
        vector<vector<int>> result(1, vector<int>(1, 0));
        result[0][0] = A[0][0] * B[0][0];
        return result;
    }

    // Dividir las matrices en 4 submatrices
    int mid = n / 2;
    vector<vector<int>> A11(mid, vector<int>(mid, 0));
    vector<vector<int>> A12(mid, vector<int>(mid, 0));
    vector<vector<int>> A21(mid, vector<int>(mid, 0));
    vector<vector<int>> A22(mid, vector<int>(mid, 0));

    vector<vector<int>> B11(mid, vector<int>(mid, 0));
    vector<vector<int>> B12(mid, vector<int>(mid, 0));
    vector<vector<int>> B21(mid, vector<int>(mid, 0));
    vector<vector<int>> B22(mid, vector<int>(mid, 0));

    for (int i = 0; i < mid; ++i) {
        for (int j = 0; j < mid; ++j) {
            A11[i][j] = A[i][j];
            A12[i][j] = A[i][j + mid];
            A21[i][j] = A[i + mid][j];
            A22[i][j] = A[i + mid][j + mid];

            B11[i][j] = B[i][j];
            B12[i][j] = B[i][j + mid];
            B21[i][j] = B[i + mid][j];
            B22[i][j] = B[i + mid][j + mid];
        }
    }

    // Calcula las siete multiplicaciones recursivas según el algoritmo de Strassen
    vector<vector<int>> P1 = strassen(A11, subtractMatrices(B12, B22));
    vector<vector<int>> P2 = strassen(addMatrices(A11, A12), B22);
    vector<vector<int>> P3 = strassen(addMatrices(A21, A22), B11);
    vector<vector<int>> P4 = strassen(A22, subtractMatrices(B21, B11));
    vector<vector<int>> P5 = strassen(addMatrices(A11, A22), addMatrices(B11, B22));
    vector<vector<int>> P6 = strassen(subtractMatrices(A12, A22), addMatrices(B21, B22));
    vector<vector<int>> P7 = strassen(subtractMatrices(A11, A21), addMatrices(B11, B12));

    // Calcula las cuatro submatrices resultantes
    vector<vector<int>> C11 = subtractMatrices(addMatrices(addMatrices(P5, P4), P6), P2);
    vector<vector<int>> C12 = addMatrices(P1, P2);
    vector<vector<int>> C21 = addMatrices(P3, P4);
    vector<vector<int>> C22 = subtractMatrices(subtractMatrices(addMatrices(P5, P1), P3), P7);

    // Combina las cuatro submatrices resultantes en una sola matriz
    vector<vector<int>> result(n, vector<int>(n, 0));
    for (int i = 0; i < mid; ++i) {
        for (int j = 0; j < mid; ++j) {
            result[i][j] = C11[i][j];
            result[i][j + mid] = C12[i][j];
            result[i + mid][j] = C21[i][j];
            result[i + mid][j + mid] = C22[i][j];
        }
    }

    return result;
}

int zOrder(int i, int j){
    // n: Numero maximo de bits a procesar
    int n = max(i,j);

    int z = 0;
    int bit = 0;
    
    while(n>0){
        // Aqui se recorren los bits de las posiciones i y j
        int ib = (i>>bit)&1;
        int jb = (j>>bit)&1;
        // Operacion para encontrar el bit de la secuencia zOrder de i y j
        z |= (ib << (2*bit)) | (jb << (2*bit+1));
        // Se desplaza n para procesar el siguiente bit
        n >>= 1;
        bit++;
    }
    return z;
}

bool comparar(matrix m1, matrix m2){
    return zOrder(m1.i,m1.j) < zOrder(m2.i,m2.j);
}

int* new_array(matrix* m, int n){
    int *arr = new int[n];

    sort(m, m+n, comparar);

    for(int i=0; i<n; i++){
        arr[i] = m[i].v;
    }

    return arr;
}



int main(){
    int l = 0;
    int n = 6;
    vector<vector<int>> matriz {
        {5,2,3,4},
        {2,3,l,4},
        {1,3,2,l},
        {2,1,2,l}
    };

    matrix m[6];
    m[0].i = 0;
    m[0].j = 0;
    m[0].v = 1;

    m[1].i = 1;
    m[1].j = 1;
    m[1].v = 2;

    m[2].i = 2;
    m[2].j = 3;
    m[2].v = 4;

    m[3].i = 3;
    m[3].j = 0;
    m[3].v = 3;

    m[4].i = 3;
    m[4].j = 1;
    m[4].v = 1;

    m[5].i = 1;
    m[5].j = 2;
    m[5].v = 8;
    
    int *arr = new_array(m, n);

    for(int i=0; i<n; i++){
        cout << arr[i] << endl;
    }

    //k2_tree<2> arbol(matriz);
    //cout << "[";
    //for(int i=0; i<arbol.get_l().size()-1; i++){
    //    cout << arbol.get_l()[i] << ",";
    //}
    //cout << arbol.get_l()[arbol.get_l().size()-1] << "]" <<endl;
    delete[] arr;
    return 0;

}