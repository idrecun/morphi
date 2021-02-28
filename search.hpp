#ifndef __SEARCH_HPP__
#define __SEARCH_HPP__

#include <iostream>
#include <vector>

#include "colouring.hpp"
#include "graph.hpp"

using namespace std;

struct node {
	int length() const {
		return invariant.size();
	}

	int operator[](int i) const {
		return invariant[i];
	}

	bool is_leaf() const {
		return leaf_node;
	}

	void set_leaf(const permutation& pi) {
		leaf_node = true;
		p = pi;
	}

	void push(uint32_t inv) {
		invariant.push_back(inv);
	}

	vector<uint32_t> invariant;
	permutation p;
	int lca;
	bool leaf_node;
};

int search(const graph& g, colouring pi, bool max_path, bool aut_path);
permutation search(const graph& g);


#endif
