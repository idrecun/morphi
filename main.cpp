#include <iostream>
#include <vector>

#include "algorithms.h"
#include "automorphism_set.h"
#include "colouring.h"
#include "graph.h"

#define DEBUG true
#define SHOW_PHI true
#define SHOW_TYPE true
#define SHOW_CANON false
#define SHOW_AUT true
#define SHOW_BACKJUMP true
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

	// Prune by phi
	bool max_path = !(level > lmax_level + 1 || (max_phi.size() > level && pi_phi < max_phi[level]));
	bool aut_path = !(fst_phi.size() <= level || pi_phi != fst_phi[level]);

	if(!max_path && !aut_path)
		return level;

	// Update max phi if necessary
	if(max_path) {
		lmax_level = level;

		if(max_phi.size() > level && pi_phi > max_phi[level])
			max_phi.resize(level);

		if(max_phi.size() <= level) {
			max_phi.push_back(pi_phi);
			max_perm.clear();
		}
	}

	// Update first phi if necessary
	if(fst_perm.empty())
		fst_phi.push_back(pi_phi);

	if(DEBUG) {
		cout << '\n';

		cout << string(level, '\t');
		cout << pi;
		
		if(SHOW_PHI) cout << " [phi: " << pi_phi << "]";
		if(SHOW_TYPE) cout << " [path: " << (max_path ? "MAX" : "AUT") << "]";
	}

	vector<int> cell = pi.cell_content(pi.target_cell());
	vector<int> mcr = cell; // For pruning by automorphisms
	for(int i = 0; i < cell.size(); i++) {
		int v = cell[i];

		// Prune by automorphisms
		if(!binary_search(mcr.begin(), mcr.end(), v))
			continue;

		// Add v to current path and search recursively
		stabilized.push_back(v);
		int backjump = dfs(g, pi.individualized(v), v, level + 1, lmax_level);
		stabilized.pop_back();

		// Backjump if necessary
		if(level > backjump)
			return backjump;

		// Update lowest common ancestor of all leaves if necessary
		if(level < fst_level)
			fst_level = level;

		// Update mcr for automorphism pruning if
		// a) we just backjumped or
		// b) we just finished traversing the first child
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

		if(DEBUG && SHOW_CANON) {
			cout << " [canon: ";
			for(int x : leaf_graph)
				cout << x;
			cout << ']';
		}

		// Check for maximal leaf
		if(max_path && (max_perm.empty() || (max_phi.size() == level + 1 && leaf_graph > g.permute(max_perm))))
			max_perm = pi.i();

		// Check for first leaf
		if(fst_perm.empty()) {
			fst_perm = pi.i();
			fst_level = level;
		}

		// Check for automorphism
		permutation a;
		if(leaf_graph == g.permute(max_perm))
			a = pi.i() * ~max_perm;
		if(leaf_graph == g.permute(fst_perm))
			a = pi.i() * ~fst_perm;
		
		if(!a.empty() && !(a == permutation(a.size()))) {

			if(DEBUG) {
				if(NOAUT)
					return level;
				if(SHOW_AUT) cout << " [aut: " << a << ']';
			}

			// Save automorphism
			aut.insert(a);

			// Backjump to fst_level if possible
			vector<int> fst_mcr = aut.mcr();
			if(!binary_search(fst_mcr.begin(), fst_mcr.end(), stabilized[fst_level + 1])) {
				if(DEBUG && SHOW_BACKJUMP)
					cout << " [BACKJUMP: " << fst_level << ']';
				return fst_level;
			}

			// Backjump to max_level otherwise
			return lmax_level;
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
	cout << '\n';

	vector<bool> canonical = g.permute(max_perm);
	for(int b : canonical)
		cout << b;
	cout << '\n';

	return 0;
}
