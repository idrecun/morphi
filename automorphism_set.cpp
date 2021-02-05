#include <vector>
#include <algorithm>

#include "automorphism_set.hpp"
#include "permutation.hpp"
#include "utility.hpp"

using std::vector;
using std::swap;

automorphism_set::automorphism_set() {
	n = 0;
}

automorphism_set::automorphism_set(size_t n) {
	this->n = n;
	stab = vector< vector<int> >(n);
	imcr = vector< vector<int> >(n);
}

void automorphism_set::insert(const permutation& aut) {
	vector<bool> is_mcr(n, true);
	vector<int> mcr;
	for(int i = 0; i < n; i++) {
		if(aut[i] == i)
			stab[i].push_back(auts.size());
		if(is_mcr[i]) {
			for(int j = aut[i]; j != i; j = aut[j])
				is_mcr[j] = false;
			mcr.push_back(i);
		}
	}

	for(int v : mcr)
		imcr[v].push_back(auts.size());

	auts.push_back(aut);
}

// O(n)
vector<int> automorphism_set::mcr() const {
	vector<int> mcr;
	for(int i = 0; i < n; i++)
		if(imcr[i].size() == auts.size())
			mcr.push_back(i);
	return mcr;
}

// O(n * t_stab)
automorphism_set automorphism_set::stabilizer(int sPoint) const {
	automorphism_set ret(n);
	for(int i : stab[sPoint])
		ret.insert(auts[i]);
	return ret;
}

// O(n * t_stab) recimo
automorphism_set automorphism_set::stabilizer(const vector<int>& sPoints) const {
	if(sPoints.empty())
		return automorphism_set(n);

	vector<int> stab_indices = stab[sPoints[0]];
	for(int sPoint : sPoints)
		stab_indices = intersect(stab_indices, stab[sPoint]);

	automorphism_set ret(n);
	for(int aut : stab_indices)
		ret.insert(auts[aut]);

	return ret;
}

bool automorphism_set::empty() const {
	return auts.empty();
}

size_t automorphism_set::size() const {
	return auts.size();
}
