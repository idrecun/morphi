#ifndef __GRAPH_HPP__
#define __GRAPH_HPP__

#include <vector>
#include <queue>
#include <iostream>
#include <memory>

#include "permutation.hpp"

using std::vector;
using std::queue;
using std::istream;
using std::unique_ptr;

class graph_invariant;
struct cell_data;

class graph {
public:
	graph();
	graph(int);

	void set_invariant(unique_ptr<graph_invariant> invariant);
	uint32_t get_invariant(int v, const cell_data& W) const;

	void insert(int u, int v);

	vector<bool> permuted(const permutation& pi) const;
	void relabel(const permutation& pi);

	bool less(const permutation& a, const permutation& b) const;
	bool is_aut(const permutation& a) const;

	int v_count() const;
	bool adjacent(int u, int v) const;
	const vector< vector<bool> >& adjacency_matrix() const;
	const vector< vector<int> >& adjacency_vector() const;

	friend istream& operator>>(istream& in, graph& g);

private:
	int vertex_count;
	vector< vector<bool> > adj_matrix;
	vector< vector<int> > adj_vector;
	unique_ptr<graph_invariant> invariant;
};

#endif
