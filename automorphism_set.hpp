#ifndef __AUTOMORPHISM_SET_HPP__
#define __AUTOMORPHISM_SET_HPP__

#include <vector>
#include "permutation.hpp"

class automorphism_set {
	public:
		automorphism_set();
		automorphism_set(size_t);

		void insert(const permutation&);

		vector<int> mcr() const;

		automorphism_set stabilizer(int sPoint) const;
		automorphism_set stabilizer(const vector<int>& sPoints) const;

		bool empty() const;
		size_t size() const;

	private:
		size_t n;
		vector< vector<int> > stab;
		vector< vector<int> > imcr;
		vector< permutation > auts;
};

#endif
