#include "search.hpp"

#include "utility.hpp"
#include "automorphism_set.hpp"

// Traversal output options
extern bool VERBOSE;
extern bool SHOW_COLOURING;
extern bool SHOW_INV;
extern bool SHOW_TYPE;
extern bool SHOW_MATRIX;
extern bool SHOW_AUT;
extern bool SHOW_BACKJUMP;
extern bool SHOW_STATS;

extern bool NOAUT;
extern bool NOINV;

extern bool USE_DV;
extern bool RELABEL;

// Algorithm state
node max_node;
node fst_node;

automorphism_set aut;

vector<int> stabilized;

// Statistics
int tree_size = 0;


permutation search(const graph& g) {
	aut = automorphism_set(g.v_count());
	colouring pi(g.v_count());

	search(g, pi, true, true);

	if(SHOW_STATS) {
		cout << tree_size << '\n';
		cout << aut.size() << '\n';
	}

	return max_node.p;
}

int search(const graph& g, colouring pi, bool max_path, bool aut_path) {

	tree_size++;

	int level = stabilized.size();
	int v = level == 0 ? -1 : stabilized.back();
	pi.make_equitable(g, v);

	uint32_t pi_phi = pi.invariant();

	if(NOINV)
		pi_phi = 0;


	// Update first phi if necessary
	if(!fst_node.is_leaf())
		fst_node.push(pi_phi);

	// Prune by phi
	max_path = max_path && (max_node.length() <= level || pi_phi >= max_node[level]);
	aut_path = aut_path && fst_node.length() > level && pi_phi == fst_node[level];

	if(!max_path && !aut_path)
		return level;

	// Update max phi if necessary
	if(max_path) {
		max_node.lca = level;

		if(max_node.length() > level && pi_phi > max_node[level])
			max_node.invariant.resize(level);

		if(max_node.length() <= level) {
			max_node.push(pi_phi);
			max_node.leaf_node = false;
		}
	}

	if(VERBOSE) {
		cout << '\n';
		cout << string(level, '\t');

		if(SHOW_COLOURING) cout << pi;
		if(SHOW_INV) cout << " [phi: " << pi_phi << "]";
		if(SHOW_TYPE) cout << " [path: " << (max_path ? "MAX" : "AUT") << "]";
	}

	vector<int> cell = pi.cell_content(pi.target_cell()).vertices;
	vector<int> mcr = cell; // For pruning by automorphism
	for(int i = 0; i < cell.size(); i++) {
		int v = cell[i];

		// Prune by automorphism
		if(!binary_search(mcr.begin(), mcr.end(), v))
			continue;

		// Add v to current path and search recursively
		stabilized.push_back(v);
		int backjump = search(g, pi.individualized(v), max_path, aut_path);
		stabilized.pop_back();

		// Backjump if necessary
		if(level > backjump)
			return backjump;

		// Update lowest common ancestors
		fst_node.lca = min(fst_node.lca, level);
		max_node.lca = min(max_node.lca, level);

		// Update mcr for automorphism pruning if
		// a) we just backjumped or
		// b) we just finished traversing the first child
		if(level == backjump || (i == 0 && cell.size() > 1)) {
			if(level == fst_node.lca)
				mcr = intersect(mcr, aut.mcr());
			else
				mcr = intersect(mcr, aut.stabilizer(stabilized).mcr());
		}
	}

	// Discrete colouring
	if(cell.empty()) {

		if(VERBOSE && SHOW_MATRIX) {
			vector<bool> leaf_graph = g.permuted(pi.i());
			cout << " [matrix: ";
			for(int x : leaf_graph)
				cout << x;
			cout << ']';
		}

		// Check for maximal leaf
		if(max_path && (!max_node.is_leaf() || (max_node.length() == level + 1 && g.less(max_node.p, pi.i()))))
			max_node.set_leaf(pi.i());

		// Check for first leaf
		if(!fst_node.is_leaf()) {
			fst_node.lca = level;
			fst_node.set_leaf(pi.i());
			return level;
		}

		if(NOAUT)
			return level;

		// Check for automorphism
		permutation a = ~fst_node.p * pi.i();
		if(g.is_aut(a)) {
			if(VERBOSE) {
				if(SHOW_AUT) cout << " [aut: " << a << ']';
				if(SHOW_BACKJUMP) cout << " [BACKJUMP: " << fst_node.lca << ']';
			}
			aut.insert(a);
			return fst_node.lca;
		}

		a = ~max_node.p * pi.i();
		if(g.is_aut(a)) {
			if(VERBOSE) {
				if(SHOW_AUT) cout << " [aut: " << a << ']';
				if(SHOW_BACKJUMP) cout << " [BACKJUMP: " << max_node.lca << ']';
			}
			aut.insert(a);
			// greska, treba !aut.is_mcr???
			if(!aut.is_mcr(stabilized[fst_node.lca]))
				return fst_node.lca;
			return max_node.lca;
		}
	}

	return level;
}
