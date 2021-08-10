#include <vector>
#include <algorithm>

#include "automorphism_set.hpp"
#include "permutation.hpp"
#include "utility.hpp"

using std::vector;
using std::swap;
using std::min;

automorphism_set::automorphism_set() {
	n = 0;
}

automorphism_set::automorphism_set(size_t n) {
	this->n = n;
	stab = vector< vector<int> >(n);
	orbit = vector<int>(n);
	for(int i = 0; i < n; i++)
		orbit[i] = i;
	orbit_permutation = permutation(orbit);
}

void automorphism_set::insert(const permutation& aut) {
	orbit = merge_orbits(orbit_permutation, aut);
	orbit_permutation = from_orbits(orbit);

	for(int i = 0; i < n; i++)
		if(aut[i] == i)
			stab[i].push_back(auts.size());

	auts.push_back(aut);
}

// O(n)
vector<int> automorphism_set::mcr() const {
	vector<int> mcr;
	for(int i = 0; i < n; i++)
		if(orbit[i] == i)
			mcr.push_back(i);
	return mcr;
}

bool automorphism_set::is_mcr(int v) const {
	return orbit[v] == v;
}

vector<int> automorphism_set::stabilizer_mcr(const vector<int>& sPoints) const {
	if(sPoints.empty())
		return mcr();

	vector<int> stab_indices = stab[sPoints[0]];
	for(int sPoint : sPoints)
		stab_indices = intersect(stab_indices, stab[sPoint]);

	vector<int> s_orbit(n);
	for(int i = 0; i < n; i++)
		s_orbit[i] = i;
	permutation sop = permutation(s_orbit);

	for(int i : stab_indices) {
		s_orbit = merge_orbits(sop, auts[i]);
		sop = from_orbits(s_orbit);
	}

	vector<int> s_mcr;
	for(int i = 0; i < n; i++)
		if(s_orbit[i] == i)
			s_mcr.push_back(i);

	return s_mcr;
}


bool automorphism_set::empty() const {
	return auts.empty();
}

size_t automorphism_set::size() const {
	return auts.size();
}
