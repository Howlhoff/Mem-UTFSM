#include "k2_tree.hpp"
#include <iostream>
#include <vector>
#include "functions.cpp"
#include <tuple>
#include <ctime>

using namespace std;

typedef uint64_t idx_type;

bool is_leaf(int actual, k2_tree<2>& t){
    if(actual >= t.get_t().size()){
        return true;
    }
    return false;
}

typedef struct{
    int i;
    int j;
} position;

// uint64_t parametros de entrada y var loc

position get_reversed_z_order(idx_type z){
    idx_type i = 0;
    idx_type j = 0;
    idx_type bit = 0;
    while(z>0){
        i |= (z & 1) << bit;
        z >>= 1;
        j |= (z & 1) << bit;
        z >>= 1;
        bit++;
    }
    position p;
    p.i = i;
    p.j = j;
    return p;
}


// suma de k2 trees usando recursion
// repensar en manera de hacerlo
// problema con que retornar
// agregar 2 flags por arbol
// agregar parametro de i y j para saber en que posicion de matriz estoy
// aplicar rank sobre el bitvector de las hojas hasta la pos correspondiente

void suma(vector<tuple<idx_type,idx_type,int>> &ret , k2_tree<2>& t1, k2_tree<2>& t2, int pos1, int pos2, bool flag1, bool flag2, int i, int j){
    // uses recursion to sum the k2 trees
    //cout << "estoy en pos1=" << pos1 << "\nestoy en pos2=" << pos2 << endl;
    if(is_leaf(pos1,t1)&&is_leaf(pos2,t2)){
        flag1 = true;
        flag2 = true;
        position p1 = get_reversed_z_order(pos1-t1.get_t().size());
        i = p1.i;
        j = p1.j;
        if(flag1 && flag2){
            int d1 = t1.get_k_l_rank(pos1-t1.get_t().size()), d2 = t2.get_k_l_rank(pos2-t2.get_t().size());
            int add = t1.get_v(d1) + t2.get_v(d2);
            tuple<idx_type,idx_type,int> r(i,j,add);
            ret.push_back(r);
        }
        else if(flag1 && !flag2){
            int d1 = t1.get_k_l_rank(pos1-t1.get_t().size());
            int add = t1.get_v(d1);
            tuple<idx_type,idx_type,int> r(i,j,add);
            ret.push_back(r);
        }
        else if(!flag1 && flag2){
            int d2 = t2.get_k_l_rank(pos2-t2.get_t().size());
            int add = t2.get_v(d2);
            tuple<idx_type,idx_type,int> r(i,j,add);
            ret.push_back(r);
        }
    }
    else{
        //suma de los elementos de los nodos
        flag1 = false;
        flag2 = false;
        int t1c1, t1c2, t1c3, t1c4;
        int t2c1, t2c2, t2c3, t2c4;
        t1c1 = t1.get_child(pos1,0);
        t1c2 = t1.get_child(pos1,1);
        t1c3 = t1.get_child(pos1,2);
        t1c4 = t1.get_child(pos1,3);
        t2c1 = t2.get_child(pos2,0);
        t2c2 = t2.get_child(pos2,1);
        t2c3 = t2.get_child(pos2,2);
        t2c4 = t2.get_child(pos2,3);
        if (t1c1 != -1 && t2c1 != -1) suma(ret, t1, t2, t1c1, t2c1,flag1,flag2,i,j);
        if (t1c2 != -1 && t2c2 != -1) suma(ret, t1, t2, t1c2, t2c2,flag1,flag2,i,j);
        if (t1c3 != -1 && t2c3 != -1) suma(ret, t1, t2, t1c3, t2c3,flag1,flag2,i,j);
        if (t1c4 != -1 && t2c4 != -1) suma(ret, t1, t2, t1c4, t2c4,flag1,flag2,i,j);
    
    }
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

void join(vector<tuple<idx_type,idx_type,int>> &ret, vector<tuple<idx_type,idx_type,int>> &ret1,vector<tuple<idx_type,idx_type,int>> &ret2,vector<tuple<idx_type,idx_type,int>> &ret3,vector<tuple<idx_type,idx_type,int>> &ret4){
    for(int i=0; i<ret1.size(); i++){
        ret.push_back(ret1[i]);
    }
    for(int i=0; i<ret2.size(); i++){
        ret.push_back(ret2[i]);
    }
    for(int i=0; i<ret3.size(); i++){
        ret.push_back(ret3[i]);
    }
    for(int i=0; i<ret4.size(); i++){
        ret.push_back(ret4[i]);
    }
}

// i es fila, j es columna

void multiplicar(vector<tuple<idx_type,idx_type,int>> &ret, k2_tree<2>& t1, k2_tree<2>& t2, int pos1, int pos2,bool flag1, bool flag2){
    vector<tuple<idx_type,idx_type,int>> c1, c2, c3, c4, c5, c6, c7, c8, ret1, ret2, ret3, ret4;
    if(is_leaf(pos1,t1)&&is_leaf(pos2,t2)){
        int i1, i2;
        i1 = t1.get_index();
        i2 = t2.get_index();
        position k1, k2;
        k1 = get_reversed_z_order(i1);
        k2 = get_reversed_z_order(i2);
        if(k1.i == k2.j){
            int add = t1.get_v(i1)*t2.get_v(i2);//comprobar
            tuple<idx_type,idx_type,int> r(k1.i,k2.j,add);
            ret.push_back(r);
        }
        else{
            int add = 0;
        }
    }
    else{
            multiplicar(c1,t1,t2,t1.get_child(pos1,0),t2.get_child(pos2,0),flag1,flag2);
            multiplicar(c2,t1,t2,t1.get_child(pos1,1),t2.get_child(pos2,2),flag1,flag2);
            multiplicar(c3,t1,t2,t1.get_child(pos1,0),t2.get_child(pos2,1),flag1,flag2);
            multiplicar(c4,t1,t2,t1.get_child(pos1,1),t2.get_child(pos2,3),flag1,flag2);
            multiplicar(c5,t1,t2,t1.get_child(pos1,2),t2.get_child(pos2,0),flag1,flag2);
            multiplicar(c6,t1,t2,t1.get_child(pos1,3),t2.get_child(pos2,2),flag1,flag2);
            multiplicar(c7,t1,t2,t1.get_child(pos1,2),t2.get_child(pos2,1),flag1,flag2);
            multiplicar(c8,t1,t2,t1.get_child(pos1,3),t2.get_child(pos2,3),flag1,flag2);
            k2_tree<2> t3(c1,c1.size());
            k2_tree<2> t4(c2,c2.size());
            k2_tree<2> t5(c3,c3.size());
            k2_tree<2> t6(c4,c4.size());
            k2_tree<2> t7(c5,c5.size());
            k2_tree<2> t8(c6,c6.size());
            k2_tree<2> t9(c7,c7.size());
            k2_tree<2> t10(c8,c8.size());
            suma(ret1,t3,t4,0,0,false,false,0,0);
            suma(ret2,t5,t6,0,0,false,false,0,0);
            suma(ret3,t7,t8,0,0,false,false,0,0);
            suma(ret4,t9,t10,0,0,false,false,0,0);
            join(ret,ret1,ret2,ret3,ret4);
        }
    
}

unsigned int k0, k1;

k2_tree<2> sumaImpl(k2_tree<2>& t1, k2_tree<2>& t2){
    vector<tuple<idx_type,idx_type,int>> ret;
    k0 = clock();
    suma(ret,t1,t2,0,0,false,false,0,0);
    k2_tree<2> t3(ret,ret.size());
    k1 = clock();
    return t3;
}

k2_tree<2> multiplicarImpl(k2_tree<2>& t1, k2_tree<2>& t2){
    vector<tuple<idx_type,idx_type,int>> ret;
    k0 = clock();
    multiplicar(ret,t1,t2,0,0,false,false);
    k2_tree<2> t3(ret,ret.size());
    k1 = clock();
    return t3;
}

double _time = (double(k1-k0)/CLOCKS_PER_SEC);

void print_bit_vector(const bit_vector &bv) {
    for (const auto &bit : bv) {
        std::cout << bit;
    }
    std::cout << std::endl;
}


int main(){
    int n = 6;
    matrix_pos m[6];
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

    matrix_pos k[5];
    k[0].i = 2;
    k[0].j = 0;
    k[0].v = 2;

    k[1].i = 3;
    k[1].j = 0;
    k[1].v = 1;

    k[2].i = 3;
    k[2].j = 1;
    k[2].v = 1;

    k[3].i = 0;
    k[3].j = 2;
    k[3].v = 2;

    k[4].i = 3;
    k[4].j = 3;
    k[4].v = 1;
    
    vector<tuple<idx_type,idx_type,int>> m1;
    for(int i=0; i<n; i++){
        tuple<idx_type,idx_type,int> t = make_tuple(m[i].i,m[i].j,m[i].v);
        m1.push_back(t);
    }
    vector<tuple<idx_type,idx_type,int>> m2;
    for(int i=0; i<5; i++){
        tuple<idx_type,idx_type,int> t = make_tuple(k[i].i,k[i].j,k[i].v);
        m2.push_back(t);
    }

    k2_tree<2> arbol1(m1,n);
    k2_tree<2> arbol2(m2,n);

    cout << "test" << endl;

    vector<tuple<idx_type,idx_type,int>> ret;

    print_bit_vector(arbol1.get_t());
    print_bit_vector(arbol1.get_l());
    print_bit_vector(arbol2.get_t());
    print_bit_vector(arbol2.get_l());

    suma(ret,arbol1,arbol2,0,0,false,false,0,0);

    k2_tree<2> arbol3(ret,ret.size());

    print_bit_vector(arbol3.get_t());
    print_bit_vector(arbol3.get_l());

    return 0;

}