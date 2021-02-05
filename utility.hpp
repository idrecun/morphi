#ifndef __UTILITY_HPP__
#define __UTILITY_HPP__

#include <vector>
#include <cstdint>

using std::vector;

uint32_t modpow(uint64_t a, int n, uint32_t m);

vector<int> intersect(const vector<int>& a, const vector<int>& b);

#endif
