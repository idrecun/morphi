#include "utility.hpp"

#include <vector>
#include <algorithm>
#include <iostream>
#include "permutation.hpp"

using std::vector;
using std::distance;
using std::unique;
using std::swap;

uint32_t hash::value() const {
	return val;
}

// Copies boost's hash_combine implementation
void sequential_hash::update(uint32_t x) {
	val ^= x + 0x9e3779b9 + (val << 6) + (val >> 2);
}

uint16_t sequential_hash::combine(uint16_t curr, uint16_t next) {
	return curr ^ (next + 0x9e37 + (curr << 3) + (curr >> 1));
}

// Represents a multiset using the rolling hash idea:
// (c0 * p^0 + c1 * p^1 + ... + cn * p^n) % m
// where ci is the number of occurances (the count) of i in the multiset and p
// is a prime greater than any ci
// m is set to be 2^32
// TODO: u grafu pretprocesirati vrednosti p^d[u][v], jer se funkcija uvek poziva kao update(d[u][v])
// mozda bolje: kompresija nad matricom d, jer nam nisu bitne vrednosti same po sebi
// mozda jos bolje: memoizacija!
void multiset_hash::update(uint32_t x) {
	val += modpow(x);
}

// Implements the modular binary exponentiation algorithm
// specifically for m = 2^32, a = 1000003
uint32_t multiset_hash::modpow(uint32_t n) {
	uint32_t a = 1000003;
	uint32_t res = 1;
	while(n > 0) {
		if(n & 1)
			res = a * res;
		a = a * a;
		n >>= 1;
	}
	return res;
}

compressed_matrix::compressed_matrix() {
	mode = 32;
}

compressed_matrix::compressed_matrix(int size, int bits) {
	if(bits <= 1) mode = 1;
	else if(bits <= 8) mode = 8;
	else if(bits <= 16) mode = 16;
	else mode = 32;

	resize(size);
}

void compressed_matrix::from_matrix(const vector< vector<uint32_t> >& m) {
	int size = m.size();

	vector<uint32_t> values;
	for(int i = 0; i < size; i++)
		for(int j = 0; j <= i; j++)
			values.push_back(m[i][j]);

	sort(values.begin(), values.end());
	values.resize(distance(values.begin(), unique(values.begin(), values.end())));

	if(values.size() <= (1 << 1)) mode = 1;
	else if(values.size() <= (1 << 8)) mode = 8;
	else if(values.size() <= (1 << 16)) mode = 16;
	else mode = 32;

	resize(size);

	for(int i = 0; i < size; i++)
		for(int j = 0; j <= i; j++)
			set(i, j, (uint32_t)(lower_bound(values.begin(), values.end(), m[i][j]) - values.begin()));
		
}

uint32_t compressed_matrix::get(int i, int j) const {
	if(i < j)
		swap(i, j);
	if(mode == 1)  return m_1[i][j];
	if(mode == 8)  return m_8[i][j];
	if(mode == 16) return m_16[i][j];
	return m_32[i][j];
}

void compressed_matrix::set(int i, int j, uint32_t val) {
	if(i < j)
		swap(i, j);
	if(mode == 1)  m_1[i][j] = val;
	else if(mode == 8) m_8[i][j] = val;
	else if(mode == 16) m_16[i][j] = val;
	else m_32[i][j] = val;
}

void compressed_matrix::resize(int size) {
	if(mode == 1) {
		m_1 = vector< vector<bool> >(size);
		for(int i = 0; i < size; i++)
			m_1[i].resize(i + 1);
	}
	else if(mode == 8) {
		m_8 = vector< vector<uint8_t> >(size);
		for(int i = 0; i < size; i++)
			m_8[i].resize(i + 1);
	}
	else if(mode == 16) {
		m_16 = vector< vector<uint16_t> >(size);
		for(int i = 0; i < size; i++)
			m_16[i].resize(i + 1);
	}
	else {
		m_32 = vector< vector<uint32_t> >(size);
		for(int i = 0; i < size; i++)
			m_32[i].resize(i + 1);
	}
}

void compressed_matrix::resize(int size, int valcount) {
	if(valcount <= (1 << 1)) mode = 1;
	else if(valcount <= (1 << 8)) mode = 8;
	else if(valcount <= (1 << 16)) mode = 16;
	else mode = 32;

	resize(size);
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
		for(int j = cnt - 1; j < cnt; j++)
			for(int next : {a[q[j]], b[q[j]]})
				if(orbit[next] != i) {
					orbit[next] = i;
					q[cnt++] = next;
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
