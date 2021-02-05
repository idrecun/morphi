#include "utility.hpp"

#include <vector>
#include <algorithm>

using std::vector;

uint32_t modpow(uint64_t a, int n, uint32_t m) {
	uint64_t res = 1;
	while(n > 0) {
		if(n & 1)
			res = (a * res) % m;
		a = (a * a) % m;
		n >>= 1;
	}
	return (uint32_t) res;
}

vector<int> intersect(const vector<int>& a, const vector<int>& b) {
	if(a.size() > b.size())
			return intersect(b, a);

	vector<int> ret;
	for(int x : a)
		if(binary_search(b.begin(), b.end(), x))
			ret.push_back(x);
	return ret;
}
