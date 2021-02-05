#include <iostream>
#include <vector>
#include <getopt.h>
#include <chrono>

#include "utility.hpp"
#include "automorphism_set.hpp"
#include "colouring.hpp"
#include "graph.hpp"

// Traversal output options
bool VERBOSE = false;
bool SHOW_COLOURING = false;
bool SHOW_INV = false;
bool SHOW_TYPE = false;
bool SHOW_CANON = false;
bool SHOW_AUT = false;
bool SHOW_BACKJUMP = false;

bool NOAUT = false;
bool NOINV = false;

bool USE_DV = false;
bool RELABEL = false;

using namespace std;
using namespace std::chrono;

vector<uint32_t> max_phi;
permutation max_perm;

vector<uint32_t> fst_phi;
permutation fst_perm;
int fst_level;

automorphism_set aut;

vector<int> stabilized;

int dfs(const graph& g, colouring pi, int v, int level, int max_level) {

	pi.make_equitable(g, v, USE_DV);

	uint32_t pi_phi = pi.invariant();

	if(NOINV)
		pi_phi = 0;

	// Prune by phi
	bool max_path = !(level > max_level + 1 || (max_phi.size() > level && pi_phi < max_phi[level]));
	bool aut_path = !(fst_phi.size() <= level || pi_phi != fst_phi[level]);

	if(!max_path && !aut_path)
		return level;

	// Update max phi if necessary
	if(max_path) {
		max_level = level;

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

	if(VERBOSE) {
		cout << '\n';
		cout << string(level, '\t');

		if(SHOW_COLOURING) cout << pi;
		if(SHOW_INV) cout << " [phi: " << pi_phi << "]";
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
		int backjump = dfs(g, pi.individualized(v), v, level + 1, max_level);
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
		vector<bool> leaf_graph = g.permuted(pi.i());

		if(VERBOSE && SHOW_CANON) {
			cout << " [canon: ";
			for(int x : leaf_graph)
				cout << x;
			cout << ']';
		}

		// Check for maximal leaf
		if(max_path && (max_perm.empty() || (max_phi.size() == level + 1 && leaf_graph > g.permuted(max_perm))))
			max_perm = pi.i();

		// Check for first leaf
		if(fst_perm.empty()) {
			fst_perm = pi.i();
			fst_level = level;
		}

		// Check for automorphism
		permutation a;
		if(leaf_graph == g.permuted(max_perm))
			a = pi.i() * ~max_perm;
		if(leaf_graph == g.permuted(fst_perm))
			a = pi.i() * ~fst_perm;
		
		if(!a.empty() && !(a == permutation(a.size()))) {

			if(NOAUT)
				return level;

			if(VERBOSE && SHOW_AUT)
				cout << " [aut: " << a << ']';

			// Save automorphism
			aut.insert(a);

			// Backjump to fst_level if possible
			vector<int> fst_mcr = aut.mcr();
			if(!binary_search(fst_mcr.begin(), fst_mcr.end(), stabilized[fst_level + 1])) {
				if(VERBOSE && SHOW_BACKJUMP)
					cout << " [BACKJUMP: " << fst_level << ']';
				return fst_level;
			}

			// Backjump to max_level otherwise
			return max_level;
		}
	}

	return level;
}

// Options:
// 
// Traversal output options:
// -s, --silent				silent (print none) - default
// -v, --verbose			verbose (print all) - flips all flags
// -C, --print-colouring	colouring
// -I, --print-invariant	invariant
// -T, --print-type			path type
// -M, --print-matrix		binary graph string at each leaf
// -A, --print-aut			automorphism
// -J, --print-backjump		backjumps
//
// Algorithm options:
// --no-aut					no automorphisms
// --no-inv					no phi (invariant equal everywhere)
// -d, --use-distance		use distance matrix for equitable colourings
// -l, --limit-aut			automorphism limit?
//
// input options:
// -r, --relabel			random relabel input graph(s)

void parse_options(int argc, char** argv) {
	int option_index;
	static option long_options[] = {
		{"silent",			no_argument,	0,	's'},
		{"verbose",			no_argument,	0,	'v'},
		{"print-colouring",	no_argument,	0,	'C'},
		{"print-invariant",	no_argument,	0,	'I'},
		{"print-type",		no_argument,	0,	'T'},
		{"print-matrix",	no_argument,	0,	'M'},
		{"print-aut",		no_argument,	0,	'A'},
		{"print-backjump",	no_argument,	0,	'J'},

		{"no-aut",			no_argument,	0,	0},
		{"no-inv",			no_argument,	0,	0},

		{"use-distance",	no_argument,	0,	'd'},
		//{"limit-aut",		required_argument,	0,	'l'},

		{"relabel",			no_argument,	0,	'r'},
		{0,					0,				0,	0}
	};

	int c;
	while((c = getopt_long(argc, argv, "vsCITMAJdl:r", long_options, &option_index)) != -1)
		switch(c) {
			case 0:
				switch(option_index) {
					case 8:
						NOAUT = true; break;
					case 9:
						NOINV = true; break;
				}
				break;
			case 'v':
				VERBOSE = true; break;
			case 's':
				VERBOSE = false; break;
			case 'C':
				SHOW_COLOURING = true; break;
			case 'I':
				SHOW_INV = true; break;
			case 'T':
				SHOW_TYPE = true; break;
			case 'M':
				SHOW_CANON = true; break;
			case 'A':
				SHOW_AUT = true; break;
			case 'J':
				SHOW_BACKJUMP = true; break;
			case 'd':
				USE_DV = true; break;
			case 'r':
				RELABEL = true; break;
		}
	if(VERBOSE) {
		SHOW_COLOURING = !SHOW_COLOURING;
		SHOW_INV = !SHOW_INV;
		SHOW_TYPE = !SHOW_TYPE;
		SHOW_CANON = !SHOW_CANON;
		SHOW_AUT = !SHOW_AUT;
		SHOW_BACKJUMP = !SHOW_BACKJUMP;
	}
	else VERBOSE = SHOW_COLOURING || SHOW_INV || SHOW_TYPE || SHOW_CANON || SHOW_AUT || SHOW_BACKJUMP;
}

int main(int argc, char** argv) {
	srand(duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count());
	parse_options(argc, argv);

	graph g;
	cin >> g;
	int n = g.v_count();

	if(RELABEL) {
		vector<int> v(n);
		for(int i = 0; i < n; i++)
			v[i] = i;
		random_shuffle(v.begin(), v.end());

		permutation relabel(v);
		g.relabel(relabel);
	}

	if(USE_DV)
		g.init_distances();

	aut = automorphism_set(n);

	colouring pi(n);
	dfs(g, pi, -1, 0, -1);
	if(VERBOSE) cout << '\n';

	vector<bool> canonical = g.permuted(max_perm);
	for(int b : canonical)
		cout << b;
	cout << '\n';

	return 0;
}
