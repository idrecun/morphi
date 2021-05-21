#include <iostream>
#include <vector>
#include <getopt.h>
#include <chrono>

#include "utility.hpp"
#include "automorphism_set.hpp"
#include "colouring.hpp"
#include "graph.hpp"
#include "graph_invariant.hpp"
#include "search.hpp"

using namespace std;
using namespace std::chrono;

// Traversal output options
bool VERBOSE = false;
bool SHOW_COLOURING = false;
bool SHOW_INV = false;
bool SHOW_TYPE = false;
bool SHOW_MATRIX = false;
bool SHOW_AUT = false;
bool SHOW_BACKJUMP = false;

bool NOAUT = false;
bool NOINV = false;

bool GINV_DIST = false;
bool GINV_NOBIT = false;

bool RELABEL = false;

// Options:
//
// -h, --help				print options
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
// -d, --invariant-distance	Use distance vertex invariant
// -b, --invariant-nobits	Use adjacency vertex invariant without bitvectors
// -l, --limit-aut			automorphism limit?
//
// input options:
// -r, --relabel			random relabel input graph(s)

string helpstring = 
"-h, --help			Prints this message.\n"
"\n"
"Traversal output options:\n"
"-s, --silent			Silent (print nothing during search) - this is the default printing option.\n"
"-v, --verbose			Verbose (print everything during search) - flips all output option flags.\n"
"-C, --print-colouring		Print colouring for each node in the search tree. Turns it off instead when using -v.\n"
"-I, --print-invariant		Print invariant for each node in the search tree. Turns it off instead when using -v.\n"
"-T, --print-type		Print path type for each node in the search tree. Turns it off instead when using -v.\n"
"-M, --print-matrix		Print graph matrix for each leaf node in the search tree. Turns it off instead when using -v.\n"
"-A, --print-aut			Print detected automorphisms. Turns it off instead when using -v.\n"
"-J, --print-backjump		Print backjump points during search. Turns it off instead when using -v.\n"
"\n"
"Algorithm options:\n"
"--no-aut			Disable automorphism detection and pruning.\n"
"--no-inv			Disable invariant calculation and pruning.\n"
"-d, --invariant-distance		Use distance vertex invariant.\n"
"-b, --invariant-nobits		Use adjacency vertex invariant without bitvectors.\n"
"-l, --limit-aut			Automorphism limit (NOT IMPLEMENTED).\n"
"\n"
"Input options:\n"
"-r, --relabel			Randomly relabel input graph.\n";


void parse_options(int argc, char** argv) {
	int option_index;
	static option long_options[] = {
		{"help",			no_argument,	0,	'h'},

		{"silent",				no_argument,	0,	's'},
		{"verbose",				no_argument,	0,	'v'},
		{"print-colouring",		no_argument,	0,	'C'},
		{"print-invariant",		no_argument,	0,	'I'},
		{"print-type",			no_argument,	0,	'T'},
		{"print-matrix",		no_argument,	0,	'M'},
		{"print-aut",			no_argument,	0,	'A'},
		{"print-backjump",		no_argument,	0,	'J'},

		{"no-aut",				no_argument,	0,	0},
		{"no-inv",				no_argument,	0,	0},

		{"invariant-nobits",	no_argument,	0,	'b'},
		{"invariant-distance",	no_argument,	0,	'd'},
		//{"limit-aut",		required_argument,	0,	'l'},

		{"relabel",				no_argument,	0,	'r'},
		{0,						0,				0,	0}
	};

	int c;
	while((c = getopt_long(argc, argv, "hvsCITMAJdbl:r", long_options, &option_index)) != -1)
		switch(c) {
			case 0:
				switch(option_index) {
					case 8:
						NOAUT = true; break;
					case 9:
						NOINV = true; break;
				}
				break;
			case 'h':
				cout << helpstring;
				exit(EXIT_SUCCESS);
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
				SHOW_MATRIX = true; break;
			case 'A':
				SHOW_AUT = true; break;
			case 'J':
				SHOW_BACKJUMP = true; break;
			case 'd':
				GINV_DIST = true; break;
			case 'b':
				GINV_NOBIT = true; break;
			case 'r':
				RELABEL = true; break;
		}
	if(VERBOSE) {
		SHOW_COLOURING = !SHOW_COLOURING;
		SHOW_INV = !SHOW_INV;
		SHOW_TYPE = !SHOW_TYPE;
		SHOW_MATRIX = !SHOW_MATRIX;
		SHOW_AUT = !SHOW_AUT;
		SHOW_BACKJUMP = !SHOW_BACKJUMP;
	}
	else VERBOSE = SHOW_COLOURING || SHOW_INV || SHOW_TYPE || SHOW_MATRIX || SHOW_AUT || SHOW_BACKJUMP;
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

	g.set_invariant(make_unique<invariant_bitvector>(g));
	if(GINV_NOBIT)	g.set_invariant(make_unique<invariant_adjacent>(g));
	if(GINV_DIST)	g.set_invariant(make_unique<invariant_distance>(g));

	permutation canon = search(g);

	vector<bool> canonical = g.permuted(canon);
	for(int b : canonical)
		cout << b;
	cout << '\n';

	return 0;
}
