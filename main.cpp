#include "k2_tree.hpp"
#include <iostream>
#include <vector>
#include "functions.cpp"
#include <tuple>
#include <ctime>

using namespace std;

typedef uint64_t idx_type;

bool is_leaf_1(int actual, k2_tree<2>& t){
    int leaf_index = actual-t.get_t().size();
    if(leaf_index < t.get_l().size() && t.get_l()[leaf_index] == 1){
        return true;
    }
    else{
        return false;
    }
}

bool is_leaf(int actual, k2_tree<2>& t){
    int leaf_index = actual-t.get_t().size();
    if(leaf_index < t.get_l().size()){
        return true;
    }
    else{
        return false;
    }
}

bool is_leaf_0(int actual, k2_tree<2>& t){
    int leaf_index = actual-t.get_t().size();
    if(leaf_index < t.get_l().size() && t.get_l()[leaf_index] == 0){
        return true;
    }
    else{
        return false;
    }
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

position shifting(idx_type i, idx_type j, int k){
    position p;
    switch (k)
    {
    case 0:
        p.i = (i << 1) | 0ULL;
        p.j = (j << 1) | 0ULL;
        break;
    case 1:
        p.i = (i << 1) | 0ULL;
        p.j = (j << 1) | 1ULL;
        break;
    case 2:
        p.i = (i << 1) | 1ULL;
        p.j = (j << 1) | 0ULL;
        break;
    case 3:
        p.i = (i << 1) | 1ULL;
        p.j = (j << 1) | 1ULL;
        break;
    default:
        break;
    }
    return p;
}

// suma de k2 trees usando recursion

void suma(vector<tuple<idx_type,idx_type,int>> &ret , k2_tree<2>& t1, k2_tree<2>& t2, int pos1, int pos2, bool flag1, bool flag2, idx_type i, idx_type j){
    if(pos1 == -1 && pos2 == -1){
        return;
    }
    auto p1 = t1.get_t();
    auto p2 = t2.get_t();
    auto l1 = t1.get_l();
    auto l2 = t2.get_l();
    cout << "Antes de shifteo, i=" << i << ", j=" << j << endl;
    if(is_leaf(pos1,t1) && is_leaf(pos2,t2)){
        flag1 = true;
        flag2 = true;
    }
    else if(is_leaf(pos1,t1) && !is_leaf(pos2,t2)){
        flag1 = true;
        flag2 = false;
    }
    else if(!is_leaf(pos1,t1) && is_leaf(pos2,t2)){
        flag1 = false;
        flag2 = true;
    }
    else{
        flag1 = false;
        flag2 = false;
    }
    if(flag1 && flag2){
        //uint64_t
        cout << "i=" << i << ", j=" << j << endl;
        idx_type d1 = t1.get_k_l_rank(pos1-t1.get_t().size()), d2 = t2.get_k_l_rank(pos2-t2.get_t().size());
        auto add = t1.get_v(d1) + t2.get_v(d2);
        tuple<idx_type,idx_type,int> r(i,j,add);
        ret.push_back(r);
    }
    else if(flag1 && !flag2){
        cout << "i=" << i << ", j=" << j << endl;
        idx_type d1 = t1.get_k_l_rank(pos1-t1.get_t().size());
        auto add = t1.get_v(d1);
        tuple<idx_type,idx_type,int> r(i,j,add);
        ret.push_back(r);
    }
    else if(!flag1 && flag2){
        cout << "i=" << i << ", j=" << j << endl;
        idx_type d2 = t2.get_k_l_rank(pos2-t2.get_t().size());
        auto add = t2.get_v(d2);
        tuple<idx_type,idx_type,int> r(i,j,add);
        ret.push_back(r);
    } // else if is zero and doesnt add to the vector
    else{
        //suma de los elementos de los nodos
        cout << "pos1=" << pos1 << ", pos2=" << pos2 << endl;
        //int t1c1 = (pos1 != -1) ? t1.get_child(pos1, 0) : -1;
        //int t2c1 = (pos2 != -1) ? t2.get_child(pos2, 0) : -1;
        //int t1c2 = (pos1 != -1) ? t1.get_child(pos1, 1) : -1;
        //int t2c2 = (pos2 != -1) ? t2.get_child(pos2, 1) : -1;
        //int t1c3 = (pos1 != -1) ? t1.get_child(pos1, 2) : -1;
        //int t2c3 = (pos2 != -1) ? t2.get_child(pos2, 2) : -1;
        //int t1c4 = (pos1 != -1) ? t1.get_child(pos1, 3) : -1;
        //int t2c4 = (pos2 != -1) ? t2.get_child(pos2, 3) : -1;
//
        //idx_type i1 = (i << 1) | 0ULL;
        //idx_type j1 = (j << 1) | 0ULL;
        //if (t1c1 != -1 || t2c1 != -1) {
        //    suma(ret, t1, t2, t1c1 != -1 ? t1c1 : -1, t2c1 != -1 ? t2c1 : -1, flag1, flag2, i1, j1);
        //}
//
        //idx_type i2 = (i << 1) | 0ULL;
        //idx_type j2 = (j << 1) | 1ULL;
        //if (t1c2 != -1 || t2c2 != -1) {
        //    suma(ret, t1, t2, t1c2 != -1 ? t1c2 : -1, t2c2 != -1 ? t2c2 : -1, flag1, flag2, i2, j2);
        //}
//
        //idx_type i3 = (i << 1) | 1ULL;
        //idx_type j3 = (j << 1) | 0ULL;
        //if (t1c3 != -1 || t2c3 != -1) {
        //    suma(ret, t1, t2, t1c3 != -1 ? t1c3 : -1, t2c3 != -1 ? t2c3 : -1, flag1, flag2, i3, j3);
        //}
//
        //idx_type i4 = (i << 1) | 1ULL;
        //idx_type j4 = (j << 1) | 1ULL;
        //if (t1c4 != -1 || t2c4 != -1) {
        //    suma(ret, t1, t2, t1c4 != -1 ? t1c4 : -1, t2c4 != -1 ? t2c4 : -1, flag1, flag2, i4, j4);
        //}

        //cambios

        for(int k=0; k<4; k++){
            int c1 = (pos1 != -1) ? t1.get_child(pos1, k) : -1;
            int c2 = (pos2 != -1) ? t2.get_child(pos2, k) : -1;
            position p = shifting(i,j,k);
            if(c1 != -1 || c2 != -1){
                suma(ret,t1,t2,c1 != -1 ? c1 : -1,c2 =! -1 ? c2 : -1,flag1,flag2,p.i,p.j);
            }
            else{
                return;
            }

        }
    
    }
}

void suma_test(vector<tuple<idx_type, idx_type, int>>& ret, k2_tree<2>& t1, k2_tree<2>& t2, idx_type pos1, idx_type pos2, bool flag1, bool flag2, idx_type i, idx_type j) {
    if(pos1 == -1 && pos2 == -1) {
        return;
    }
    if(is_leaf(pos1,t1) && is_leaf(pos2,t2)){
        flag1 = true;
        flag2 = true;
    }
    else if(is_leaf(pos1,t1) && !is_leaf(pos2,t2)){
        flag1 = true;
        flag2 = false;
    }
    else if(!is_leaf(pos1,t1) && is_leaf(pos2,t2)){
        flag1 = false;
        flag2 = true;
    }
    else{
        flag1 = false;
        flag2 = false;
    }
    if(flag1 && flag2){
        //uint64_t
        cout << "i=" << i << ", j=" << j << endl;
        if(t1.get_l()[pos1-t1.get_t().size()] == 1 && t2.get_l()[pos2-t2.get_t().size()] == 1){
            idx_type d1 = t1.get_k_l_rank(pos1-t1.get_t().size()), d2 = t2.get_k_l_rank(pos2-t2.get_t().size());
            auto add = t1.get_v(d1) + t2.get_v(d2);
            tuple<idx_type,idx_type,int> r(i,j,add);
            ret.push_back(r);
        }
        else if(t1.get_l()[pos1-t1.get_t().size()] == 1 && t2.get_l()[pos2-t2.get_t().size()] == 0){
            idx_type d1 = t1.get_k_l_rank(pos1-t1.get_t().size());
            auto add = t1.get_v(d1);
            tuple<idx_type,idx_type,int> r(i,j,add);
            ret.push_back(r);
        }
        else if(t1.get_l()[pos1-t1.get_t().size()] == 0 && t2.get_l()[pos2-t2.get_t().size()] == 1){
            idx_type d2 = t2.get_k_l_rank(pos2-t2.get_t().size());
            auto add = t2.get_v(d2);
            tuple<idx_type,idx_type,int> r(i,j,add);
            ret.push_back(r);
        }
        else{
            return;
        }
        //idx_type d1 = t1.get_k_l_rank(pos1-t1.get_t().size()), d2 = t2.get_k_l_rank(pos2-t2.get_t().size());
        //auto add = t1.get_v(d1) + t2.get_v(d2);
        //tuple<idx_type,idx_type,int> r(i,j,add);
        //ret.push_back(r);
    }
    else if(flag1 && !flag2){
        cout << "i=" << i << ", j=" << j << endl;
        if(t1.get_l()[pos1-t1.get_t().size()] == 1 && t2.get_l()[pos2-t2.get_t().size()] == 0){
            idx_type d1 = t1.get_k_l_rank(pos1-t1.get_t().size());
            auto add = t1.get_v(d1);
            tuple<idx_type,idx_type,int> r(i,j,add);
            ret.push_back(r);
        }
        //idx_type d1 = t1.get_k_l_rank(pos1-t1.get_t().size());
        //auto add = t1.get_v(d1);
        //tuple<idx_type,idx_type,int> r(i,j,add);
        //ret.push_back(r);
    }
    else if(!flag1 && flag2){
        cout << "i=" << i << ", j=" << j << endl;
        if(t1.get_l()[pos1-t1.get_t().size()] == 0 && t2.get_l()[pos2-t2.get_t().size()] == 1){
            idx_type d2 = t2.get_k_l_rank(pos2-t2.get_t().size());
            auto add = t2.get_v(d2);
            tuple<idx_type,idx_type,int> r(i,j,add);
            ret.push_back(r);
        }
        //idx_type d2 = t2.get_k_l_rank(pos2-t2.get_t().size());
        //auto add = t2.get_v(d2);
        //tuple<idx_type,idx_type,int> r(i,j,add);
        //ret.push_back(r);
    } else {
        // Suma de los elementos de los nodos
        cout << "pos1=" << pos1 << ", pos2=" << pos2 << endl;
        int t1c1 = (pos1 != -1) ? t1.get_child(pos1, 0) : -1;
        int t2c1 = (pos2 != -1) ? t2.get_child(pos2, 0) : -1;
        int t1c2 = (pos1 != -1) ? t1.get_child(pos1, 1) : -1;
        int t2c2 = (pos2 != -1) ? t2.get_child(pos2, 1) : -1;
        int t1c3 = (pos1 != -1) ? t1.get_child(pos1, 2) : -1;
        int t2c3 = (pos2 != -1) ? t2.get_child(pos2, 2) : -1;
        int t1c4 = (pos1 != -1) ? t1.get_child(pos1, 3) : -1;
        int t2c4 = (pos2 != -1) ? t2.get_child(pos2, 3) : -1;

        if (t1c1 == -1 && t2c1 == -1 && t1c2 == -1 && t2c2 == -1 && t1c3 == -1 && t2c3 == -1 && t1c4 == -1 && t2c4 == -1) {
            return;
        }

        idx_type i1 = (i << 1);
        idx_type j1 = (j << 1);
        if (t1c1 != -1 || t2c1 != -1) {
            suma_test(ret, t1, t2, t1c1, t2c1, flag1, flag2, i1 | 0ULL, j1 | 0ULL);
        }
        if (t1c2 != -1 || t2c2 != -1) {
            suma_test(ret, t1, t2, t1c2, t2c2, flag1, flag2, i1 | 0ULL, j1 | 1ULL);
        }
        if (t1c3 != -1 || t2c3 != -1) {
            suma_test(ret, t1, t2, t1c3, t2c3, flag1, flag2, i1 | 1ULL, j1 | 0ULL);
        }
        if (t1c4 != -1 || t2c4 != -1) {
            suma_test(ret, t1, t2, t1c4, t2c4, flag1, flag2, i1 | 1ULL, j1 | 1ULL);
        }
    }
}


//join 4 vector tuples into a single vector without element repetition
void join(vector<tuple<idx_type,idx_type,int>> &ret, vector<tuple<idx_type,idx_type,int>> &ret1, vector<tuple<idx_type,idx_type,int>> &ret2, vector<tuple<idx_type,idx_type,int>> &ret3, vector<tuple<idx_type,idx_type,int>> &ret4){
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

void multiplicar(vector<tuple<idx_type,idx_type,int>> &ret, k2_tree<2>& t1, k2_tree<2>& t2, int pos1, int pos2, bool flag1, bool flag2){
    vector<tuple<idx_type,idx_type,int>> c1, c2, c3, c4, c5, c6, c7, c8, ret1, ret2, ret3, ret4;
    if(is_leaf_1(pos1,t1)&&is_leaf_1(pos2,t2)){
        idx_type i1 = t1.get_k_l_rank(pos1-t1.get_t().size());
        idx_type i2 = t2.get_k_l_rank(pos2-t2.get_t().size());
        position k1, k2;
        k1 = get_reversed_z_order(i1);
        k2 = get_reversed_z_order(i2);
        if(k1.i == k2.j){
            int add = t1.get_v(i1)*t2.get_v(i2);
            tuple<idx_type,idx_type,int> r(k1.i,k2.j,add);
            ret.push_back(r);
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
            idx_type i = 0, j = 0;
            suma(ret1,t3,t4,0,0,false,false,i,j);
            suma(ret2,t5,t6,0,0,false,false,i,j);
            suma(ret3,t7,t8,0,0,false,false,i,j);
            suma(ret4,t9,t10,0,0,false,false,i,j);
            join(ret,ret1,ret2,ret3,ret4);
        }
    
}

unsigned int k0, k1;

k2_tree<2> sumaImpl(k2_tree<2>& t1, k2_tree<2>& t2){
    vector<tuple<idx_type,idx_type,int>> ret;
    k0 = clock();
    idx_type i = 0, j = 0;
    suma(ret,t1,t2,0,0,false,false,i,j);
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
    vector<tuple<idx_type,idx_type,int>> m1 = {{0,0,1},{1,1,2},{2,3,4},{3,0,3},{3,1,1},{1,2,8}};
    vector<tuple<idx_type,idx_type,int>> m2 = {{2,0,2},{3,0,1},{3,1,1},{0,2,2},{3,3,1}}; 

    k2_tree<2> arbol1(m1,m1.size());
    k2_tree<2> arbol2(m2,m2.size());

    cout << "test" << endl;

    vector<tuple<idx_type,idx_type,int>> ret;

    print_bit_vector(arbol1.get_t());
    print_bit_vector(arbol1.get_l());
    print_bit_vector(arbol2.get_t());
    print_bit_vector(arbol2.get_l());

    idx_type i = 0, j = 0;

    suma(ret,arbol1,arbol2,0,0,false,false,i,j);

    k2_tree<2> arbol3(ret,ret.size());

    print_bit_vector(arbol3.get_t());
    print_bit_vector(arbol3.get_l());

    //print ret
    for(int i=0; i<ret.size(); i++){
        cout << get<0>(ret[i]) << " " << get<1>(ret[i]) << " " << get<2>(ret[i]) << endl;
    }

    cout << "n=" << ret.size() << endl;

    return 0;

}