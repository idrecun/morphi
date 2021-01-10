#include <iostream>
#include <vector>

#include "automorphism_set.h"
#include "colouring.h"
#include "graph.h"

#define DEBUG false

using namespace std;

vector<uint32_t> max_phi;
permutation max_perm;

vector<uint32_t> fst_phi;
permutation fst_perm;

automorphism_set aut;

vector<int> stabilized;

void dfs(const graph& g, colouring pi, int v, int level) {

	if(v >= 0)
		stabilized.push_back(v);
	pi.make_equitable(g, v);

	uint32_t pi_phi = pi.invariant();
	if(max_phi.size() > level && pi_phi < max_phi[level]) {
		// cout << string(level, '\t') << "CUT\n";
		return;
	}
	if(max_phi.size() > level && pi_phi > max_phi[level])
		max_phi.resize(level);
	if(max_phi.size() <= level) {
		max_phi.push_back(pi_phi);
		max_perm.clear();
	}

	if(DEBUG) {
		cout << string(level, '\t') << pi;
		cout << "   INV: ";
		cout << pi.invariant();
		cout << '\n';
	}

	vector<int> cell = pi.cell_content(pi.target_cell());
	for(int i = 0; i < cell.size(); i++) {
		int v = cell[i];
		colouring pi_v = pi;
		pi_v.individualize(v);
		dfs(g, pi_v, v, level + 1);

		if(i == 0 && cell.size() > 1) {
			automorphism_set stabilizer = aut.stabilizer(stabilized);
			if(!stabilizer.empty()) {
				vector<int> mcr = stabilizer.mcr();
				vector<int> intersect;
				set_intersection(cell.begin(), cell.end(), mcr.begin(), mcr.end(), back_inserter(intersect));
				cell = intersect;
			}
		}
	}

	// Discrete colouring
	if(cell.empty()) {
		vector<bool> leaf_graph = g.permute(pi.p());
		if(max_perm.empty() || leaf_graph > g.permute(max_perm))
			max_perm = pi.p();

		// Automorphism detected
		if(fst_perm.empty())
			fst_perm = pi.p();
		else if(leaf_graph == g.permute(fst_perm)) {
			aut.insert(pi.p() * ~fst_perm);
			if(DEBUG)
				cout << string(level, '\t') << "Automorphism detected: " << (pi.p() * ~fst_perm) << '\n';
			// dodati backjump ako je ispunjen uslov za teta
		}
	}

	stabilized.pop_back();
}

int main() {
	int n, m;
	cin >> n >> m;

	aut = automorphism_set(n);
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

	vector<bool> canonical = g.permute(max_perm);
	for(int b : canonical)
		cout << b;
	cout << '\n';
	//cout << g.permute(max_perm) << '\n';

	return 0;
}
