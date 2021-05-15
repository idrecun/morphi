#include <vector>

#include "utility.hpp"
#include "graph_invariant.hpp"

invariant_adjacent::invariant_adjacent(const graph& g) : g(g) {
	calculate();
}

void invariant_adjacent::calculate() { }

uint32_t invariant_adjacent::get(int v, const vector<int>& W) const {
	int cnt = 0;
	for(int w : W)
		cnt += g.adjacent(v, w);
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

uint32_t invariant_distance::get(int v, const vector<int>& W) const {
	multiset_hash h;
	for(int w : W)
		h.update(distance_matrix[v][w]);
	return h.value();
}
