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

void dfs(const graph& g, colouring pi, int v, int level, int lmax_level) {

	if(v >= 0)
		stabilized.push_back(v);
	pi.make_equitable(g, v);

	uint32_t pi_phi = pi.invariant();
	bool max_path = !(level > lmax_level + 1 || (max_phi.size() > level && pi_phi < max_phi[level]));
	bool aut_path = !(fst_phi.size() <= level || pi_phi != fst_phi[level]);
	if(!max_path && !aut_path)
		return;

	if(max_path) {
		lmax_level = level;

		if(max_phi.size() > level && pi_phi > max_phi[level])
			max_phi.resize(level);

		if(max_phi.size() <= level) {
			max_phi.push_back(pi_phi);
			max_perm.clear();
		}
	}

	if(fst_perm.empty())
		fst_phi.push_back(pi_phi);

	if(DEBUG) {
		cout << string(level, '\t');
		cout << (max_path ? "MAX" : "AUT");
		cout << " ";
		cout << pi;
		cout << "   INV: ";
		cout << pi_phi;
		cout << '\n';
	}

	vector<int> cell = pi.cell_content(pi.target_cell());
	for(int i = 0; i < cell.size(); i++) {
		int v = cell[i];
		colouring pi_v = pi;
		pi_v.individualize(v);
		dfs(g, pi_v, v, level + 1, lmax_level);

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
		vector<bool> leaf_graph = g.permute(pi.i());
		if(max_path && (max_perm.empty() || (max_phi.size() == level + 1 && leaf_graph > g.permute(max_perm))))
			max_perm = pi.i();

		if(DEBUG) {
			cout << string(level, '\t');
			for(int x : leaf_graph)
				cout << x;
			cout << '\n';
		}

		// Automorphism detected
		if(fst_perm.empty())
			fst_perm = pi.i();
		else if(leaf_graph == g.permute(fst_perm)) {
			if(g.permute(permutation(pi.p().size())) != g.permute(pi.i() * ~fst_perm))
				cout << "problem";
			aut.insert(pi.i() * ~fst_perm);
			if(DEBUG)
				cout << string(level, '\t') << "Automorphism detected: " << (pi.i() * ~fst_perm) << '\n';
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
	dfs(g, pi, -1, 0, -1);

	vector<bool> canonical = g.permute(max_perm);
	for(int b : canonical)
		cout << b;
	cout << '\n';

	return 0;
}
