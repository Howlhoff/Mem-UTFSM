#include "k2_tree.hpp"
#include <iostream>
#include <vector>
#include "functions.cpp"
#include <tuple>
#include <ctime>

using namespace std;

typedef uint64_t idx_type;

bool is_leaf(int actual, k2_tree<2>& t){
    int leaf_index = actual-t.get_t().size();
    if(leaf_index < t.get_l().size()){
        return true;
    }
    else{
        return false;
    }
}

bool is_previous_leaf(int actual, k2_tree<2>& t){
    if(is_leaf(t.get_child(actual,0),t)&&is_leaf(t.get_child(actual,1),t)&&is_leaf(t.get_child(actual,2),t)&&is_leaf(t.get_child(actual,3),t)){
        return true;
    }
    else{
        return false;
    }
}

int get_leaf_value(k2_tree<2>& tree, int child_pos) {
    if (child_pos >= tree.get_t().size() && child_pos < tree.get_t().size() + tree.get_l().size()) {
        return tree.get_l()[child_pos - tree.get_t().size()] == 1 ? tree.get_v(tree.get_k_l_rank(child_pos - tree.get_t().size())) : 0;
    }
    return 0;
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

void suma(vector<tuple<idx_type, idx_type, int>>& ret, k2_tree<2>& t1, k2_tree<2>& t2, idx_type pos1, idx_type pos2, bool flag1, bool flag2, idx_type i, idx_type j) {
    if(pos1 == -1 && pos2 == -1) {
        return;
    }
    if(is_leaf(pos1,t1)){
        flag1 = true;
    }
    else{
        flag1 = false;
    }
    if(is_leaf(pos2,t2)){
        flag2 = true;
    }
    else{
        flag2 = false;
    }
    if(flag1 && flag2){
        //uint64_t
        auto index1 = pos1-t1.get_t().size();
        auto index2 = pos2-t2.get_t().size();
        if(t1.get_l()[index1] == 1 && t2.get_l()[index2] == 1){
            idx_type d1 = t1.get_k_l_rank(index1), d2 = t2.get_k_l_rank(index2);
            auto add = t1.get_v(d1) + t2.get_v(d2);
            tuple<idx_type,idx_type,int> r(i,j,add);
            ret.push_back(r);
        }
        else if(t1.get_l()[index1] == 1 && t2.get_l()[index2] == 0){
            idx_type d1 = t1.get_k_l_rank(index1);
            auto add = t1.get_v(d1);
            tuple<idx_type,idx_type,int> r(i,j,add);
            ret.push_back(r);
        }
        else if(t1.get_l()[index1] == 0 && t2.get_l()[index2] == 1){
            idx_type d2 = t2.get_k_l_rank(index2);
            auto add = t2.get_v(d2);
            tuple<idx_type,idx_type,int> r(i,j,add);
            ret.push_back(r);
        }
        else{
            return;
        }
    }
    else if(flag1 && !flag2){
        if(t1.get_l()[pos1-t1.get_t().size()] == 1){
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

        if(t2.get_l()[pos2-t2.get_t().size()] == 1){
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
            suma(ret, t1, t2, t1c1, t2c1, flag1, flag2, i1 | 0ULL, j1 | 0ULL);
        }
        if (t1c2 != -1 || t2c2 != -1) {
            suma(ret, t1, t2, t1c2, t2c2, flag1, flag2, i1 | 0ULL, j1 | 1ULL);
        }
        if (t1c3 != -1 || t2c3 != -1) {
            suma(ret, t1, t2, t1c3, t2c3, flag1, flag2, i1 | 1ULL, j1 | 0ULL);
        }
        if (t1c4 != -1 || t2c4 != -1) {
            suma(ret, t1, t2, t1c4, t2c4, flag1, flag2, i1 | 1ULL, j1 | 1ULL);
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

void mult_squared_mat(vector<tuple<idx_type,idx_type,int>>& ret, k2_tree<2>& t1, k2_tree<2>& t2, int pos1, int pos2, bool flag1, bool flag2, idx_type i, idx_type j){
    if(flag1&&flag2){
        int c1[4], c2[4];
        for(int k=0; k<4; k++){
            c1[k] = (pos1 != -1) ? t1.get_child(pos1,k) : -1;
            c2[k] = (pos2 != -1) ? t2.get_child(pos2,k) : -1;
        }
        
        bool all_empty = true;
        for(int k=0; k<4; k++){
            if(c1[k] != -1 || c2[k] != -1){
                all_empty = false;
                break;
            }
        }
        if(all_empty){
            return;
        }

        idx_type i1 = (i << 1);
        idx_type j1 = (j << 1);
        int val1[4],val2[4];
        for(int k=0; k<4; k++){
            val1[k] = get_leaf_value(t1,c1[k]);
            val2[k] = get_leaf_value(t2,c2[k]);
        }
        int mul[4];
        mul[0] = val1[0]*val2[0]+val1[1]*val2[2];
        mul[1] = val1[0]*val2[1]+val1[1]*val2[3];
        mul[2] = val1[2]*val2[0]+val1[3]*val2[2];
        mul[3] = val1[2]*val2[1]+val1[3]*val2[3];
        cout << "Multiplication results: mul[0]=" << mul[0] << ", mul[1]=" << mul[1]
     << ", mul[2]=" << mul[2] << ", mul[3]=" << mul[3] << endl;
        tuple<idx_type,idx_type,int> r1(i1|0ULL,j1|0ULL,mul[0]);
        tuple<idx_type,idx_type,int> r2(i1|0ULL,j1|1ULL,mul[1]);
        tuple<idx_type,idx_type,int> r3(i1|1ULL,j1|0ULL,mul[2]);
        tuple<idx_type,idx_type,int> r4(i1|1ULL,j1|1ULL,mul[3]);

        if(mul[0] != 0){
            cout << "Adding to ret: (" << i1 << ", " << j1 << ", " << mul[0] << ")" << endl;
            ret.push_back(r1);
        }
        if(mul[1] != 0){
            cout << "Adding to ret: (" << i1 << ", " << j1 << ", " << mul[1] << ")" << endl;
            ret.push_back(r2);
        }
        if(mul[2] != 0){
            cout << "Adding to ret: (" << i1 << ", " << j1 << ", " << mul[0] << ")" << endl;
            ret.push_back(r3);
        }
        if(mul[3] != 0){
            cout << "Adding to ret: (" << i1 << ", " << j1 << ", " << mul[0] << ")" << endl;
            ret.push_back(r4);
        }
        return;
    }
    else{
        return;
    }
}

void multiplicar_helper(vector<tuple<idx_type,idx_type,int>>& ret, k2_tree<2>& t1, k2_tree<2>& t2, int pos11, int pos12, int pos21, int pos22, bool flag1, bool flag2, idx_type i, idx_type j){
    if(pos11 == -1 || pos12 == -1 || pos21 == -1 || pos22 == -1){
        return;
    }
    if(is_leaf(pos11,t1) && is_leaf(pos12,t1)){
        flag1 = true;
    }
    else{
        flag1 = false;
    }
    if(is_leaf(pos21,t2) && is_leaf(pos22,t2)){
        flag2 = true;
    }
    else{
        flag2 = false;
    }
    if(flag1 && flag2){
        mult_squared_mat(ret,t1,t2,pos11,pos21,flag1,flag2,i,j);
        mult_squared_mat(ret,t1,t2,pos12,pos22,flag1,flag2,i,j);
    }
    else{
        int t1c1 = (pos11 != -1) ? t1.get_child(pos11, 0) : -1;
        int t2c1 = (pos21 != -1) ? t2.get_child(pos21, 0) : -1;
        int t1c2 = (pos11 != -1) ? t1.get_child(pos11, 1) : -1;
        int t2c2 = (pos21 != -1) ? t2.get_child(pos21, 1) : -1;
        int t1c3 = (pos12 != -1) ? t1.get_child(pos12, 2) : -1;
        int t2c3 = (pos22 != -1) ? t2.get_child(pos22, 2) : -1;
        int t1c4 = (pos12 != -1) ? t1.get_child(pos12, 3) : -1;
        int t2c4 = (pos22 != -1) ? t2.get_child(pos22, 3) : -1;

        if (t1c1 == -1 && t2c1 == -1 && t1c2 == -1 && t2c2 == -1 && t1c3 == -1 && t2c3 == -1 && t1c4 == -1 && t2c4 == -1) {
            return;
        }

        idx_type i1 = (i << 1);
        idx_type j1 = (j << 1);
        if (t1c1 != -1 || t2c1 != -1 || t1c2 != -1 || t2c3 != -1) {
            multiplicar_helper(ret, t1, t2, t1c1, t1c2, t2c1, t2c3, flag1, flag2, i1 | 0ULL, j1 | 0ULL);
        }
        if (t1c2 != -1 || t2c2 != -1 || t1c2 != -1 || t2c4 != -1) {
            multiplicar_helper(ret, t1, t2, t1c1, t1c2, t2c2, t2c4, flag1, flag2, i1 | 0ULL, j1 | 1ULL);
        }
        if (t1c3 != -1 || t2c1 != -1 || t1c4 != -1 || t2c3 != -1) {
            multiplicar_helper(ret, t1, t2, t1c3, t1c4, t2c1, t2c3, flag1, flag2, i1 | 1ULL, j1 | 0ULL);
        }
        if (t1c4 != -1 || t2c2 != -1 || t1c4 != -1 || t2c4 != -1) {
            multiplicar_helper(ret, t1, t2, t1c3, t1c4, t2c2, t2c4, flag1, flag2, i1 | 1ULL, j1 | 1ULL);
        }
    }    
    
}

void multiplicar_test(vector<tuple<idx_type, idx_type, int>>& ret, k2_tree<2>& t1, k2_tree<2>& t2, int pos11, int pos12, int pos21, int pos22, bool flag1, bool flag2, idx_type i, idx_type j) {
    cout << "multiplicar_test called with pos11=" << pos11 << ", pos2=" << pos12 << ", i=" << i << ", j=" << j << ", flag1=" << flag1 << ", flag2=" << flag2 << endl;


    if (pos11 == -1 || pos12 == -1 || pos21 == -1 || pos22 == -1) {
        return;
    }
    if(is_previous_leaf(pos11,t1) && is_previous_leaf(pos12,t1)){
        flag1 = true;
    }
    else{
        flag1 = false;
    }
    if(is_previous_leaf(pos21,t2) && is_previous_leaf(pos22,t2)){
        flag2 = true;
    }
    else{
        flag2 = false;
    }
    if(flag1 && flag2){
        cout << "Calling multiplicar_helper with leaf nodes at pos11=" << pos11 << " and pos12=" << pos12 << endl;
        multiplicar_helper(ret,t1,t2,pos11,pos12,pos21,pos22,flag1,flag2,i,j);
    }
    else{
        idx_type i1 = (i << 1);
        idx_type j1 = (j << 1);

        int c1[4], c2[4], c3[4], c4[4];
        for(int k=0; k<4; k++){
            c1[k] = (pos11 != -1) ? t1.get_child(pos11,k) : -1;
            c2[k] = (pos12 != -1) ? t1.get_child(pos12,k) : -1;
            c3[k] = (pos21 != -1) ? t2.get_child(pos21,k) : -1;
            c4[k] = (pos22 != -1) ? t2.get_child(pos22,k) : -1;
        }

        if(all_of(begin(c1),end(c1),[](int x){return x==-1;})&&
        all_of(begin(c2),end(c2),[](int x){return x==-1;})&&
        all_of(begin(c3),end(c3),[](int x){return x==-1;})&&
        all_of(begin(c4),end(c4),[](int x){return x==-1;})){
            return;
        }
        else{
            if(c1[0] != -1 || c2[1] != -1 || c3[0] != -1 || c4[2] != -1){
                multiplicar_test(ret,t1,t2,c1[0],c2[1],c3[0],c4[2],flag1,flag2,i1|0ULL,j1|0ULL);
            }
            if(c1[0] != -1 || c2[1] != -1 || c3[1] != -1 || c4[3] != -1){
                multiplicar_test(ret,t1,t2,c1[0],c2[1],c3[1],c4[3],flag1,flag2,i1|0ULL,j1|1ULL);
            }
            if(c1[2] != -1 || c2[3] != -1 || c3[0] != -1 || c4[2] != -1){
                multiplicar_test(ret,t1,t2,c1[2],c2[3],c3[0],c4[2],flag1,flag2,i1|1ULL,j1|0ULL);
            }
            if(c1[2] != -1 || c2[3] != -1 || c3[1] != -1 || c4[3] != -1){
                multiplicar_test(ret,t1,t2,c1[2],c2[3],c3[1],c4[3],flag1,flag2,i1|1ULL,j1|1ULL);
            }
        }

    }
    
}

void multiplicar(vector<tuple<idx_type,idx_type,int>> &ret, k2_tree<2>& t1, k2_tree<2>& t2, int pos1, int pos2, bool flag1, bool flag2) {
    vector<tuple<idx_type,idx_type,int>> c1, c2, c3, c4, c5, c6, c7, c8, ret1, ret2, ret3, ret4;
    if (pos1 == -1 || pos2 == -1) {
        return;
    }
    if (is_leaf(pos1, t1) && is_leaf(pos2, t2)) {
        idx_type i1 = t1.get_k_l_rank(pos1 - t1.get_t().size());
        idx_type i2 = t2.get_k_l_rank(pos2 - t2.get_t().size());
        position k1, k2;
        k1 = get_reversed_z_order(i1);
        k2 = get_reversed_z_order(i2);
        if (k1.i == k2.j) {
            int add = t1.get_v(i1) * t2.get_v(i2);
            tuple<idx_type,idx_type,int> r(k1.i, k2.j, add);
            ret.push_back(r);
        }
    } else {
        multiplicar(c1, t1, t2, t1.get_child(pos1, 0), t2.get_child(pos2, 0), flag1, flag2);
        multiplicar(c2, t1, t2, t1.get_child(pos1, 1), t2.get_child(pos2, 2), flag1, flag2);
        multiplicar(c3, t1, t2, t1.get_child(pos1, 0), t2.get_child(pos2, 1), flag1, flag2);
        multiplicar(c4, t1, t2, t1.get_child(pos1, 1), t2.get_child(pos2, 3), flag1, flag2);
        multiplicar(c5, t1, t2, t1.get_child(pos1, 2), t2.get_child(pos2, 0), flag1, flag2);
        multiplicar(c6, t1, t2, t1.get_child(pos1, 3), t2.get_child(pos2, 2), flag1, flag2);
        multiplicar(c7, t1, t2, t1.get_child(pos1, 2), t2.get_child(pos2, 1), flag1, flag2);
        multiplicar(c8, t1, t2, t1.get_child(pos1, 3), t2.get_child(pos2, 3), flag1, flag2);
        k2_tree<2> t3(c1, c1.size());
        k2_tree<2> t4(c2, c2.size());
        k2_tree<2> t5(c3, c3.size());
        k2_tree<2> t6(c4, c4.size());
        k2_tree<2> t7(c5, c5.size());
        k2_tree<2> t8(c6, c6.size());
        k2_tree<2> t9(c7, c7.size());
        k2_tree<2> t10(c8, c8.size());
        idx_type i = 0, j = 0;
        suma(ret1, t3, t4, 0, 0, false, false, i, j);
        suma(ret2, t5, t6, 0, 0, false, false, i, j);
        suma(ret3, t7, t8, 0, 0, false, false, i, j);
        suma(ret4, t9, t10, 0, 0, false, false, i, j);
        join(ret, ret1, ret2, ret3, ret4);
    }
}

void delete_zeros(vector<tuple<idx_type,idx_type,int>> &ret){
    for(int i=0; i<ret.size(); i++){
        if(get<2>(ret[i]) == 0){
            ret.erase(ret.begin()+i);
        }
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
    cout << "time: " << (double(k1-k0)/CLOCKS_PER_SEC) << endl;
    return t3;
}

k2_tree<2> multiplicarImpl(k2_tree<2>& t1, k2_tree<2>& t2){
    vector<tuple<idx_type,idx_type,int>> ret;
    k0 = clock();
    multiplicar(ret,t1,t2,0,0,false,false);
    k2_tree<2> t3(ret,ret.size());
    k1 = clock();
    cout << "time: " << (double(k1-k0)/CLOCKS_PER_SEC) << endl;
    return t3;
}

double _time = (double(k1-k0)/CLOCKS_PER_SEC);

void print_bit_vector(const bit_vector &bv) {
    for (const auto &bit : bv) {
        std::cout << bit;
    }
    std::cout << std::endl;
}

void print_from_list_to_matrix(vector<tuple<idx_type,idx_type,int>> m){
    //print a matrix from a list of tuples
    int n = 0;
    for(int i=0; i<m.size(); i++){
        if(get<0>(m[i]) > n){
            n = get<0>(m[i]);
        }
    }
    n++;
    vector<vector<int>> matrix(n,vector<int>(n,0));
    for(int i=0; i<m.size(); i++){
        matrix[get<0>(m[i])][get<1>(m[i])] = get<2>(m[i]);
    }
    for(int i=0; i<n; i++){
        for(int j=0; j<n; j++){
            cout << matrix[i][j] << " ";
        }
        cout << endl;
    }
}

void print_vector(vector<int> v){
    for(int i=0; i<v.size(); i++){
        cout << v[i] << " ";
    }
    cout << endl;
}

int main(){
    vector<tuple<idx_type,idx_type,int>> m1 = {{0,0,1},{1,1,2},{0,3,3},{1,3,1},{2,1,8},{3,2,4}};
    vector<tuple<idx_type,idx_type,int>> m2 = {{0,2,2},{0,3,1},{1,3,1},{2,0,2},{3,3,1}}; 
    
    k2_tree<2> arbol1(m1,m1.size());
    k2_tree<2> arbol2(m2,m2.size());

    print_vector(arbol1.get_l_values());
    print_vector(arbol2.get_l_values());
    
    cout << "test" << endl;

    vector<tuple<idx_type,idx_type,int>> ret;

    print_bit_vector(arbol1.get_t());
    print_bit_vector(arbol1.get_l());
    print_bit_vector(arbol2.get_t());
    print_bit_vector(arbol2.get_l());

    idx_type i = 0, j = 0;

    //suma(ret,arbol1,arbol2,0,0,false,false,i,j);
    multiplicar_test(ret,arbol1,arbol2,0,0,0,0,false,false,i,j);
    k2_tree<2> arbol3(ret,ret.size());

    print_bit_vector(arbol3.get_t());
    print_bit_vector(arbol3.get_l());

    //print ret
    cout << "m1" << endl;
    print_from_list_to_matrix(m1);
    cout << "m2" << endl;
    print_from_list_to_matrix(m2);
    cout << "ret" << endl;
    print_from_list_to_matrix(ret);
    cout << "ret values" << endl;
    print_vector(arbol3.get_l_values());

    for(int i=0; i<ret.size(); i++){
        cout << get<0>(ret[i]) << " " << get<1>(ret[i]) << " " << get<2>(ret[i]) << endl;
    }

    cout << "n=" << ret.size() << endl;

    return 0;

}