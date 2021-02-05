#ifndef __UTILITY_HPP__
#define __UTILITY_HPP__

#include <vector>
#include <cstdint>

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
		void update(uint32_t) override;
};

class multiset_hash : public hash {
	public: 
		void update(uint32_t) override;
};

uint32_t modpow(uint64_t a, uint32_t n, uint32_t m);

vector<int> intersect(const vector<int>& a, const vector<int>& b);

#endif
