#ifndef K2_TREE_HPP
#define K2_TREE_HPP

#include <deque>
#include <queue>
#include <stdexcept>
#include <tuple>
#include <sdsl/bit_vectors.hpp>
#include "k2_tree_helper.hpp"
#include <sdsl/int_vector_buffer.hpp>

using namespace sdsl;


//! Namespace for the succint data structure library
//! A k^2-tree
/*! A k^2-tree is a compact tree structure to represent a web graph. The
 *  structure takes advantage of large empty areas of the adjacency matrix of
 *  the graph.
 *
 *   References
 *      [1] Brisaboa, N. R., Ladra, S., & Navarro, G. (2009, August):
 *          k2-trees for compact web graph representation. In International
 *          Symposium on String Processing and Information Retrieval
 *          (pp. 18-30). Springer Berlin Heidelberg.
 */

typedef struct{
    int i;
    int j;
    int v;
} matrix;


typedef uint64_t idx_type;

template<uint8_t k,
         typename t_bv=sdsl::bit_vector,
         typename t_rank=typename t_bv::rank_1_type,
         typename data_type= int>
class k2_tree
{
    public:
        typedef uint64_t idx_type;
        typedef uint64_t size_type;

    private:
        //! Bit array to store all the bits of the tree, except those in the
        //! last level.
        t_bv        k_t;
        //! Bit array to store the last level of the tree.
        t_bv        k_l;

        t_rank      k_t_rank;
        t_rank      k_l_rank;

        //new values.
        // parametrizar tipo de datos

        std::vector<data_type>      k_l_values;

        uint8_t     k_k;
        uint16_t    k_height;



    protected:

        void build_from_matrix(const std::vector<std::vector <int>>& matrix)
        {
            // Makes the size a power of k.
            int simulated_size = std::pow(k, k_height);
            std::vector<std::deque<bit_vector>> acc(k_height + 1);

            k2_tree_ns::_build_from_matrix<bit_vector>(matrix, k,
                    simulated_size, k_height,
                    1, 0, 0, acc);

            size_type t_size = 0;
            size_type l_size = 0;
            for (int i = 1; i < k_height; i++)
                for (auto it = acc[i].begin(); it != acc[i].end(); it++)
                    t_size += (*it).size();

            for (auto it = acc[k_height].begin(); it != acc[k_height].end(); it++)
                l_size += (*it).size();

            bit_vector k_t_(t_size, 0);
            bit_vector k_l_(l_size, 0);

            int n = 0;
            for (int j = 1; j < k_height; j++)
                for (auto it = acc[j].begin(); it != acc[j].end(); it++)
                    for (unsigned i = 0; i < (*it).size(); i++) {
                        // TODO there should be a better way to do this
                        k_t_.set_int(n, (*it).get_int(i, 1), 1);
                        n++;
                    }
            n = 0;
            for (auto it = acc[k_height].begin(); it != acc[k_height].end(); it++)
                for (unsigned i = 0; i < (*it).size(); i++) {
                    // TODO there should be a better way to do this
                    k_l_.set_int(n * 1, (*it).get_int(i, 1), 1);
                    n++;
                }

            k2_tree_ns::build_template_vector<t_bv>(k_t_, k_l_, k_t, k_l);
        }

        static idx_type interleave(uint32_t x) {
            uint64_t z = x;
            z = (z | (z << 16)) & 0x0000FFFF0000FFFF;
            z = (z | (z << 8)) & 0x00FF00FF00FF00FF;
            z = (z | (z << 4)) & 0x0F0F0F0F0F0F0F0F;
            z = (z | (z << 2)) & 0x3333333333333333;
            z = (z | (z << 1)) & 0x5555555555555555;
            return z;
        }
        static idx_type zOrder(idx_type x, idx_type y){
            // n: Numero maximo de bits a procesar

            uint32_t x_low = static_cast<uint32_t>(x);
            uint32_t y_low = static_cast<uint32_t>(y);

            return (interleave(y_low) | (interleave(x_low) << 1));        
        }


        /*! Recursive function to retrieve list of neighbors.
         *
         *  \param n Size of the submatrix in the next recursive step.
         *  \param row Row of interest in the current submatrix, this is the
         *      row corresponding the node we are looking neighbors for.
         *  \param col Column offset of the current submatrix in the global
         *      matrix.
         *  \param level Position in k_t:k_l (k_l appended to k_t) of the node
         *      or leaf being processed at this step.
         *  \param acc Accumulator to store the neighbors found.
         */
        void _neigh(size_type n, idx_type row, idx_type col, size_type level,
                    std::vector<idx_type>& acc) const
        {
            if (level >= k_t.size()) { // Last level
                if (k_l[level - k_t.size()] == 1)
                    acc.push_back(col);
                return;
            }

            if (k_t[level] == 1) {
                idx_type y = k_t_rank(level + 1) * std::pow(k_k, 2) +
                             k_k * std::floor(row/static_cast<double>(n));
                for (unsigned j = 0; j < k_k; j++)
                    _neigh(n/k_k, row % n, col + n * j, y + j, acc);
            }
        }

        /*! Recursive function to retrieve list of reverse neighbors.
         *
         *  \param n Size of the submatrix in the next recursive step.
         *  \param row Row offset of the current submatrix in the global matrix.
         *  \param col Column of interest in the current submatrix, this is the
         *      column corresponding the node we are looking reverse neighbors
         *      for.
         *  \param level Position in k_t:k_l (k_l appended to k_t) of the node
         *      or leaf being processed at this step.
         *  \param acc Accumulator to store the neighbors found.
         */
        void _reverse_neigh(size_type n, idx_type row, idx_type col,
                            size_type level, std::vector<idx_type>& acc) const
        {
            if (level >= k_t.size()) { // Last level
                if (k_l[level - k_t.size()] == 1) {
                    acc.push_back(row);
                }
                return;
            }

            if (k_t[level] == 1) {
                idx_type y = k_t_rank(level + 1) * std::pow(k_k, 2) +
                             std::floor(col/static_cast<double>(n));
                for (unsigned j = 0; j < k_k; j++)
                    _reverse_neigh(n/k_k, row + n * j, col % n,
                                   y + j * k_k, acc);
            }
        }

        void build_from_edges(std::vector<std::tuple<idx_type, idx_type>>& edges, const size_type size) {
            typedef std::tuple<idx_type, idx_type, size_type, idx_type, idx_type> t_part_tuple;
            k_k = k;
            k_height = std::ceil(std::log(size) / std::log(k_k));
            k_height = k_height > 1 ? k_height : 1; // If size == 0
            size_type k_2 = std::pow(k_k, 2);
            bit_vector k_t_ = bit_vector(k_2 * k_height * edges.size(), 0);
            bit_vector k_l_;

            std::queue<t_part_tuple> q;
            idx_type t = 0, last_level = 0;
            idx_type i, j, r_0, c_0, it, c, r;
            size_type l = std::pow(k_k, k_height - 1);
            std::vector<idx_type> pos_by_chunk(k_2 + 1, 0);

            q.push(t_part_tuple(0, edges.size(), l, 0, 0));

            while (!q.empty()) {
                std::vector<idx_type> amount_by_chunk(k_2, 0);
                std::tie(i, j, l, r_0, c_0) = q.front();
                q.pop();

                // Get size for each chunk
                for (it = i; it < j; it++) {
                    auto idx = k2_tree_ns::get_chunk_idx(
                    std::get<0>(edges[it]), std::get<1>(edges[it]),
                    c_0, r_0, l, k_k);
                    //std::cout << "Edge: (" << std::get<0>(edges[it]) << ", " << std::get<1>(edges[it]) << ") -> Chunk Index: " << idx << std::endl;
                    amount_by_chunk[idx] += 1;
                }

                if (l == 1) {
                    if (last_level == 0) {
                        last_level = t;
                        k_l_ = bit_vector(k_t_.size() - last_level, 0);
                        k_t_.resize(last_level);
                        last_level = 1; // if t was 0
                        t = 0; // Restart counter as we're storing at k_l_ now.
                    }
                    for (it = 0; it < k_2; it++, t++) {
                        if (amount_by_chunk[it] != 0) {
                            k_l_[t] = 1;
                            //std::cout << "Setting k_l_[" << t << "] = 1 (Leaf level)" << std::endl;
                        }
                    }
                    // At l == 1 we do not put new elements at the queue.
                    continue;
                }

                // Set starting position in the vector for each chunk
                pos_by_chunk[0] = i;
                for (it = 1; it < k_2; it++) {
                    pos_by_chunk[it] = pos_by_chunk[it - 1] + amount_by_chunk[it - 1];
                }
                pos_by_chunk[k_2] = j;

                // Push to the queue every non-zero elements chunk
                for (it = 0; it < k_2; it++, t++) {
                    if (amount_by_chunk[it] != 0) {
                        r = it / k_k;
                        c = it % k_k;
                        k_t_[t] = 1;
                        q.push(t_part_tuple(pos_by_chunk[it],
                                            pos_by_chunk[it + 1],
                                            l / k_k,
                                            r_0 + r * l,
                                            c_0 + c * l));
                        //std::cout << "Pushed chunk (" << r << ", " << c << ") to queue, setting k_t_[" << t << "] = 1" << std::endl;
                    }
                }

                // Sort edges' vector
                for (unsigned ch = 0; ch < k_2; ch++) {
                    idx_type be = ch == 0 ? i : pos_by_chunk[ch - 1];
                    for (it = pos_by_chunk[ch]; it < be + amount_by_chunk[ch];) {
                        idx_type chunk = k2_tree_ns::get_chunk_idx(
                            std::get<0>(edges[it]), std::get<1>(edges[it]),
                            c_0, r_0, l, k_k);

                        if (pos_by_chunk[chunk] != it) {
                            std::iter_swap(edges.begin() + it,
                                        edges.begin() + pos_by_chunk[chunk]);
                        } else {
                            it++;
                        }
                        pos_by_chunk[chunk]++;
                    }
                }
            }

            k_l_.resize(t);
            k2_tree_ns::build_template_vector<t_bv>(k_t_, k_l_, k_t, k_l);
            k_t_rank = t_rank(&k_t);
            k_l_rank = t_rank(&k_l);
        }

        void SelectionSort(std::vector<std::tuple<idx_type, idx_type,int>>& array){
            int i, j, minIndex;
            std::tuple<idx_type, idx_type, int> tmp;
            for(i=0; i<array.size()-1; i++){
                minIndex = i;
                for(j=i+1; j<array.size(); j++){
                    idx_type i1, i2, j1, j2;
                    i1 = std::get<0>(array[j]);
                    i2 = std::get<1>(array[minIndex]);
                    j1 = std::get<0>(array[j]);
                    j2 = std::get<1>(array[minIndex]);
                    if(zOrder(i1,j1) < zOrder(i2,j2)){
                        minIndex = j;
                    }
                }
                if(minIndex != i){
                    tmp = array[i];
                    array[i] = array[minIndex];
                    array[minIndex] = tmp;
                }
            }
        }
    public:

        k2_tree() = default;

        //! Constructor
        /*! This constructos takes the graph adjacency matrix.
         *  The time complexity for this constructor is linear in the matrix
         *  size
         *  \param matrix Adjacency matrix of the graph. It must be a binary
         *      square matrix.
         */
        k2_tree(const std::vector<std::vector<int>> &matrix)
        {
            if (matrix.size() < 1) {
                throw std::logic_error("Matrix has no elements");
            }
            std::vector<bit_vector> t;
            k_k = k;
            if (matrix.size() < k_k)
                k_height = 1;
            else // height = log_k n
                k_height = std::ceil(std::log(matrix.size())/std::log(k_k));

            build_from_matrix(matrix);

            k_t_rank = t_rank(&k_t);
        }

        //! Constructor
        /*! This constructos takes a vector of edges describing the graph
         *  and the graph size. And takes linear time over the amount of
         *  edges to build the k_2 representation.
         *  \param edges A vector with all the edges of the graph, it can
         *               not be empty.
         *  \param size Size of the graph, all the nodes in edges must be
         *              within 0 and size ([0, size[).
         */
        k2_tree(std::vector<std::tuple<idx_type, idx_type, int>>& array,
                size_type size=0)
        {

            //construir un vector de pares
            std::vector<std::tuple<idx_type, idx_type>> edges;

            for(int i=0; i<array.size(); i++){
                idx_type a,b;
                a = std::get<0>(array[i]);
                b = std::get<1>(array[i]);
                std::tuple<idx_type, idx_type> t = std::make_tuple(a,b);
                edges.push_back(t);
            }

            if(size==0){
                size_type max = 0;
                for(auto v : edges){
                    max = std::max(static_cast<size_type>(std::get<0>(v)), max);
                    max = std::max(static_cast<size_type>(std::get<1>(v)), max);
                }
                size = max + 1;
            }

            assert(size > 0);
            assert(edges.size() > 0);

            build_from_edges(edges, size);

            sort(array.begin(),array.end(), new_sort);

            //SelectionSort(array);

            for(int i=0; i<array.size(); i++){
                k_l_values.push_back(std::get<2>(array[i]));
            }

        }

        static bool new_sort(std::tuple<idx_type,idx_type,data_type> a, std::tuple<idx_type,idx_type,data_type> b){
            idx_type i1, i2, j1, j2;
            i1 = std::get<0>(a);
            i2 = std::get<0>(b);
            j1 = std::get<1>(a);
            j2 = std::get<1>(b);
            return zOrder(i1,j1) < zOrder(i2,j2);            
        }

        
        //check filas por columnas en multiplicacion
        //arreglar suma y constructor

        //! Constructor
        /*! This constructos expects a filename prefix. Two serialized
         *  int_vectors have to be present at filename.x and filename.y.
         *  Each pair x,y describes an edge of the graph, from the node x
         *  to the node y.
         *  \param filename String with the prefix of the files filename.x,
         *					filename.y each of them containing a serialized
         *					int_vector<>.
         *  \param size Size of the graph, all the nodes in the edges defined
         *				by the files must be within 0 and size ([0, size[). If
         *				size==0, the size will be taken as the max node
         *				in the edges.
         */
        k2_tree(std::string filename, size_type size=0)
        {
			sdsl::int_vector_buffer<> buf_x(filename + ".x", std::ios::in);
			sdsl::int_vector_buffer<> buf_y(filename + ".y", std::ios::in);

			assert(buf_x.size() == buf_y.size());
			assert(buf_x.size() > 0);

			std::vector<std::tuple<idx_type, idx_type>>edges;
			edges.reserve(buf_x.size());

			if(size==0) {
				size_type max = 0;
				for(auto v : buf_x)
					max = std::max(static_cast<size_type>(v), max);
				for(auto v : buf_y)
					max = std::max(static_cast<size_type>(v), max);
				size = max + 1;
			}

			for(uint64_t i = 0; i < buf_x.size(); i++)
				edges.push_back(
						std::tuple<idx_type, idx_type> {buf_x[i], buf_y[i]});

			build_from_edges(edges, size);
		}


        k2_tree(k2_tree& tr)
        {
            *this = tr;
        }

        k2_tree(k2_tree&& tr)
        {
            *this = std::move(tr);
        }
        
        //! Move assignment operator
        k2_tree& operator=(k2_tree&& tr)
        {
            if (this != &tr) {
                k_t = std::move(tr.k_t);
                k_l = std::move(tr.k_l);
                k_k = std::move(tr.k_k);
                k_height = std::move(tr.k_height);
                k_t_rank = std::move(tr.k_t_rank);
                k_t_rank.set_vector(&k_t);
            }
            return *this;
        }

        //! Assignment operator
        k2_tree& operator=(k2_tree& tr)
        {
            if (this != &tr) {
                k_t = tr.k_t;
                k_l = tr.k_l;
                k_t_rank = tr.k_t_rank;
                k_t_rank.set_vector(&k_t);
                k_k = tr.k_k;
                k_height = tr.k_height;
            }
            return *this;
        }
        
        //uint64_t
        data_type get_v(idx_type v){
            return k_l_values[v];
        }

        //! Swap operator
        void swap(k2_tree& tr)
        {
            if (this != &tr) {
                std::swap(k_t, tr.k_t);
                std::swap(k_l, tr.k_l);
                util::swap_support(k_t_rank, tr.k_t_rank, &k_t, &(tr.k_t));
                std::swap(k_k, tr.k_k);
                std::swap(k_height, tr.k_height);
            }
        }

        //! Equal operator
        bool operator==(const k2_tree& tr) const
        {
            // TODO check the rank support equality?
            if (k_k != tr.k_k || k_height != tr.k_height)
                return false;
            if (k_t.size() != tr.k_t.size() || k_l.size() != tr.k_l.size())
                return false;
            for (unsigned i = 0; i < k_t.size(); i++)
                if (k_t[i] != tr.k_t[i])
                    return false;
            for (unsigned i = 0; i < k_l.size(); i++)
                if (k_l[i] != tr.k_l[i])
                    return false;
            return true;
        }

        t_bv get_t()
        {
            return k_t;
        }

        t_bv get_l()
        {
            return k_l;
        }

        std::vector<data_type> get_l_values(){
            return k_l_values;
        }

        
        bool comparar(matrix m1, matrix m2){
            return zOrder(m1.i,m1.j) < zOrder(m2.i,m2.j);
        }

        void new_array(matrix* m, int n){

            sort(m, m+n, comparar);

            for(int i=0; i<n; i++){
                k_l_values.push_back(m[i].v);
            }
            
        }

        //! Indicates whether node j is adjacent to node i or not.
        /*!
         *  \param i Node i.
         *  \param j Node j.
         *  \returns true if there is an edge going from node i to node j,
         *           false otherwise.
         */
        bool adj(idx_type i, idx_type j) const
        {
            if (k_t.size() == 0 && k_l.size() == 0)
                return false;
            size_type n = std::pow(k_k, k_height - 1);
            size_type k_2 = std::pow(k_k, 2);
            idx_type col, row;

            // This is duplicated to avoid an extra if at the loop. As idx_type
            // is unsigned and rank has an offset of one, is not possible to run
            // k_t_rank with zero as parameter at the first iteration.
            row = std::floor(i/static_cast<double>(n));
            col = std::floor(j/static_cast<double>(n));
            i = i % n;
            j = j % n;
            idx_type level = k_k * row + col;
            n = n/k_k;

            while (level < k_t.size()) {
                if (k_t[level] == 0)
                    return false;
                row = std::floor(i/static_cast<double>(n));
                col = std::floor(j/static_cast<double>(n));
                i = i % n;
                j = j % n;
                level = k_t_rank(level + 1) * k_2 + k_k * row + col;
                n = n/k_k;
            }

            return k_l[level - k_t.size()] == 1;
        }

        int size(){
            return k_t.size() + k_l.size();
        }

        int get_index(){
            //returns current index of k_l
            return k_l.size();
        }

        int get_k_k(){
            return k_k;
        }



        // function at
        int at(int i){
            if(i < k_t.size()){
                return k_t[i];
            }else{
                return k_l[i - k_t.size()];
            }
        }

        //resize function
        void resize(int n){
            k_t.resize(n);
            k_l.resize(n);
        }

        // get k_t_rank

        int get_k_t_rank(int i){
            return k_t_rank(i);
        }

        int get_k_l_rank(int i){
            return k_l_rank(i);
        }

        int get_child(idx_type a, idx_type i) {
            // Check that the current index is within the range of the bit vectors
            if (a < 0 || i < 0 || a >= k_t.size() + k_l.size()) {
                //std::cout << "Error: Indices out of range" << std::endl;
                return -1;  // Return std::nullopt if there is an out-of-range index
            }
        
            // Si el índice está dentro del rango de `k_t`
            if (a < k_t.size()) {
                if ((a + i) >= k_t.size() + k_l.size()) {
                    std::cout << "Error: k_t index out of range" << std::endl;
                    return -1;  // Avoid accessing out of bounds
                }
                // Verificar si el bit actual es 1
                if (k_t[a] == 1) {
                    // Calcular el hijo utilizando `rank`
                    idx_type rank_val = k_t_rank(a + 1);
                    if (rank_val * k_k * k_k + i >= k_t.size() + k_l.size()) {
                        std::cout << "Error: Calculated rank out of range" << std::endl;
                        return -1;  // Avoid accessing out of bounds
                    }
                    return rank_val * k_k * k_k + i;  // Retornar el índice del hijo
                } else {
                    return -1;  // No hay hijo, retornar std::nullopt
                }
            }
        
            int leaf_index = a - k_t.size();
            if (leaf_index < k_l.size() && leaf_index >= 0) {
                // El valor en `k_l` es 1, retornar el índice `i`
                return i;
            }
        
            // Retornar std::nullopt si no se cumple ninguna de las condiciones anteriores
            return -1;
        }

        //! Returns a list of neighbors of node i.
        /*!
         *  \param i Node to get neighbors from.
         *  \returns A list of neighbors of node i.
         */
        std::vector<idx_type> neigh(idx_type i) const
        {
            std::vector<idx_type> acc{};
            if (k_l.size() == 0 && k_t.size() == 0)
                return acc;
			// n = k^h / k
			// k^h - dimension n of matrix nxn
			// /k  - to calculate div only once and not for for all parameter again, always (n/k)
            size_type n =
                static_cast<size_type>(std::pow(k_k, k_height)) / k_k;
			// y = k * i/n
            idx_type y = k_k * std::floor(i/static_cast<double>(n));
            for (unsigned j = 0; j < k_k; j++)
                _neigh(n/k_k, i % n, n * j, y + j, acc);
            return acc;
        }

        //! Returns a list of reverse neighbors of node i.
        /*!
         *  \param i Node to get reverse neighbors from.
         *  \returns A list of reverse neighbors of node i.
         */
        std::vector<idx_type> reverse_neigh(idx_type i) const
        {
            std::vector<idx_type> acc{};
            if (k_l.size() == 0 && k_t.size() == 0)
                return acc;
            // Size of the first square division
            size_type n =
                static_cast<size_type>(std::pow(k_k, k_height)) / k_k;
            idx_type y = std::floor(i/static_cast<double>(n));
            for (unsigned j = 0; j < k_k; j++)
                _reverse_neigh(n/k_k, n * j, i % n, y + j * k_k, acc);

            return acc;
        }

        std::vector<std::pair<idx_type,idx_type>> range(
			idx_type row1, idx_type row2,
			idx_type col1, idx_type col2
		) const {
			std::vector<std::pair<idx_type,idx_type>> res;

			size_type n = static_cast<size_type>(std::pow(k_k, k_height)) / k_k;
			struct state{
				idx_type n, row1, row2, col1, col2, dr, dc, z;
				state(idx_type n, idx_type row1, idx_type row2, idx_type col1, idx_type col2,
					  idx_type dr, idx_type dc, idx_type z)
					: n(n), row1(row1), row2(row2), col1(col1), col2(col2), dr(dr), dc(dc), z(z) {}
			};
			std::vector<state> states;
			states.reserve(k_height); // minimum
			states.emplace_back(n, row1, row2, col1, col2, 0, 0, std::numeric_limits<idx_type>::max());

			while(!states.empty()){
				state s = states.back();
				states.pop_back();

				//TODO: peel first loop where z==-1 atm
				if(s.z!=std::numeric_limits<idx_type>::max() && s.z >= k_t.size()){ // Last level
					if(k_l[s.z - k_t.size()] == 1){
						res.emplace_back(s.dr, s.dc);
					}
				}else if(s.z==std::numeric_limits<idx_type>::max() || k_t[s.z]==1){

					auto y = k_t_rank(s.z+1) * k_k * k_k;

					for(idx_type i=s.row1/s.n; i<=s.row2/s.n; ++i){
						idx_type row1new, row2new;
						//TODO: loop peeling, first iteration and last iteration special
						if(i==s.row1/s.n) row1new = s.row1 % s.n; else row1new = 0;
						if(i==s.row2/s.n) row2new = s.row2 % s.n; else row2new = s.n - 1;

						for(idx_type j=s.col1/s.n; j<=s.col2/s.n; ++j){
							idx_type col1new, col2new;
							//TODO: loop peeling, first iteration and last iteration special
							if(j==s.col1/s.n) col1new = s.col1 % s.n; else col1new = 0;
							if(j==s.col2/s.n) col2new = s.col2 % s.n; else col2new = s.n - 1;

							states.emplace_back(s.n/k_k, row1new, row2new, col1new, col2new,
												s.dr + s.n*i, s.dc + s.n*j, y + k_k*i+j);
						}
					}
				}
			}

			return res;
        };

        //! Serialize to a stream
        /*! Serialize the k2_tree data structure
         *  \param out Outstream to write the k2_tree.
         *  \param v
         *  \param string_name
         *  \returns The number of written bytes.
         */
        size_type serialize(std::ostream& out, structure_tree_node* v=nullptr,
                            std::string name="") const
        {
            sdsl::structure_tree_node* child = structure_tree::add_child(
                                             v, name, util::class_name(*this));
            size_type written_bytes = 0;

            written_bytes += k_t.serialize(out, child, "t");
            written_bytes += k_l.serialize(out, child, "l");
            written_bytes += k_t_rank.serialize(out, child, "t_rank");
            written_bytes += write_member(k_k, out, child, "k");
            written_bytes += write_member(k_height, out, child, "height");
            structure_tree::add_size(child, written_bytes);
            return written_bytes;
        }


        //! Load from istream
        /*! Serialize the k2_tree from the given istream.
         *  \param istream Stream to load the k2_tree from.
         */
        void load(std::istream& in)
        {
            k_t.load(in);
            k_l.load(in);
            k_t_rank.load(in);
            k_t_rank.set_vector(&k_t);
            read_member(k_k, in);
            read_member(k_height, in);
        }


};



#endif