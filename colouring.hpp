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

struct cell_data {
	struct bit_data{ int idx; uint64_t val; };

	vector<int> vertices;
	vector<bit_data> bitvector;
};

class colouring {
	public:
	colouring(int);

	int target_cell() const;

	cell_data cell_content(int cell) const;

	void individualize(int v);
	colouring individualized(int v) const;

	int refine_cell(int c, const graph& g, const cell_data& W);
	void make_equitable(const graph& g, int v);

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
