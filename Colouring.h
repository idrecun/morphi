#ifndef __COLOURING_H__
#define __COLOURING_H__

#include <iostream>
#include <vector>
#include <algorithm>

#include "Graph.h"

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
		pi = vector<int>(n);
		ip = vector<int>(n);
		for(int i = 0; i < n; i++) {
			pi[i] = i;
			ip[i] = i;
		}
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
		return vector<int>(next(pi.begin(), i), next(pi.begin(), cells[i]));
	}

	void p_swap(int i, int j) {
		swap(pi[i], pi[j]);
		swap(ip[pi[i]], ip[pi[j]]);
	}

	void individualize(int v) {
		int i = ip[v];
		while(cells[i] == -1) {
			p_swap(i, i - 1);
			i--;
		}
		cells[i + 1] = cells[i];
		cells[i] = i + 1;
	}

	int refine_cell(int c, const graph& g, const vector<int>& W) {
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
			pi[i + l] = kv[i].second;
			ip[pi[i + l]] = i + l;
			if(i > 0 && kv[i].first != kv[i - 1].first) {
				cells[c] = i + l;
				c = i + l;
			}
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
	}

	void make_equitable(const graph& g) {
		make_equitable(g, vector<int>({0}));
	}

	void make_equitable(const graph& g, int v) {
		make_equitable(g, vector<int>({ip[v]}));
	}

	uint32_t hash() const {
		uint32_t res = n;
		for(int x : pi)
			res ^= (uint32_t) x + 0x9e3779b9 + (res << 6) + (res >> 2);
		for(int i = 0; i < n; i = cells[i])
			res ^= (uint32_t) i + 0x9e3779b7 + (res << 6) + (res >> 2);
		return res;
	}

	vector<int> permutation() const {
		return pi;
	}

	friend ostream& operator<<(ostream& out, const colouring& c);

	private:
	int n;
	vector<int> pi;
	vector<int> ip;
	vector<int> cells;
};

ostream& operator<<(ostream& out, const colouring& c) {
	for(int l = 0; l < c.n; l = c.cells[l]) {
		if(l > 0)
			cout << " | ";
		for(int i = l; i < c.cells[l] - 1; i++)
			cout << c.pi[i] << ' ';
		cout << c.pi[c.cells[l] - 1];
	}
	return out;
}

#endif
