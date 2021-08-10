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
	uint64_t mask;
	for(auto w : W.bitvector)
		if((mask = w.val & adjacency_bitvector[v][w.idx]) != 0)
			cnt += __builtin_popcountll(w.val & adjacency_bitvector[v][w.idx]);
	return cnt;
}


// Distance invariant
invariant_distance::invariant_distance(const graph& g) : g(g) {
	calculate();
}

void invariant_distance::calculate() {
	int vertex_count = g.v_count();
	vector<uint32_t> distances(vertex_count);

	queue<int> q;
	uint32_t diameter = 0;
	for(int i = 0; i < 2; i++) {
		for(int v = 0; v < vertex_count; v++) {
			fill(distances.begin(), distances.end(), vertex_count);
			distances[v] = 0;

			q.push(v);
			while(!q.empty()) {
				int u = q.front();
				q.pop();

				for(int x : g.adjacency_vector()[u])
					if(distances[x] == vertex_count) {
						distances[x] = distances[u] + 1;
						q.push(x);
					}
			}

			if(i == 0)
				diameter = std::max(diameter, *max_element(distances.begin(), distances.end()));
			else
				for(int w = 0; w <= v; w++)
					distance_matrix.set(v, w, distances[w]);
		}

		if(i == 0) distance_matrix.resize(vertex_count, diameter + 1);
	}

	//distance_matrix.from_matrix(distances);
}

uint32_t invariant_distance::get(int v, const cell_data& W) const {
	multiset_hash h;
	for(auto w : W.vertices)
		h.update(distance_matrix.get(v, w));
	return h.value();
}


// Path count invariant
invariant_paths::invariant_paths(const graph& g, int d) : g(g) {
	this->d = d;
	calculate();
}

void invariant_paths::calculate() {
	int vertex_count = g.v_count();

	compressed_matrix& hashes = path_matrix;
	hashes.resize(vertex_count, 1 << 16);
	vector< vector<uint16_t> > paths(vertex_count, vector<uint16_t>(vertex_count));

	for(int i = 0; i < vertex_count; i++)
		for(int j = 0; j < vertex_count; j++) {
			paths[i][j] = g.adjacent(i, j);
			if(j <= i)
				hashes.set(i, j, sequential_hash::combine(hashes.get(i, j), g.adjacent(i, j)));
		}

	for(int c = 1; c < d; c++) {
		for(int i = 0; i < vertex_count; i++) {
			vector<int> ones;
			for(int j = 0; j < vertex_count; j++)
				if(g.adjacent(i, j))
					ones.push_back(j);
			for(int j = i; j < vertex_count; j++) {
				uint16_t s = 0;
				for(int k : ones)
					s += paths[k][j];
				paths[j][i] = s;
				hashes.set(j, i, sequential_hash::combine(hashes.get(j, i), paths[j][i]));
			}
		}
		for(int i = 0; i < vertex_count; i++)
			for(int j = 0; j < i; j++)
				paths[j][i] = paths[i][j];
	}
}

uint32_t invariant_paths::get(int v, const cell_data& W) const {
	multiset_hash h;
	for(auto w : W.vertices)
		h.update(path_matrix.get(v, w));
	return h.value();
}
