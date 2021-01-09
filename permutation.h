#ifndef __PERMUTATION_H__
#define __PERMUTATION_H__

#include <vector>
#include <fstream>

using std::vector;
using std::ostream;

class permutation {
	public:

		permutation() {
			n = 0;
		}

		permutation(int n) {
			this->n = n;
			pi = vector<int>(n);
			ip = vector<int>(n);
			for(int i = 0; i < n; i++)
				pi[i] = ip[i] = i;
		}

		permutation operator~() const {
			permutation ret(n);
			ret.pi = ip;
			ret.ip = pi;
			return ret;
		}

		permutation operator*(const permutation& oth) const {
			permutation ret(n);
			for(int i = 0; i < n; i++)
				ret.set(i, pi[oth[i]]);
			return ret;
		}

		const vector<int>& p() const {
			return pi;
		}

		const vector<int>& i() const {
			return ip;
		}

		int i(size_t v) const {
			return ip[v];
		}

		int operator[](size_t i) const {
			return pi[i];
		}

		int set(size_t i, int val) {
			pi[ip[val]] = pi[i];
			ip[pi[i]] = ip[val];

			pi[i] = val;
			ip[val] = i;

			return pi[i];
		}

		void swap(int i, int j) {
			set(i, pi[j]);
		}

		void clear() {
			n = 0;
			pi.clear();
			ip.clear();
		}

		bool empty() const {
			return n == 0;
		}

		size_t size() const {
			return n;
		}

		friend ostream& operator<<(ostream& out, const permutation& p);

	private:
		size_t n;
		vector<int> pi;
		vector<int> ip;
};

ostream& operator<<(ostream& out, const permutation& p) {
	if(p.n > 0) {
		for(int i = 0; i < p.n - 1; i++)
			out << p[i] << ' ';
		out << p[p.n - 1];
	}
	else out << ' ';
	return out;
}

#endif
