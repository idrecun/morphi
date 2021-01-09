#ifndef __AUTOMORPHISMSET_H__
#define __AUTOMORPHISMSET_H__

class AutomorphismSet {
	public:
		AutomorphismSet(size_t n) {
			this->n = n;
			stab = vector< vector<int> >(n);
			imcr = vector< vector<int> >(n);
		}

		// O(n)
		void insert(const vector<int>& aut) {
			vector<bool> is_mcr(aut.size(), true);
			vector<int> mcr;
			for(int i = 0; i < n; i++) {
				if(aut[i] == i)
					stab[i].push_back(auts.size());
				if(is_mcr[i]) {
					for(int j = aut[i]; j != i; j = aut[j])
						is_mcr[j] = false;
					mcr.push_back(i);
				}
			}

			for(int v : mcr)
				imcr[v].push_back(auts.size());

			auts.push_back(mcr);
		}

		// O(n)
		vector<int> mcr() const {
			vector<int> mcr;
			for(int i = 0; i < n; i++)
				if(imcr[i].size() == auts.size())
					mcr.push_back(i);
			return mcr;
		}

		// O(n * t_stab)
		AutomorphismSet stabilizer(int sPoint) const {
			AutomorphismSet ret(n);
			for(int i : stab[sPoint])
				ret.insert(auts[i]);
			return ret;
		}

		// O(n * t_stab) recimo
		AutomorphismSet stabilizer(const vector<int>& sPoints) const {
			vector<int> intersect = stab[sPoints[0]];
			for(int sPoint : sPoints) {
				vector<int> intersect_r;
				for(int aut : intersect)
					if(binary_search(stab[sPoint].begin(), stab[sPoint].end(), aut))
						intersect_r.push_back(aut);
				intersect = intersect_r;
			}

			AutomorphismSet ret(n);
			for(int aut : intersect)
				ret.insert(auts[aut]);
			return ret;
		}

	private:
		size_t n;
		vector< vector<int> > stab;
		vector< vector<int> > imcr;
		vector< vector<int> > auts;
};

#endif