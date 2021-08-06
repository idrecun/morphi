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
bool NOOUT = false;
bool VERBOSE = false;
bool SHOW_COLOURING = false;
bool SHOW_INV = false;
bool SHOW_TYPE = false;
bool SHOW_MATRIX = false;
bool SHOW_AUT = false;
bool SHOW_BACKJUMP = false;
bool SHOW_STATS = false;

bool NOAUT = false;
bool NOINV = false;

bool GINV_DIST = false;
bool GINV_NOBIT = false;
bool GINV_PATH = false;

bool RELABEL = false;

string helpstring = 
"-h, --help			Prints this message.\n"
"\n"
"Traversal output options:\n"
"-n, --no-output			Suppress output of canonical graph matrix.\n"
"-s, --silent			Silent (print nothing during search) - this is the default printing option.\n"
"-v, --verbose			Verbose (print everything during search) - flips all output option flags.\n"
"-C, --print-colouring		Print colouring for each node in the search tree. Turns it off instead when using -v.\n"
"-I, --print-invariant		Print invariant for each node in the search tree. Turns it off instead when using -v.\n"
"-T, --print-type		Print path type for each node in the search tree. Turns it off instead when using -v.\n"
"-M, --print-matrix		Print graph matrix for each leaf node in the search tree. Turns it off instead when using -v.\n"
"-A, --print-aut			Print detected automorphisms. Turns it off instead when using -v.\n"
"-J, --print-backjump		Print backjump points during search. Turns it off instead when using -v.\n"
"-S, --print-stats		Print statistics after search. Turns it off instead when using -v.\n"
"\n"
"Algorithm options:\n"
"--no-aut			Disable automorphism detection and pruning.\n"
"--no-inv			Disable invariant calculation and pruning.\n"
"-d, --invariant-distance		Use distance vertex invariant.\n"
"-b, --invariant-nobits		Use adjacency vertex invariant without bitvectors.\n"
"-p, --invariant-paths		Use path count vertex invariant.\n"
"-l, --limit-aut			Automorphism limit (NOT IMPLEMENTED).\n"
"\n"
"Input options:\n"
"-r, --relabel			Randomly relabel input graph.\n";


void parse_options(int argc, char** argv) {
	int option_index;
	static option long_options[] = {
		{"help",			no_argument,	0,	'h'},

		{"no-output",				no_argument,	0,	'n'},
		{"silent",				no_argument,	0,	's'},
		{"verbose",				no_argument,	0,	'v'},
		{"print-colouring",		no_argument,	0,	'C'},
		{"print-invariant",		no_argument,	0,	'I'},
		{"print-type",			no_argument,	0,	'T'},
		{"print-matrix",		no_argument,	0,	'M'},
		{"print-aut",			no_argument,	0,	'A'},
		{"print-backjump",		no_argument,	0,	'J'},
		{"print-stats",		no_argument,	0,	'S'},

		{"no-aut",				no_argument,	0,	0},
		{"no-inv",				no_argument,	0,	0},

		{"invariant-nobits",	no_argument,	0,	'b'},
		{"invariant-distance",	no_argument,	0,	'd'},
		{"invariant-paths", 	no_argument,	0,	'p'},
		//{"limit-aut",		required_argument,	0,	'l'},

		{"relabel",				no_argument,	0,	'r'},
		{0,						0,				0,	0}
	};

	int c;
	while((c = getopt_long(argc, argv, "hnvsCITMAJSdbpl:r", long_options, &option_index)) != -1)
		switch(c) {
			case 0:
				switch(option_index) {
					case 10:
						NOAUT = true; break;
					case 11:
						NOINV = true; break;
				}
				break;
			case 'h':
				cout << helpstring;
				exit(EXIT_SUCCESS);
			case 'n':
				NOOUT = true; break;
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
			case 'S':
				SHOW_STATS = true; break;
			case 'd':
				GINV_DIST = true; break;
			case 'b':
				GINV_NOBIT = true; break;
			case 'p':
				GINV_PATH = true; break;
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
	if(GINV_PATH)	g.set_invariant(make_unique<invariant_paths>(g, 3));

	permutation canon = search(g);

	if(!NOOUT) {
		vector<bool> canonical = g.permuted(canon);
		for(int b : canonical)
			cout << b;
		cout << '\n';
	}

	return 0;
}
