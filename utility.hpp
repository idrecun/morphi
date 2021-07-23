#ifndef __UTILITY_HPP__
#define __UTILITY_HPP__

#include <vector>
#include <cstdint>
#include "permutation.hpp"

using std::vector;

class hash {
	public:

		virtual void update(uint32_t) = 0;
		uint32_t value() const;
	protected:
		uint32_t val = 0;
};

class sequential_hash : public hash {
	public:
		static uint16_t combine(uint16_t, uint16_t);
		void update(uint32_t) override;
};

class multiset_hash : public hash {
	public: 
		static uint32_t modpow(uint32_t n);

		void update(uint32_t) override;
};


class compressed_matrix {
	public:
		compressed_matrix();
		compressed_matrix(int size, int bits);

		uint32_t get(int i, int j) const;
		void set(int i, int j, uint32_t val);

		void resize(int size);
		void resize(int size, int valcount);

	private:
		int mode;
		vector< vector<bool> >      m_1;
		vector< vector<uint8_t> >   m_8;
		vector< vector<uint16_t> >  m_16;
		vector< vector<uint32_t> >  m_32;

};

vector<int> intersect(const vector<int>& a, const vector<int>& b);

vector<int> merge_orbits(const permutation& a, const permutation& b);
permutation from_orbits(const vector<int>& orbit);

#endif
