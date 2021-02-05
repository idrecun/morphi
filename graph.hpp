#ifndef __GRAPH_HPP__
#define __GRAPH_HPP__

#include <vector>
#include <queue>
#include <iostream>

#include "permutation.hpp"

using std::vector;
using std::queue;
using std::istream;

class graph {
public:
	graph();
	graph(int);

	int count(int v, const vector<int>& W) const;
	uint32_t dvector_hash(int v, const vector<int>& W) const;

	void insert(int u, int v);
	void init_distances();

	vector<bool> permuted(const permutation& pi) const;
	void relabel(const permutation& pi);

	int v_count() const;

	friend istream& operator>>(istream& in, graph& g);

private:
	int n;
	vector< vector<int> > m;
	vector< vector<int> > g;
	vector< vector<int> > d;
};

#endif
