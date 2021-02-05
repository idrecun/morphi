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
	hash = n;
	pi = permutation(n);
	cells = vector<int>(n, -1);
	cells[0] = n;
}

int colouring::target_cell() const {
	int i = 0;
	while(i < n && cells[i] == i + 1)
		i = cells[i];
	return i;
}

vector<int> colouring::cell_content(int i) const {
	if(i < 0 || i >= n || cells[i] == -1)
		return vector<int>();
	return vector<int>(next(pi.p().begin(), i), next(pi.p().begin(), cells[i]));
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

int colouring::refine_cell(int c, const graph& g, const vector<int>& W, bool upd_hash, bool use_dv = false) {
	if(cells[c] == -1)
		return n;

	int l = c, r = cells[c];
	vector< pair<int, int> > kv(r - l);
	for(int i = 0; i < kv.size(); i++) {
		if(use_dv)
			kv[i].first = g.dvector_hash(pi[i + l], W);
		else
			kv[i].first = g.count(pi[i + l], W);
		kv[i].second = pi[i + l];
	}
	sort(kv.begin(), kv.end());
	for(int i = 0; i < kv.size(); i++) {
		pi[i + l] = kv[i].second;
		if(i > 0 && kv[i].first != kv[i - 1].first) {
			cells[c] = i + l;
			c = i + l;
		}
		if(upd_hash)
			update_hash((uint32_t)kv[i].first);
	}
	cells[c] = r;

	int max_cell = l;
	for(int i = l; i < r; i = cells[i])
		if(cells[i] - i > cells[max_cell] - max_cell)
			max_cell = i;

	return max_cell;
}

/*vector<int> quotient(const graph& g) const {
	int k = count_if(cells.begin(), cells.end(), [](int c) { return c != -1; });
	vector<int> q(k * (k - 1) / 2);

}*/

void colouring::make_equitable(const graph& g, vector<int> alpha) {
	vector<bool> alpha_set(n);
	for(int v : alpha)
		alpha_set[v] = true;

	while(!alpha.empty()) {
		vector<int> W = cell_content(alpha.back());
		alpha_set[alpha.back()] = false;
		alpha.pop_back();


		for(int l = 0, r; l < n; l = r) {
			r = cells[l];
			int skip_cell = refine_cell(l, g, W, true);

			if(alpha_set[l])
				skip_cell = l;

			for(int i = l; i < r; i = cells[i])
				if(i != skip_cell) {
					alpha_set[i] = true;
					alpha.push_back(i);
				}
		}
	}

	hash = n;
	for(int i = 0; i != n; i = cells[i])
		update_hash((uint32_t) i);
}

void colouring::make_equitable(const graph& g) {
	make_equitable(g, vector<int>({0}));
}

void colouring::make_equitable(const graph& g, int v) {
	make_equitable(g, vector<int>({pi.i(v)}));
}

uint32_t colouring::invariant() const {
	return hash;
}

const permutation& colouring::p() const {
	return pi;
}

permutation colouring::i() const {
	return ~pi;
}

void colouring::update_hash(uint32_t val) {
	hash ^= val + 0x9e3779b7 + (hash << 6) + (hash >> 2);
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
