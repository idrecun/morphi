#include <iostream>
#include <vector>

#include "colouring.h"
#include "graph.h"

#define DEBUG true

using namespace std;

vector<uint32_t> max_phi;
vector<int> max_perm;

vector<uint32_t> fst_phi;
vector<int> fst_perm;

void dfs(const graph& g, colouring pi, int v, int level) {
	pi.make_equitable(g, v);

	//obrada
	uint32_t chash = pi.invariant();
	if(max_phi.size() > level && chash < max_phi[level]) {
		cout << string(level, '\t') << "CUT\n";
		return;
	}
	if(max_phi.size() > level && max_phi[level] < chash)
		max_phi.resize(level);
	if(max_phi.size() <= level) {
		max_phi.push_back(chash);
		max_perm.clear();
	}

	cout << string(level, '\t') << pi << '\n';


	vector<int> cell = pi.cell_content(pi.target_cell());
	for(int v : cell) {
		colouring pi_v = pi;
		pi_v.individualize(v);
		dfs(g, pi_v, v, level + 1);
	}


	if(cell.empty()) {
		vector<bool> leaf_graph = g.permute(pi.permutation());
		if(max_perm.empty() || leaf_graph > g.permute(max_perm))
			max_perm = pi.permutation();
	}
}

int main() {
	int n, m;
	cin >> n >> m;

	graph g(n);
	for(int i = 0; i < m; i++) {
		int u, v;
		cin >> u >> v;
		g.insert(u, v);
	}

	colouring pi(n);

	//pi.make_equitable(g);
	//cout << pi << '\n';
	
	dfs(g, pi, -1, 0);

	for(int x : max_perm)
		cout << x << ' ';
	cout << '\n';

	return 0;
}
