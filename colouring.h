#ifndef __COLOURING_H__
#define __COLOURING_H__

#include <iostream>
#include <vector>
#include <algorithm>

#include "graph.h"
#include "permutation.h"

#define DEBUG true

using std::pair;
using std::vector;
using std::ostream;
using std::cout;
using std::swap;
using std::endl;

class colouring {

	public:
	colouring(int n) {
		this->n = n;
		hash = n;
		pi = permutation(n);
		cells = vector<int>(n, -1);
		cells[0] = n;
	}

	int target_cell() const {
		int i = 0;
		while(i < n && cells[i] == i + 1)
			i = cells[i];
		return i;
	}

	vector<int> cell_content(int i) const {
		if(i < 0 || i >= n || cells[i] == -1)
			return vector<int>();
		return vector<int>(next(pi.p().begin(), i), next(pi.p().begin(), cells[i]));
	}

	void individualize(int v) {
		int i = pi.i(v);
		while(cells[i] == -1) {
			pi.swap(i, i - 1);
			i--;
		}
		cells[i + 1] = cells[i];
		cells[i] = i + 1;
	}

	int refine_cell(int c, const graph& g, const vector<int>& W, bool upd_hash) {
		if(cells[c] == -1)
			return n;

		int l = c, r = cells[c];
		vector< pair<int, int> > kv(r - l);
		for(int i = 0; i < kv.size(); i++) {
			kv[i].first = g.count(pi[i + l], W);
			kv[i].second = pi[i + l];
		}
		sort(kv.begin(), kv.end());
		for(int i = 0; i < kv.size(); i++) {
			pi.set(i + l, kv[i].second);
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

	void make_equitable(const graph& g, vector<int> alpha) {
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

	void make_equitable(const graph& g) {
		make_equitable(g, vector<int>({0}));
	}

	void make_equitable(const graph& g, int v) {
		make_equitable(g, vector<int>({pi.i(v)}));
	}

	uint32_t invariant() const {
		return hash;
	}

	const permutation& p() const {
		return pi;
	}

	friend ostream& operator<<(ostream& out, const colouring& c);

private:

	void update_hash(uint32_t val) {
		hash ^= val + 0x9e3779b7 + (hash << 6) + (hash >> 2);
	}

	int n;
	uint32_t hash;
	permutation pi;
	vector<int> cells;
};

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

#endif
