#ifndef __COLOURING_HPP__
#define __COLOURING_HPP__

#include <iostream>
#include <vector>
#include <algorithm>

#include "graph.hpp"
#include "permutation.hpp"
#include "utility.hpp"

using std::pair;
using std::vector;
using std::ostream;
using std::cout;
using std::swap;
using std::endl;

class colouring {
	public:
	colouring(int);

	int target_cell() const;
	vector<int> cell_content(int i) const;

	void individualize(int v);
	colouring individualized(int v) const;

	int refine_cell(int c, const graph& g, const vector<int>& W, bool use_dv, bool upd_hash);

	void make_equitable(const graph& g, vector<int> alpha, bool use_dv);
	void make_equitable(const graph& g, int v, bool use_dv);

	//vector<int> quotient(const graph& g) const;

	uint32_t invariant() const;

	const permutation& p() const;
	permutation i() const;

	friend ostream& operator<<(ostream& out, const colouring& c);

private:

	int n;
	sequential_hash inv;
	permutation pi;
	vector<int> cells;
};

#endif
