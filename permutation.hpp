#ifndef __PERMUTATION_HPP__
#define __PERMUTATION_HPP__

#include <vector>
#include <fstream>

using std::vector;
using std::ostream;

class permutation {
	public:
		permutation();
		permutation(int n);
		permutation(int n, bool random);
		permutation(const vector<int>& v);

		permutation operator~() const;
		permutation operator*(const permutation&) const;

		bool operator==(const permutation&) const;


		const vector<int>& p() const;
		const vector<int>& i() const;
		int i(size_t v) const;

		class reference {
			friend class permutation;

			public:
				reference& operator=(int oth);
				operator int() const;

			private:
				reference(int i, permutation& p);

				int i;
				permutation& p;
		};

		int operator[](size_t i) const;
		reference operator[](size_t i);

		int set(size_t i, int val);
		void swap(int i, int j);

		void clear();
		bool empty() const;
		size_t size() const;

		friend ostream& operator<<(ostream& out, const permutation& p);

	private:
		size_t n;
		vector<int> pi;
		vector<int> ip;
};

#endif
