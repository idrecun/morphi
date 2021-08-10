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

// Hash of a multiset is calculated as the sum of hashes of its
// elements mod 2^32
void multiset_hash::update(uint32_t x) {
	val += single(x);
}

// Single integer hash
// https://github.com/skeeto/hash-prospector
uint32_t multiset_hash::single(uint32_t x) {
	x ^= x >> 16;
	x *= 0x7feb352d;
	x ^= x >> 15;
	x *= 0x846ca68b;
	x ^= x >> 16;
	return x;
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
