#include <iostream>
#include <vector>
#include <algorithm>

#include "colouring.hpp"
#include "graph.hpp"
#include "permutation.hpp"

using std::pair;
using std::vector;
using std::ostream;
using std::cout;
using std::swap;
using std::endl;

colouring::colouring(int n) {
	this->n = n;
	inv = sequential_hash();
	pi = permutation(n);
	cells = vector<int>(n, -1);
	cells[0] = n;
}

cell_data colouring::cell_content(int cell) const {
	cell_data W;
	if(cell < 0 || cell >= n || cells[cell] == -1)
		return W;

	W.vertices = vector<int>(next(pi.p().begin(), cell), next(pi.p().begin(), cells[cell]));

	for(int w : W.vertices) {
		int idx = w / 64;
		if(W.bitvector.empty() || W.bitvector.back().idx != idx)
			W.bitvector.push_back({idx, 0ull});
		W.bitvector.back().val |= (1ull << (w % 64));
	}

	return W;
}

int colouring::target_cell() const {
	int i = 0;
	while(i < n && cells[i] == i + 1)
		i = cells[i];
	return i;
}

void colouring::individualize(int v) {
	int i = pi.i(v);
	while(cells[i] == -1) {
		pi.swap(i, i - 1);
		i--;
	}
	cells[i + 1] = cells[i];
	cells[i] = i + 1;
}

colouring colouring::individualized(int v) const {
	colouring ret = *this;
	ret.individualize(v);
	return ret;
}

int colouring::refine_cell(int c, const graph& g, const cell_data& W) {
	if(cells[c] == -1)
		return n;
	if(cells[c] == c + 1)
		return c;

	int l = c, r = cells[c];
	vector< pair<int, int> > kv(r - l);
	for(int i = 0; i < kv.size(); i++)
		kv[i] = { g.get_invariant(pi[i + l], W), pi[i + l] };
	sort(kv.begin(), kv.end());
	for(int i = 0; i < kv.size(); i++) {
		pi[i + l] = kv[i].second;
		if(i > 0 && kv[i].first != kv[i - 1].first) {
			cells[c] = i + l;
			c = i + l;
		}
		//inv.update(kv[i].first);
	}
	cells[c] = r;

	int max_cell = l;
	for(int i = l; i < r; i = cells[i])
		if(cells[i] - i > cells[max_cell] - max_cell)
			max_cell = i;

	return max_cell;
}

void colouring::make_equitable(const graph& g, int v) {

	// Store colouring for invariant calculation ...
	vector<bool> input_colouring(n);
	for(int i = 0; i != n; i = cells[i])
		input_colouring[i] = true;

	// ... and undo individualization
	if(v != -1)
		input_colouring[pi.i(v) + 1] = false; 

	vector<int> alpha(1, v == -1 ? 0 : pi.i(v));
	vector<bool> alpha_set(n);
	for(int v : alpha)
		alpha_set[v] = true;

	while(!alpha.empty()) {
		cell_data W = cell_content(alpha.back());

		alpha_set[alpha.back()] = false;
		alpha.pop_back();

		for(int l = 0, r; l < n; l = r) {
			r = cells[l];
			int skip_cell = refine_cell(l, g, W);

			if(alpha_set[l])
				skip_cell = l;

			for(int i = l; i < r; i = cells[i])
				if(i != skip_cell) {
					alpha_set[i] = true;
					alpha.push_back(i);
				}
		}
	}

	// Calculate invariant
	vector<int> new_cells;
	for(int i = 0; i != n; i = cells[i])
		if(!input_colouring[i] || (cells[i] < n && !input_colouring[cells[i]]))
			new_cells.push_back(i);

	for(int i = 0; i < new_cells.size(); i++) {
		int cell = new_cells[i];
		inv.update(cell);
		inv.update(cells[cell]);
		cell_data W = cell_content(cell);
		for(int j = 0; j <= i; j++) {
			int cell2 = new_cells[j];
			inv.update(g.get_invariant(pi[cell2], W));
		}
	}
}

uint32_t colouring::invariant() const {
	return inv.value();
}

const permutation& colouring::p() const {
	return pi;
}

permutation colouring::i() const {
	return ~pi;
}

ostream& operator<<(ostream& out, const colouring& c) {
	for(int l = 0; l < c.n; l = c.cells[l]) {
		if(l > 0)
			out << " | ";
		for(int i = l; i < c.cells[l] - 1; i++)
			out << c.pi[i] << ' ';
		out << c.pi[c.cells[l] - 1];
	}
	return out;
}
