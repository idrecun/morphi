#include <vector>
#include <fstream>
#include <algorithm>

#include "permutation.hpp"

using std::vector;
using std::ostream;

permutation::permutation() {
	n = 0;
}

permutation::permutation(int n) {
	this->n = n;
	pi = vector<int>(n);
	ip = vector<int>(n);
	for(int i = 0; i < n; i++)
		pi[i] = ip[i] = i;
}

permutation::permutation(const vector<int>& v) {
	n = v.size();
	pi = vector<int>(n);
	ip = vector<int>(n);
	for(int i = 0; i < n; i++) {
		pi[i] = v[i];
		ip[pi[i]] = i;
	}
}

permutation permutation::operator~() const {
	permutation ret(n);
	ret.pi = ip;
	ret.ip = pi;
	return ret;
}

permutation permutation::operator*(const permutation& oth) const {
	permutation ret(n);
	for(int i = 0; i < n; i++)
		ret.set(i, oth[pi[i]]);
	return ret;
}

bool permutation::operator==(const permutation& oth) const {
	return n == oth.n && pi == oth.pi;
}

permutation::reference& permutation::reference::operator=(int oth) {
	p.set(i, oth);
	return *this;
}

permutation::reference::operator int() const { return p.pi[i]; }

permutation::reference::reference(int i, permutation& p) : i(i), p(p) {}

const vector<int>& permutation::p() const {
	return pi;
}

const vector<int>& permutation::i() const {
	return ip;
}

int permutation::i(size_t v) const {
	return ip[v];
}

int permutation::operator[](size_t i) const {
	return pi[i];
}

permutation::reference permutation::operator[](size_t i) {
	return reference(i, *this);
}

int permutation::set(size_t i, int val) {
	pi[ip[val]] = pi[i];
	ip[pi[i]] = ip[val];

	pi[i] = val;
	ip[val] = i;

	return pi[i];
}

void permutation::swap(int i, int j) {
	set(i, pi[j]);
}

void permutation::clear() {
	n = 0;
	pi.clear();
	ip.clear();
}

bool permutation::empty() const {
	return n == 0;
}

size_t permutation::size() const {
	return n;
}

vector<int> permutation::orbits() const {
	vector<int> ret(n, n);
	for(int i = 0; i < n; i++)
		for(int j = i; ret[j] > i; j = pi[j])
			ret[j] = i;
	return ret;
}

ostream& operator<<(ostream& out, const permutation& p) {
	if(p.n > 0) {
		for(int i = 0; i < p.n - 1; i++)
			out << p[i] << ' ';
		out << p[p.n - 1];
	}
	else out << ' ';
	return out;
}
