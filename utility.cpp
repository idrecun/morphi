#include "utility.hpp"

#include <vector>
#include <algorithm>

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
void multiset_hash::update(uint32_t x) {
	val = ((uint64_t) val + modpow(1000003, x, 1000000007)) % 1000000007;
}

// Implements the modular binary exponentiation algorithm
uint32_t modpow(uint64_t a, uint32_t n, uint32_t m) {
	uint64_t res = 1;
	while(n > 0) {
		if(n & 1)
			res = (a * res) % m;
		a = (a * a) % m;
		n >>= 1;
	}
	return (uint32_t) res;
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
