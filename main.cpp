#include <iostream>
#include <vector>

#include "algorithms.h"
#include "automorphism_set.h"
#include "colouring.h"
#include "graph.h"

#define DEBUG false
#define NOAUT false
#define NOPHI false

using namespace std;

vector<uint32_t> max_phi;
permutation max_perm;

vector<uint32_t> fst_phi;
permutation fst_perm;
int fst_level;

automorphism_set aut;

vector<int> stabilized;

int dfs(const graph& g, colouring pi, int v, int level, int lmax_level) {

	pi.make_equitable(g, v);

	uint32_t pi_phi = pi.invariant();
	if(DEBUG && NOPHI)
		pi_phi = 0;
	bool max_path = !(level > lmax_level + 1 || (max_phi.size() > level && pi_phi < max_phi[level]));
	bool aut_path = !(fst_phi.size() <= level || pi_phi != fst_phi[level]);
	if(!max_path && !aut_path)
		return level;

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
	vector<int> mcr = cell;
	for(int i = 0; i < cell.size(); i++) {
		int v = cell[i];
		if(!binary_search(mcr.begin(), mcr.end(), v))
			continue;

		colouring pi_v = pi;
		pi_v.individualize(v);

		stabilized.push_back(v);
		int backjump = dfs(g, pi_v, v, level + 1, lmax_level);
		stabilized.pop_back();

		if(level > backjump)
			return backjump;

		if(level < fst_level)
			fst_level = level;

		if(level == backjump || (i == 0 && cell.size() > 1)) {
			if(level == fst_level)
				mcr = intersect(mcr, aut.mcr());
			else
				mcr = intersect(mcr, aut.stabilizer(stabilized).mcr());
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
		if(fst_perm.empty()) {
			fst_perm = pi.i();
			fst_level = level;
		}
		else if(leaf_graph == g.permute(fst_perm)) {
			aut.insert(pi.i() * ~fst_perm);
			if(DEBUG) {
				if(NOAUT)
					aut = automorphism_set(pi.p().size());
				else
					cout << string(level, '\t') << "Automorphism detected: " << (pi.i() * ~fst_perm) << '\n';
			}
			// dodati backjump ako je ispunjen uslov za teta
			vector<int> fst_mcr = aut.mcr();
			if(!binary_search(fst_mcr.begin(), fst_mcr.end(), stabilized[fst_level + 1])) {
				if(DEBUG)
					cout << string(level, '\t') << "BACKJUMP to " << fst_level << '\n';
				return fst_level;
			}
		}
	}

	return level;
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
