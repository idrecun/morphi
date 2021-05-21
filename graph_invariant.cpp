#include <vector>

#include "utility.hpp"
#include "graph_invariant.hpp"
#include "colouring.hpp"

invariant_adjacent::invariant_adjacent(const graph& g) : g(g) {
	calculate();
}

void invariant_adjacent::calculate() {
	adjacency_bitvector = vector< vector<uint64_t> >(g.v_count(), vector<uint64_t>((g.v_count() + 63) / 64));
	for(int i = 0; i < g.v_count(); i++)
		for(int j = 0; j < g.v_count(); j++)
			if(g.adjacent(i, j))
				adjacency_bitvector[i][j / 64] |= 1ull << (j % 64);
}

uint32_t invariant_adjacent::get(int v, const cell_data& W) const {
	int cnt = 0;
	for(auto w : W.bitvector)
		cnt += __builtin_popcountll(w.val & adjacency_bitvector[v][w.idx]);
	return cnt;
}



invariant_distance::invariant_distance(const graph& g) : g(g) {
	calculate();
}

void invariant_distance::calculate() {
	int vertex_count = g.v_count();
	distance_matrix = g.adjacency_matrix();

	for(int i = 0; i < vertex_count; i++)
		for(int j = i + 1; j < vertex_count; j++)
			distance_matrix[i][j] = distance_matrix[j][i] = vertex_count;

	queue<int> q;
	for(int v = 0; v < vertex_count; v++) {
		q.push(v);
		while(!q.empty()) {
			int u = q.front();
			q.pop();

			for(int x : g.adjacency_vector()[u])
				if(distance_matrix[v][x] == vertex_count) {
					distance_matrix[v][x] = distance_matrix[v][u] + 1;
					q.push(x);
				}
		}
	}
}

uint32_t invariant_distance::get(int v, const cell_data& W) const {
	multiset_hash h;
	for(auto w : W.vertices)
		h.update(distance_matrix[v][w]);
	return h.value();
}
