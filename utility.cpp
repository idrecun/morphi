#include "utility.hpp"

#include <vector>
#include <algorithm>
#include "permutation.hpp"

using std::vector;

uint32_t hash::value() const {
	return val;
}

// Copies boost's hash_combine implementation
void sequential_hash::update(uint32_t x) {
	val ^= x + 0x9e3779b9 + (val << 6) + (val >> 2);
}

// Represents a multiset using the rolling hash idea:
// (c0 * p^0 + c1 * p^1 + ... + cn * p^n) % m
// where ci is the number of occurances (the count) of i in the multiset and p
// is a prime greater than any ci
// m is set to be 2^32
// TODO: u grafu pretprocesirati vrednosti p^d[u][v], jer se funkcija uvek poziva kao update(d[u][v])
// mozda bolje: kompresija nad matricom d, jer nam nisu bitne vrednosti same po sebi
void multiset_hash::update(uint32_t x) {
	val += modpow(1000003, x);
}

// Implements the modular binary exponentiation algorithm
// specifically for m = 2^32
uint32_t modpow(uint32_t a, uint32_t n) {
	uint32_t res = 1;
	while(n > 0) {
		if(n & 1)
			res = a * res;
		a = a * a;
		n >>= 1;
	}
	return res;
}

// Assumes that vectors represent sets and are sorted.
vector<int> intersect(const vector<int>& a, const vector<int>& b) {
	if(a.size() > b.size())
			return intersect(b, a);

	vector<int> ret;
	for(int x : a)
		if(binary_search(b.begin(), b.end(), x))
			ret.push_back(x);
	return ret;
}

vector<int> merge_orbits(const permutation& a, const permutation& b) {
	int n = a.size();
	vector<int> orbit(n);
	for(int i = 0; i < n; i++)
		orbit[i] = i;

	vector<int> q(n);
	int cnt = 0;
	for(int i = 0; i < n; i++) {
		if(orbit[i] != i)
			continue;
		q[cnt++] = i;
		for(int j = cnt - 1; j < cnt; j++) {
			int anext = a[q[j]], bnext = b[q[j]];
			if(orbit[anext] != i) {
				orbit[anext] = i;
				q[cnt++] = anext;
			}
			if(orbit[bnext] != i) {
				orbit[bnext] = i;
				q[cnt++] = bnext;
			}
		}
	}

	return orbit;
}

permutation from_orbits(const vector<int>& orbit) {
	int n = orbit.size();
	vector<int> last(n);
	vector<int> p(n);
	for(int i = 0; i < n; i++) {
		if(i == orbit[i])
			last[i] = i;
		p[i] = last[orbit[i]];
		p[orbit[i]] = last[orbit[i]] = i;
	}
	return permutation(p);
}
