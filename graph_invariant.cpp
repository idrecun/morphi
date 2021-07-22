#include <vector>

#include "utility.hpp"
#include "graph_invariant.hpp"
#include "colouring.hpp"

// Standard invariant - adjacency matrix
invariant_adjacent::invariant_adjacent(const graph& g) : g(g) {
	calculate();
}

void invariant_adjacent::calculate() { }

uint32_t invariant_adjacent::get(int v, const cell_data& W) const {
	int cnt = 0;
	for(auto w : W.vertices)
		cnt += g.adjacent(v, w);
	return cnt;
}


// Default invariant - adjacency matrix with bitvectors
invariant_bitvector::invariant_bitvector(const graph& g) : g(g) {
	calculate();
}

void invariant_bitvector::calculate() {
	adjacency_bitvector = vector< vector<uint64_t> >(g.v_count(), vector<uint64_t>((g.v_count() + 63) / 64));
	for(int i = 0; i < g.v_count(); i++)
		for(int j = 0; j < g.v_count(); j++)
			if(g.adjacent(i, j))
				adjacency_bitvector[i][j / 64] |= 1ull << (j % 64);
}

uint32_t invariant_bitvector::get(int v, const cell_data& W) const {
	int cnt = 0;
	for(auto w : W.bitvector)
		cnt += __builtin_popcountll(w.val & adjacency_bitvector[v][w.idx]);
	return cnt;
}


// Distance invariant
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


// Path count invariant
invariant_paths::invariant_paths(const graph& g, int d) : g(g) {
	this->d = d;
	calculate();
}

void invariant_paths::calculate() {
	int vertex_count = g.v_count();
	vector< vector< sequential_hash > > hashes(vertex_count, vector<sequential_hash>(vertex_count));
	path_matrix = vector< vector<uint32_t> >(vertex_count, vector<uint32_t>(vertex_count));
	for(int i = 0; i < vertex_count; i++)
		for(int j = 0; j < vertex_count; j++) {
			path_matrix[i][j] = g.adjacent(i, j);
			hashes[i][j].update(path_matrix[i][j]);
		}
	vector< vector<uint32_t> > tmp_matrix(vertex_count, vector<uint32_t>(vertex_count));

	for(int c = 1; c < d; c++) {
		tmp_matrix = vector< vector<uint32_t> >(vertex_count, vector<uint32_t>(vertex_count));
		for(int k = 0; k < vertex_count; k++)
			for(int i = 0; i < vertex_count; i++)
				if(g.adjacent(i, k))
					for(int j = 0; j < vertex_count; j++)
						tmp_matrix[i][j] += path_matrix[k][j];
		for(int i = 0; i < vertex_count; i++)
			for(int j = 0; j < vertex_count; j++) {
				path_matrix[i][j] = tmp_matrix[i][j];
				hashes[i][j].update(path_matrix[i][j]);
			}
	}

	for(int i = 0; i < vertex_count; i++)
		for(int j = 0; j < vertex_count; j++)
			path_matrix[i][j] = hashes[i][j].value();


	vector<uint32_t> sorted(vertex_count * vertex_count);
	for(int i = 0; i < vertex_count; i++)
		for(int j = 0; j < vertex_count; j++)
			sorted[i * vertex_count + j] = path_matrix[i][j];

	sort(sorted.begin(), sorted.end());

	for(int i = 0; i < vertex_count; i++)
		for(int j = 0; j < vertex_count; j++)
			path_matrix[i][j] = lower_bound(sorted.begin(), sorted.end(), path_matrix[i][j]) - sorted.begin();
}

uint32_t invariant_paths::get(int v, const cell_data& W) const {
	multiset_hash h;
	for(auto w : W.vertices)
		h.update(path_matrix[v][w]);
	return h.value();
}
