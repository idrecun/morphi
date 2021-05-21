#include <vector>
#include <queue>
#include <algorithm>
#include <string>

#include "graph.hpp"
#include "graph_invariant.hpp"
#include "permutation.hpp"
#include "utility.hpp"

using std::vector;
using std::queue;
using std::string;
using std::pair;
using std::unique_ptr;
using std::make_unique;
using std::move;

graph::graph() {
	vertex_count = 0;
}

graph::graph(int n) {
	this->vertex_count = n;
	adj_matrix = vector< vector<int> >(n, vector<int>(n, 0));
	adj_vector = vector< vector<int> >(n);
}

void graph::set_invariant(unique_ptr<graph_invariant> new_invariant) {
	this->invariant.reset();
	this->invariant = move(new_invariant);
}

uint32_t graph::get_invariant(int v, const cell_data& W) const {
	return invariant->get(v, W);
}

void graph::insert(int u, int v) {
	if(adj_matrix[u][v] == 0) {
		adj_matrix[u][v] = adj_matrix[v][u] = 1;
		adj_vector[u].push_back(v);
		adj_vector[v].push_back(u);
	}
}

vector<bool> graph::permuted(const permutation& pi) const {
	vector<bool> ret(vertex_count * (vertex_count - 1) / 2);
	int i = 0, j = 1;
	for(int k = 0; k < ret.size(); k++) {
		ret[k] = adj_matrix[pi.i(i)][pi.i(j)];
		if(++j == vertex_count) {
			i++;
			j = i + 1;
		}
	}
	return ret;
}

void graph::relabel(const permutation& pi) {
	vector< pair<int, int> > edges;
	for(int i = 0; i < vertex_count; i++)
		adj_vector[i].clear();
	for(int i = 0; i < vertex_count; i++)
		for(int j = i + 1; j < vertex_count; j++) {
			if(adj_matrix[i][j])
				edges.push_back({pi[i], pi[j]});
			adj_matrix[i][j] = adj_matrix[j][i] = 0;
		}
	for(auto e : edges)
		insert(e.first, e.second);

	if(invariant)
		invariant->calculate();
}

bool graph::less(const permutation& a, const permutation& b) const {
	for(int i = 0; i < vertex_count; i++)
		for(int j = i + 1; j < vertex_count; j++) {
			int aij = adj_matrix[a.i(i)][a.i(j)];
			int bij = adj_matrix[b.i(i)][b.i(j)];
			if(aij < bij)
				return true;
			else if(aij > bij)
				return false;
		}	
	return false;
}

bool graph::is_aut(const permutation& a) const {
	for(int i = 0; i < vertex_count; i++)
		for(int j = i + 1; j < vertex_count; j++)
			if(adj_matrix[i][j] != adj_matrix[a.i(i)][a.i(j)])
				return false;
	return true;
}

int graph::v_count() const {
	return vertex_count;
}

bool graph::adjacent(int u, int v) const {
	return adj_matrix[u][v];
}

const vector< vector<int> >& graph::adjacency_matrix() const {
	return adj_matrix;
}

const vector< vector<int> >& graph::adjacency_vector() const {
	return adj_vector;
}

istream& operator>>(istream& in, graph& g) {
	string s;
	int v, e, a, b;
	in >> s; in >> s;
	in >> v >> e;
	
	g = graph(v);

	while(e--) {
		in >> s >> a >> b;
		g.insert(a - 1, b - 1);
	}

	return in;
}
