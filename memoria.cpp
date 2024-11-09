#include "k2_tree.hpp"
#include <iostream>
#include <vector>
#include "functions.cpp"
#include <tuple>
#include <ctime>

using namespace std;

typedef uint64_t idx_type;

bool is_leaf(int node_index, k2_tree<2>& t){
    int leaf_index = node_index - t.get_t().size();
    return leaf_index >= 0 && leaf_index < t.get_l().size();
}

bool is_previous_leaf(int actual, k2_tree<2>& t){
    return is_leaf(t.get_child(actual, 0), t) &&
           is_leaf(t.get_child(actual, 1), t) &&
           is_leaf(t.get_child(actual, 2), t) &&
           is_leaf(t.get_child(actual, 3), t);
}

int get_leaf_value(k2_tree<2>& tree, int child_pos) {
    bool flag = child_pos >= tree.get_t().size() && child_pos < tree.get_t().size() + tree.get_l().size(); 
    if (flag) {
        auto a = tree.get_l()[child_pos - tree.get_t().size()] == 1 ? tree.get_v(tree.get_k_l_rank(child_pos - tree.get_t().size())) : 0;
        cout << "Leaf value: " << a << endl;
        return a;
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
    p.i = (i << 1) | ((k == 2 || k == 3) ? 1ULL : 0ULL);
    p.j = (j << 1) | ((k == 1 || k == 3) ? 1ULL : 0ULL);
    return p;
}

// suma de k2 trees usando recursion

/**
 * Recursively sums the values of two k2_trees and stores the result in a vector of tuples.
 * 
 * @param ret The vector to store the result tuples.
 * @param t1 The first k2_tree.
 * @param t2 The second k2_tree.
 * @param pos1 The current position in the first k2_tree.
 * @param pos2 The current position in the second k2_tree.
 * @param flag1 Indicates if the current node in the first k2_tree is a leaf.
 * @param flag2 Indicates if the current node in the second k2_tree is a leaf.
 * @param i The current row index in the result matrix.
 * @param j The current column index in the result matrix.
 */
void suma(vector<tuple<idx_type, idx_type, int>>& ret, k2_tree<2>& t1, k2_tree<2>& t2, idx_type pos1, idx_type pos2, bool flag1, bool flag2, idx_type i, idx_type j) {
    if(pos1 == -2 && pos2 == -2){
        idx_type i1 = (i << 1);
        idx_type j1 = (j << 1);
        suma(ret, t1, t2, 0, 0, flag1, flag2, i1 | 0ULL, j1 | 0ULL);
        suma(ret, t1, t2, 1, 1, flag1, flag2, i1 | 0ULL, j1 | 1ULL);
        suma(ret, t1, t2, 2, 2, flag1, flag2, i1 | 1ULL, j1 | 0ULL);
        suma(ret, t1, t2, 3, 3, flag1, flag2, i1 | 1ULL, j1 | 1ULL);
    }
    if (pos1 == -1 && pos2 == -1) {
        return;
    }

    flag1 = is_leaf(pos1, t1);
    flag2 = is_leaf(pos2, t2);

    if (flag1 && flag2) {
        auto index1 = pos1 - t1.get_t().size();
        auto index2 = pos2 - t2.get_t().size();
        if (t1.get_l()[index1] == 1 && t2.get_l()[index2] == 1) {
            idx_type d1 = t1.get_k_l_rank(index1), d2 = t2.get_k_l_rank(index2);
            auto add = t1.get_v(d1) + t2.get_v(d2);
            ret.emplace_back(i, j, add);
        } else if (t1.get_l()[index1] == 1) {
            idx_type d1 = t1.get_k_l_rank(index1);
            auto add = t1.get_v(d1);
            ret.emplace_back(i, j, add);
        } else if (t2.get_l()[index2] == 1) {
            idx_type d2 = t2.get_k_l_rank(index2);
            auto add = t2.get_v(d2);
            ret.emplace_back(i, j, add);
        }
        return;
    }

    if (flag1) {
        if (t1.get_l()[pos1 - t1.get_t().size()] == 1) {
            idx_type d1 = t1.get_k_l_rank(pos1 - t1.get_t().size());
            auto add = t1.get_v(d1);
            ret.emplace_back(i, j, add);
        }
        return;
    }

    if (flag2) {
        if (t2.get_l()[pos2 - t2.get_t().size()] == 1) {
            idx_type d2 = t2.get_k_l_rank(pos2 - t2.get_t().size());
            auto add = t2.get_v(d2);
            ret.emplace_back(i, j, add);
        }
        return;
    }

    // Suma de los elementos de los nodos
    int t1c1 = (pos1 != -1) ? t1.get_child(pos1, 0) : -1; //A1
    int t2c1 = (pos2 != -1) ? t2.get_child(pos2, 0) : -1; //B1
    int t1c2 = (pos1 != -1) ? t1.get_child(pos1, 1) : -1; //A2
    int t2c2 = (pos2 != -1) ? t2.get_child(pos2, 1) : -1; //B2
    int t1c3 = (pos1 != -1) ? t1.get_child(pos1, 2) : -1; //A3
    int t2c3 = (pos2 != -1) ? t2.get_child(pos2, 2) : -1; //B3
    int t1c4 = (pos1 != -1) ? t1.get_child(pos1, 3) : -1; //A4
    int t2c4 = (pos2 != -1) ? t2.get_child(pos2, 3) : -1; //B4

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

void print_vector_tuple(vector<tuple<idx_type,idx_type,int>> v){
    for(int i=0; i<v.size(); i++){
        cout << get<0>(v[i]) << " " << get<1>(v[i]) << " " << get<2>(v[i]) << endl;
    }
}

void print_bit_vector(const bit_vector &bv) {
    for (const auto &bit : bv) {
        std::cout << bit;
    }
    std::cout << std::endl;
}

//multiplicacion de matrices usando divide y venceras

/**
 * Multiplies two k2_trees and stores the result in a vector of tuples.
 * 
 * @param ret The vector to store the result tuples.
 * @param t1 The first k2_tree.
 * @param t2 The second k2_tree.
 * @param pos1 The current position in the first k2_tree.
 * @param pos2 The current position in the second k2_tree.
 * @param flag1 Indicates if the current node in the first k2_tree is a leaf.
 * @param flag2 Indicates if the current node in the second k2_tree is a leaf.
 * @param i The current row index in the result matrix.
 * @param j The current column index in the result matrix.
 */
void mult_new_test(vector<tuple<idx_type, idx_type, int>>& ret, k2_tree<2>& t1, k2_tree<2>& t2, idx_type pos1, idx_type pos2, bool flag1, bool flag2, idx_type i, idx_type j) {
    if (pos1 == -2 && pos2 == -2) {
        idx_type i1 = (i << 1);
        idx_type j1 = (j << 1);
        vector<tuple<idx_type, idx_type, int>> ret1, ret2, ret3, ret4, ret5, ret6, ret7, ret8;
        //C1 = A1*B1 + A2*B3
        mult_new_test(ret1, t1, t2, 0, 0, flag1, flag2, i1, j1);
        mult_new_test(ret2, t1, t2, 1, 2, flag1, flag2, i1, j1);
        //C2 = A1*B2 + A2*B4
        mult_new_test(ret3, t1, t2, 0, 1, flag1, flag2, i1, j1);
        mult_new_test(ret4, t1, t2, 1, 3, flag1, flag2, i1, j1);
        //C3 = A3*B1 + A4*B3
        mult_new_test(ret5, t1, t2, 2, 0, flag1, flag2, i1, j1);
        mult_new_test(ret6, t1, t2, 3, 2, flag1, flag2, i1, j1);
        //C4 = A3*B2 + A4*B4
        mult_new_test(ret7, t1, t2, 2, 1, flag1, flag2, i1, j1);
        mult_new_test(ret8, t1, t2, 3, 3, flag1, flag2, i1, j1);
        k2_tree<2> r1(ret1), r2(ret2), r3(ret3), r4(ret4), r5(ret5), r6(ret6), r7(ret7), r8(ret8);
        suma(ret, r1, r2, -2, -2, false, false, i | 0ULL, j | 0ULL);
        suma(ret, r3, r4, -2, -2, false, false, i | 0ULL, j | 1ULL);
        suma(ret, r5, r6, -2, -2, false, false, i | 1ULL, j | 0ULL);
        suma(ret, r7, r8, -2, -2, false, false, i | 1ULL, j | 1ULL);

    }
    cout << "pos1: " << pos1 << " pos2: " << pos2 << endl;
    if (pos1 == -1 && pos2 == -1) {
        return;
    }
    print_vector_tuple(ret);
    flag1 = is_leaf(pos1, t1);
    flag2 = is_leaf(pos2, t2);
    if (flag1 && flag2) {
        // uint64_t
        cout << "c0" << endl;
        auto index1 = pos1 - t1.get_t().size();
        auto index2 = pos2 - t2.get_t().size();
        cout << "index1: " << index1 << " index2: " << index2 << endl;
        bool k1 = index1 >= 0 && index1 < t1.get_l().size();
        bool k2 = index2 >= 0 && index2 < t2.get_l().size();
        cout << "k1: " << k1 << " k2: " << k2 << endl;
        if(k1 && k2){
            cout << "p1" << endl;
            bool u1 = t1.get_l()[index1] == 1;
            bool u2 = t2.get_l()[index2] == 1;
            cout << "u1: " << u1 << " u2: " << u2 << endl;
            if (u1 && u2) {
                idx_type d1 = t1.get_k_l_rank(index1), d2 = t2.get_k_l_rank(index2);
                cout << "v1 = " << t1.get_v(d1) << " v2 = " << t2.get_v(d2) << endl;
                auto add = t1.get_v(d1) * t2.get_v(d2);
                cout << "add: " << add << endl;
                cout << "p2" << endl;
                tuple<idx_type, idx_type, int> r(i, j, add);
                ret.push_back(r);
            }
            else{
                return;
            }
        }
        else{
            return;
        }
    }
    //C = A*0 = 0
    else if(flag1 && !flag2){
        cout << "c1" << endl;
        return;
    }
    //C = 0*A = 0
    else if(!flag1 && flag2){
        cout << "c2" << endl;
        return;
    } else {
        cout << "a" << endl;
        int t1c1 = (pos1 != -1) ? t1.get_child(pos1, 0) : -1; //A1
        int t2c1 = (pos2 != -1) ? t2.get_child(pos2, 0) : -1; //B1
        int t1c2 = (pos1 != -1) ? t1.get_child(pos1, 1) : -1; //A2
        int t2c2 = (pos2 != -1) ? t2.get_child(pos2, 1) : -1; //B2
        int t1c3 = (pos1 != -1) ? t1.get_child(pos1, 2) : -1; //A3
        int t2c3 = (pos2 != -1) ? t2.get_child(pos2, 2) : -1; //B3
        int t1c4 = (pos1 != -1) ? t1.get_child(pos1, 3) : -1; //A4
        int t2c4 = (pos2 != -1) ? t2.get_child(pos2, 3) : -1; //B4

        cout << "Children: t1c1=" << t1c1 << ", t2c1=" << t2c1 << ", t1c2=" << t1c2 << ", t2c2=" << t2c2 << ", t1c3=" << t1c3 << ", t2c3=" << t2c3 << ", t1c4=" << t1c4 << ", t2c4=" << t2c4 << endl;

        if (t1c1 == -1 && t2c1 == -1 && t1c2 == -1 && t2c2 == -1 && t1c3 == -1 && t2c3 == -1 && t1c4 == -1 && t2c4 == -1) {
            return;
        }

        idx_type i1 = (i << 1);
        idx_type j1 = (j << 1);
        vector<tuple<idx_type, idx_type, int>> ret1;
        vector<tuple<idx_type, idx_type, int>> ret2;
        vector<tuple<idx_type, idx_type, int>> ret3;
        vector<tuple<idx_type, idx_type, int>> ret4;
        vector<tuple<idx_type, idx_type, int>> ret5;
        vector<tuple<idx_type, idx_type, int>> ret6;
        vector<tuple<idx_type, idx_type, int>> ret7;
        vector<tuple<idx_type, idx_type, int>> ret8;
        //C1 = A1*B1 + A2*B3
        if (t1c1 != -1 || t2c1 != -1 || t1c2 != -1 || t2c3 != -1) {
            mult_new_test(ret1, t1, t2, t1c1, t2c1, flag1, flag2, i1 | 0ULL, j1 | 0ULL);
            mult_new_test(ret2, t1, t2, t1c2, t2c3, flag1, flag2, i1 | 0ULL, j1 | 0ULL);        
            print_vector_tuple(ret1);
            print_vector_tuple(ret2);
            k2_tree<2> z1(ret1, ret1.size()), z2(ret2, ret2.size());
            suma(ret, z1, z2, 0, 0, false, false, 0, 0);
            cout << "b1" << endl;
        }
        //sumar ret1 y ret 2
        //C2 = A1*B2 + A2*B4
        if (t1c1 != -1 || t2c2 != -1 || t1c2 != -1 || t2c4 != -1) {
            mult_new_test(ret3, t1, t2, t1c1, t2c2, flag1, flag2, i1 | 0ULL, j1 | 1ULL);
            mult_new_test(ret4, t1, t2, t1c2, t2c4, flag1, flag2, i1 | 0ULL, j1 | 1ULL);    
            print_vector_tuple(ret3);
            print_vector_tuple(ret4);
            k2_tree<2> z1(ret3, ret3.size()), z2(ret4, ret4.size());
            suma(ret, z1, z2, 0, 0, false, false, 0, 0);
            cout << "b2" << endl;
        }
        //sumar ret3 y ret4
        //C3 = A3*B1 + A4*B3
        if(t1c3 != -1 || t2c1 != -1 || t1c4 != -1 || t2c3 != -1){
            mult_new_test(ret5, t1, t2, t1c3, t2c1, flag1, flag2, i1 | 1ULL, j1 | 0ULL);
            mult_new_test(ret6, t1, t2, t1c4, t2c3, flag1, flag2, i1 | 1ULL, j1 | 0ULL);
            print_vector_tuple(ret5);
            print_vector_tuple(ret6);
            k2_tree<2> z1(ret5, ret5.size()), z2(ret6, ret6.size());
            suma(ret, z1, z2, 0, 0, false, false, 0, 0);
            cout << "b3" << endl;
        }
        //sumar ret5 y ret6
        //C4 = A3*B2 + A4*B4
        if(t1c3 != -1 || t2c2 != -1 || t1c4 != -1 || t2c4 != -1){
            mult_new_test(ret7, t1, t2, t1c3, t2c2, flag1, flag2, i1 | 1ULL, j1 | 1ULL);
            mult_new_test(ret8, t1, t2, t1c4, t2c4, flag1, flag2, i1 | 1ULL, j1 | 1ULL);
            print_vector_tuple(ret7);
            print_vector_tuple(ret8);
            k2_tree<2> z1(ret7, ret7.size()), z2(ret8, ret8.size());
            suma(ret, z1, z2, 0, 0, false, false, 0, 0);
            cout << "b4" << endl;
        }
        //sumar ret7 y ret8
    }
}

void test_mult_test(vector<tuple<idx_type, idx_type, int>>& ret, k2_tree<2>& t1, k2_tree<2>& t2, idx_type pos1, idx_type pos2, bool flag1, bool flag2, idx_type i, idx_type j){
    if(pos1 == -2 && pos2 == -2){
        idx_type i1 = (i << 1);
        idx_type j1 = (j << 1);
        vector<tuple<idx_type, idx_type, int>> ret1, ret2, ret3, ret4, ret5, ret6, ret7, ret8;
        //C1 = A1*B1 + A2*B3
        test_mult_test(ret1, t1, t2, 0, 0, flag1, flag2, i1, j1);
        test_mult_test(ret2, t1, t2, 1, 2, flag1, flag2, i1, j1);
        //C2 = A1*B2 + A2*B4
        test_mult_test(ret3, t1, t2, 0, 1, flag1, flag2, i1, j1);
        test_mult_test(ret4, t1, t2, 1, 3, flag1, flag2, i1, j1);
        //C3 = A3*B1 + A4*B3
        test_mult_test(ret5, t1, t2, 2, 0, flag1, flag2, i1, j1);
        test_mult_test(ret6, t1, t2, 3, 2, flag1, flag2, i1, j1);
        //C4 = A3*B2 + A4*B4
        test_mult_test(ret7, t1, t2, 2, 1, flag1, flag2, i1, j1);
        test_mult_test(ret8, t1, t2, 3, 3, flag1, flag2, i1, j1);
        k2_tree<2> r1(ret1), r2(ret2), r3(ret3), r4(ret4), r5(ret5), r6(ret6), r7(ret7), r8(ret8);
        suma(ret, r1, r2, -2, -2, false, false, i, j);
        suma(ret, r3, r4, -2, -2, false, false, i, j);
        suma(ret, r5, r6, -2, -2, false, false, i, j);
        suma(ret, r7, r8, -2, -2, false, false, i, j);
    }
    cout << "pos1: " << pos1 << " pos2: " << pos2 << endl;

    if (pos1 == -1 && pos2 == -1) {
        return;
    }
    print_vector_tuple(ret);

    flag1 = is_leaf(pos1, t1);
    flag2 = is_leaf(pos2, t2);

    if (flag1 && flag2) {
        cout << "c0" << endl;
        auto index1 = pos1 - t1.get_t().size();
        auto index2 = pos2 - t2.get_t().size();
        cout << "index1: " << index1 << " index2: " << index2 << endl;

        if (index1 >= 0 && index1 < t1.get_l().size() && index2 >= 0 && index2 < t2.get_l().size()) {
            bool u1 = t1.get_l()[index1] == 1;
            bool u2 = t2.get_l()[index2] == 1;
            cout << "u1: " << u1 << " u2: " << u2 << endl;

            if (u1 && u2) {
                idx_type d1 = t1.get_k_l_rank(index1), d2 = t2.get_k_l_rank(index2);
                int add = t1.get_v(d1) * t2.get_v(d2);
                ret.emplace_back(i, j, add);
            }
        }
        return;
    }
    
    if (flag1 || flag2) {
        cout << (flag1 ? "c1" : "c2") << endl;
        return;
    }

    cout << "a" << endl;

    // Obtener hijos y verificar su validez.
    int children1[4] = { t1.get_child(pos1, 0), t1.get_child(pos1, 1), t1.get_child(pos1, 2), t1.get_child(pos1, 3) };
    int children2[4] = { t2.get_child(pos2, 0), t2.get_child(pos2, 1), t2.get_child(pos2, 2), t2.get_child(pos2, 3) };
    
    idx_type i1 = (i << 1);
    idx_type j1 = (j << 1);

    // Calcula las submatrices
    vector<vector<tuple<idx_type, idx_type, int>>> sub_results(8);
    auto add_and_sum = [&](int idx, int c1, int c2, int i_offset, int j_offset) {
        if (c1 != -1 || c2 != -1) {
            test_mult_test(sub_results[idx], t1, t2, c1, c2, flag1, flag2, i1 | i_offset, j1 | j_offset);
            print_vector_tuple(sub_results[idx]);
        }
    };

    // C1 = A1*B1 + A2*B3
    add_and_sum(0, children1[0], children2[0], 0ULL, 0ULL);
    add_and_sum(1, children1[1], children2[2], 0ULL, 0ULL);
    k2_tree<2> z1(sub_results[0], sub_results[0].size()), z2(sub_results[1], sub_results[1].size());
    suma(ret, z1, z2, 0, 0, false, false, 0, 0);

    // C2 = A1*B2 + A2*B4
    add_and_sum(2, children1[0], children2[1], 0ULL, 1ULL);
    add_and_sum(3, children1[1], children2[3], 0ULL, 1ULL);
    k2_tree<2> z3(sub_results[2], sub_results[2].size()), z4(sub_results[3], sub_results[3].size());
    suma(ret, z3, z4, 0, 0, false, false, 0, 0);

    // C3 = A3*B1 + A4*B3
    add_and_sum(4, children1[2], children2[0], 1ULL, 0ULL);
    add_and_sum(5, children1[3], children2[2], 1ULL, 0ULL);
    k2_tree<2> z5(sub_results[4], sub_results[4].size()), z6(sub_results[5], sub_results[5].size());
    suma(ret, z5, z6, 0, 0, false, false, 0, 0);

    // C4 = A3*B2 + A4*B4
    add_and_sum(6, children1[2], children2[1], 1ULL, 1ULL);
    add_and_sum(7, children1[3], children2[3], 1ULL, 1ULL);
    k2_tree<2> temp_z1(sub_results[6], sub_results[6].size());
    k2_tree<2> temp_z2(sub_results[7], sub_results[7].size());
    suma(ret, temp_z1, temp_z2, 0, 0, false, false, 0, 0);
}


void print_array(int a[], int n) {
    for (int i = 0; i < n; i++) {
        cout << a[i] << " ";
    }
    cout << endl;
}




bool all_children_empty(int c1[], int c2[], int n) {
    for (int k = 0; k < n; k++) {
        bool flag = c1[k] != -1 || c2[k] != -1;
        cout << "flag: " << flag << endl;
        if (flag) {
            return false;
        }
    }
    return true;
}

void get_leaf_values(k2_tree<2> &t1, k2_tree<2> &t2, int c1[], int c2[], int val1[], int val2[]) {
    for (int k = 0; k < 4; k++) {
        val1[k] = get_leaf_value(t1, c1[k]);
        val2[k] = get_leaf_value(t2, c2[k]);
    }
    print_array(val1, 4);
    print_array(val2, 4);
}


void calculate_multiplications(int val1[], int val2[], int mul[]) {
    mul[0] = val1[0] * val2[0] + val1[1] * val2[2];
    mul[1] = val1[0] * val2[1] + val1[1] * val2[3];
    mul[2] = val1[2] * val2[0] + val1[3] * val2[2];
    mul[3] = val1[2] * val2[1] + val1[3] * val2[3];
}

void add_results_to_ret(vector<tuple<idx_type, idx_type, int>> &ret, idx_type i1, idx_type j1, int mul[4]) {
    tuple<idx_type, idx_type, int> r1(i1 | 0ULL, j1 | 0ULL, mul[0]);
    tuple<idx_type, idx_type, int> r2(i1 | 0ULL, j1 | 1ULL, mul[1]);
    tuple<idx_type, idx_type, int> r3(i1 | 1ULL, j1 | 0ULL, mul[2]);
    tuple<idx_type, idx_type, int> r4(i1 | 1ULL, j1 | 1ULL, mul[3]);

    print_vector_tuple(ret);
    if (mul[0] != 0) {
        ret.push_back(r1);
    }
    if (mul[1] != 0) {
        ret.push_back(r2);
    }
    if (mul[2] != 0) {
        ret.push_back(r3);
    }
    if (mul[3] != 0) {
        ret.push_back(r4);
    }
}


// i es fila, j es columna

void mult_squared_mat(vector<tuple<idx_type,idx_type,int>> &ret, k2_tree<2> &t1, k2_tree<2> &t2, int pos1, int pos2, bool flag1, bool flag2, idx_type i, idx_type j) {
    cout << "mult_squared_mat called with pos1=" << pos1 << ", pos2=" << pos2 << ", i=" << i << ", j=" << j << endl;
    print_vector_tuple(ret);
    if (flag1 && flag2) {
        int c1[4], c2[4];
        for (int k = 0; k < 4; k++) {
            c1[k] = (pos1 != -1) ? t1.get_child(pos1, k) : -1;
            c2[k] = (pos2 != -1) ? t2.get_child(pos2, k) : -1;
        }
        print_array(c1, 4);
        print_array(c2, 4);
        cout << 1 << endl;
        if (all_children_empty(c1, c2, 4)) {
            return;
        }

        cout << 2 << endl;

        idx_type i1 = (i << 1);
        idx_type j1 = (j << 1);
        int val1[4], val2[4];
        get_leaf_values(t1, t2, c1, c2, val1, val2);
        int mul[4];
        cout << 3 << endl;
        calculate_multiplications(val1, val2, mul);
        cout << 4 << endl;
        add_results_to_ret(ret, i1, j1, mul);
        cout << 5 << endl;
    }
}

bool all_children_empty(int c[4]) {
    return all_of(c, c + 4, [](int x) { return x == -1; });
}

void process_leaf_nodes(vector<tuple<idx_type,idx_type,int>> &ret, k2_tree<2> &t1, k2_tree<2> &t2, int pos11, int pos12, int pos21, int pos22, idx_type i, idx_type j) {
    cout << "process_leaf_nodes called with pos11=" << pos11 << ", pos12=" << pos12 << ", pos21=" << pos21 << ", pos22=" << pos22 << ", i=" << i << ", j=" << j << endl;
    print_vector_tuple(ret);
    mult_squared_mat(ret, t1, t2, pos11, pos21, true, true, i, j);
    mult_squared_mat(ret, t1, t2, pos12, pos22, true, true, i, j);
}

void multiplicar_helper(vector<tuple<idx_type, idx_type, int>> &ret, k2_tree<2> &t1, k2_tree<2> &t2, int pos11, int pos12, int pos21, int pos22, bool flag1, bool flag2, idx_type i, idx_type j) {
    cout << "multiplicar_helper called with pos11=" << pos11 << ", pos12=" << pos12 << ", pos21=" << pos21 << ", pos22=" << pos22 << ", i=" << i << ", j=" << j << endl;
    print_vector_tuple(ret);
    if (pos11 == -1 || pos12 == -1 || pos21 == -1 || pos22 == -1) {
        return;
    }

    flag1 = is_leaf(pos11, t1) && is_leaf(pos12, t1);
    flag2 = is_leaf(pos21, t2) && is_leaf(pos22, t2);

    if (flag1 && flag2) {
        cout << "Processing leaf nodes at pos11=" << pos11 << ", pos12=" << pos12 << ", pos21=" << pos21 << ", pos22=" << pos22 << endl;
        process_leaf_nodes(ret, t1, t2, pos11, pos12, pos21, pos22, i, j);
    } else {
        cout << "process_non_leaf_nodes called with pos11=" << pos11 << ", pos12=" << pos12 << ", pos21=" << pos21 << ", pos22=" << pos22 << ", i=" << i << ", j=" << j << endl;
        idx_type i1 = (i << 1);
        idx_type j1 = (j << 1);

        int c1[4], c2[4], c3[4], c4[4];

        for (int k = 0; k < 4; k++) {
            c1[k] = (pos11 != -1) ? t1.get_child(pos11, k) : -1;
            c2[k] = (pos12 != -1) ? t1.get_child(pos12, k) : -1;
            c3[k] = (pos21 != -1) ? t2.get_child(pos21, k) : -1;
            c4[k] = (pos22 != -1) ? t2.get_child(pos22, k) : -1;
        }

        if (all_children_empty(c1) && all_children_empty(c2) && all_children_empty(c3) && all_children_empty(c4)) {
            return;
        }

            cout << "process_child_nodes called with i1=" << i1 << ", j1=" << j1 << endl;
        if (c1[0] != -1 || c2[1] != -1 || c3[0] != -1 || c4[2] != -1) {
            multiplicar_helper(ret, t1, t2, c1[0], c2[1], c3[0], c4[2], flag1, flag2, i1 | 0ULL, j1 | 0ULL);
        }
        if (c1[0] != -1 || c2[1] != -1 || c3[1] != -1 || c4[3] != -1) {
            multiplicar_helper(ret, t1, t2, c1[0], c2[1], c3[1], c4[3], flag1, flag2, i1 | 0ULL, j1 | 1ULL);
        }
        if (c1[2] != -1 || c2[3] != -1 || c3[0] != -1 || c4[2] != -1) {
            multiplicar_helper(ret, t1, t2, c1[2], c2[3], c3[0], c4[2], flag1, flag2, i1 | 1ULL, j1 | 0ULL);
        }
        if (c1[2] != -1 || c2[3] != -1 || c3[1] != -1 || c4[3] != -1) {
            multiplicar_helper(ret, t1, t2, c1[2], c2[3], c3[1], c4[3], flag1, flag2, i1 | 1ULL, j1 | 1ULL);
        }
    }
}

void process_non_leaf_nodes(vector<tuple<idx_type, idx_type, int>> &ret, k2_tree<2> &t1, k2_tree<2> &t2, int pos11, int pos12, int pos21, int pos22, bool flag1, bool flag2, idx_type i, idx_type j) {
    cout << "process_non_leaf_nodes called with pos11=" << pos11 << ", pos12=" << pos12 << ", pos21=" << pos21 << ", pos22=" << pos22 << ", i=" << i << ", j=" << j << endl;
    print_vector_tuple(ret);
    idx_type i1 = (i << 1);
    idx_type j1 = (j << 1);

    int c1[4], c2[4], c3[4], c4[4];

    for (int k = 0; k < 4; k++) {
        c1[k] = (pos11 != -1) ? t1.get_child(pos11, k) : -1;
        c2[k] = (pos12 != -1) ? t1.get_child(pos12, k) : -1;
        c3[k] = (pos21 != -1) ? t2.get_child(pos21, k) : -1;
        c4[k] = (pos22 != -1) ? t2.get_child(pos22, k) : -1;
    }

    if (all_children_empty(c1) && all_children_empty(c2) && all_children_empty(c3) && all_children_empty(c4)) {
        return;
    }

    cout << "process_child_nodes called with i1=" << i1 << ", j1=" << j1 << endl;
    if (c1[0] != -1 || c2[1] != -1 || c3[0] != -1 || c4[2] != -1) {
        multiplicar_helper(ret, t1, t2, c1[0], c2[1], c3[0], c4[2], flag1, flag2, i1 | 0ULL, j1 | 0ULL);
    }
    if (c1[0] != -1 || c2[1] != -1 || c3[1] != -1 || c4[3] != -1) {
        multiplicar_helper(ret, t1, t2, c1[0], c2[1], c3[1], c4[3], flag1, flag2, i1 | 0ULL, j1 | 1ULL);
    }
    if (c1[2] != -1 || c2[3] != -1 || c3[0] != -1 || c4[2] != -1) {
        multiplicar_helper(ret, t1, t2, c1[2], c2[3], c3[0], c4[2], flag1, flag2, i1 | 1ULL, j1 | 0ULL);
    }
    if (c1[2] != -1 || c2[3] != -1 || c3[1] != -1 || c4[3] != -1) {
        multiplicar_helper(ret, t1, t2, c1[2], c2[3], c3[1], c4[3], flag1, flag2, i1 | 1ULL, j1 | 1ULL);
    }
}

void multiplicar_test(vector<tuple<idx_type, idx_type, int>> &ret, k2_tree<2> &t1, k2_tree<2> &t2, int pos11, int pos12, int pos21, int pos22, bool flag1, bool flag2, idx_type i, idx_type j) {
    cout << "multiplicar_test called with pos11=" << pos11 << ", pos2=" << pos12 << ", i=" << i << ", j=" << j << ", flag1=" << flag1 << ", flag2=" << flag2 << endl;

    if (pos11 == -1 || pos12 == -1 || pos21 == -1 || pos22 == -1) {
        return;
    }

    flag1 = is_previous_leaf(pos11, t1) && is_previous_leaf(pos12, t1);
    flag2 = is_previous_leaf(pos21, t2) && is_previous_leaf(pos22, t2);

    if (flag1 && flag2) {
        cout << "Calling multiplicar_helper with leaf nodes at pos11=" << pos11 << " and pos12=" << pos12 << endl;
        multiplicar_helper(ret, t1, t2, pos11, pos12, pos21, pos22, flag1, flag2, i, j);
    } else {
        process_non_leaf_nodes(ret, t1, t2, pos11, pos12, pos21, pos22, flag1, flag2, i, j);
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

void sumaImpl(k2_tree<2>& t1, k2_tree<2>& t2){
    vector<tuple<idx_type,idx_type,int>> ret;
    ofstream out("out.txt");
    if (!out.is_open()) {
        cerr << "Error opening output file: out.txt" << endl;
        return;
    }
    k0 = clock();
    idx_type i = 0, j = 0;
    suma(ret,t1,t2,-2,-2,false,false,i,j);
    k2_tree<2> t3(ret,ret.size());
    k1 = clock();
    out << "time: " << (double(k1-k0)/CLOCKS_PER_SEC) << endl;
    for (int i = 0; i < ret.size(); i++) {
        out << get<0>(ret[i]) << " " << get<1>(ret[i]) << " " << get<2>(ret[i]) << endl;
    }
    return;
}

k2_tree<2> multiplicarImpl(k2_tree<2>& t1, k2_tree<2>& t2){
    vector<tuple<idx_type,idx_type,int>> ret;
    ofstream out("out.txt");
    if (!out.is_open()) {
        cerr << "Error opening output file: out.txt" << endl;
        return k2_tree<2>();
    }
    k0 = clock();
    idx_type i = 0, j = 0;
    multiplicar_test(ret,t1,t2,0,0,0,0,false,false,i,j);
    k2_tree<2> t3(ret,ret.size());
    k1 = clock();
    out << "time: " << (double(k1-k0)/CLOCKS_PER_SEC) << endl;
    for (int i = 0; i < ret.size(); i++) {
        out << get<0>(ret[i]) << " " << get<1>(ret[i]) << " " << get<2>(ret[i]) << endl;
    }
    return t3;
}

double _time = (double(k1-k0)/CLOCKS_PER_SEC);



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

vector<vector<int>> reconstruct_matrix(vector<tuple<idx_type,idx_type,int>> mat){
    int n = 0;
    for(int i=0; i<mat.size(); i++){
        if(get<0>(mat[i]) > n){
            n = get<0>(mat[i]);
        }
    }
    n++;
    vector<vector<int>> matrix(n,vector<int>(n,0));
    for(int i=0; i<mat.size(); i++){
        matrix[get<0>(mat[i])][get<1>(mat[i])] = get<2>(mat[i]);
    }
    return matrix;
}

void test_suma(string arch) {
    ifstream file(arch);
    if (!file.is_open()) {
        cerr << "Error opening file: " << arch << endl;
        return;
    }
    ofstream out("out.txt");
    if (!out.is_open()) {
        cerr << "Error opening output file: out.txt" << endl;
        return;
    }
    out << "Matriz1 " << "Matriz2 " << "#_elem_MAT1 " << "#_elem_MAT2" << endl;
    unsigned int r0, r1, r2, r3;
    string line;
    while (getline(file, line)) {
        // Split line by ' '
        vector<string> v;
        string s = "";
        for (int i = 0; i < line.size(); i++) {
            if (line[i] == ' ') {
                v.push_back(s);
                s = "";
            } else {
                s += line[i];
            }
        }
        v.push_back(s);
        // Create matrix from v
        vector<tuple<idx_type, idx_type, int>> m1;
        vector<tuple<idx_type, idx_type, int>> m2;
        string d = "./data/";
        ifstream file1(d + v[0]);
        if (!file1.is_open()) {
            cerr << "Error opening file: " << d + v[0] << endl;
            continue;
        }
        ifstream file2(d + v[1]);
        if (!file2.is_open()) {
            cerr << "Error opening file: " << d + v[1] << endl;
            continue;
        }
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
            m1.push_back(t);
        }
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
            m2.push_back(t);
        }
        file1.close();
        file2.close();
        // Print vector tuple
        //print_vector_tuple(m2);
        //k2_tree<2> arbol1(m1);
        //k2_tree<2> arbol2(m2);
        //vector<tuple<idx_type, idx_type, int>> ret;
        //idx_type i = 0, j = 0;
        //r0 = clock();
        //suma(ret, arbol1, arbol2, -2, -2, false, false, i, j);
        //r1 = clock();
        //double timek2 = (double(r1 - r0) / CLOCKS_PER_SEC);
        //vector<vector<int>> mat1, mat2;
        //mat1 = reconstruct_matrix(m1);
        //mat2 = reconstruct_matrix(m2);
        //r2 = clock();
        //vector<vector<int>> res = addMatrices(mat1, mat2);
        //r3 = clock();
        //double timeN = (double(r3 - r2) / CLOCKS_PER_SEC);
        out << v[0] << " " << v[1] << " " << m1.size() << " " << m2.size() << endl;
    }
    file.close();
    out.close();
}

void base_test(){
    vector<tuple<idx_type,idx_type,int>> m1 = {{0,0,1},{1,1,2},{0,3,3},{1,3,1},{2,1,8},{3,2,4}};
    vector<tuple<idx_type,idx_type,int>> m2 = {{0,2,2},{0,3,1},{1,3,1},{2,0,2},{3,3,1}}; 
    
    //k2_tree<2> arbol1(m1);
    //k2_tree<2> arbol2(m2);
    //print_vector(arbol1.get_l_values());
    //print_vector(arbol2.get_l_values());
    
    cout << "test" << endl;

    vector<tuple<idx_type,idx_type,int>> ret;

    //print_bit_vector(arbol1.get_t());
    //print_bit_vector(arbol1.get_l());
    //print_bit_vector(arbol2.get_t());
    //print_bit_vector(arbol2.get_l());

    idx_type i = 0, j = 0;

    //suma(ret,arbol1,arbol2,-2,-2,false,false,i,j);
    cout << "sumatest" << endl;
    sumatest(ret,m1,m2);
    //test_mult_test(ret,arbol1,arbol2,0,0,false,false,i,j);
    //k2_tree<2> arbol3(ret);

    //print_bit_vector(arbol3.get_t());
    //print_bit_vector(arbol3.get_l());

    //print ret
    //cout << "m1" << endl;
    //print_from_list_to_matrix(m1);
    //cout << "m2" << endl;
    //print_from_list_to_matrix(m2);
    //cout << "ret" << endl;
    print_from_list_to_matrix(ret);
    cout << "ret values" << endl;
    //print_vector(arbol3.get_l_values());

    for(int i=0; i<ret.size(); i++){
        cout << get<0>(ret[i]) << " " << get<1>(ret[i]) << " " << get<2>(ret[i]) << endl;
    }

    cout << "n=" << ret.size() << endl;
}

void generador(){
    string d = "./data/";
    int todos = 0;
    int i = 0;
    while(i < 2101){
        ifstream file(d+"m"+to_string(i)+".txt");
        if (!file.is_open()) {
            cerr << "Error opening file: " << d+"m"+to_string(i)+".txt" << endl;
            i++;
            continue;
        }
        string line;
        vector<tuple<idx_type,idx_type,int>> m1;
        while (getline(file, line)) {
            vector<string> v1;
            string s1 = "";
            for (int i = 0; i < line.size(); i++) {
                if (line[i] == ' ') {
                    v1.push_back(s1);
                    s1 = "";
                } else {
                    s1 += line[i];
                }
            }
            if (s1.size() > 0) {
                v1.push_back(s1);
            }
            if (v1.size() < 3) {
                continue;
            }
            idx_type i = static_cast<idx_type>(stoi(v1[0])), j = static_cast<idx_type>(stoi(v1[1]));
            int val = stoi(v1[2]);
            tuple<idx_type, idx_type, int> t(i, j, val);
            m1.push_back(t);
        }
        file.close();
        k2_tree<2> arbol1(m1);
        if(arbol1.get_t().size() > 0){
            todos++;
        }
        cout << "Done: " << i << endl;
        i++;
    }
    cout << "todos: " << todos << endl;

}
