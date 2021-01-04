#ifndef __GRAPH_H__
#define __GRAPH_H__

#include <vector>

using std::vector;

class graph {
public:
	graph(int n) {
		this->n = n;
		m = vector< vector<int> >(n, vector<int>(n, 0));
		g = vector< vector<int> >(n);
	}

	int count(int v, const vector<int>& W) const {
		int cnt = 0;
		for(int w : W)
			cnt += m[v][w];
		return cnt;
	}

	void insert(int u, int v) {
		m[u][v] = m[v][u] = 1;
		g[u].push_back(v);
		g[v].push_back(u);
	}

	vector<bool> permute(const vector<int>& pi) const {
		vector< vector<int> > t_m(n, vector<int>(n, 0));
		for(int i = 0; i < n; i++)
			for(int j = i + 1; j < n; j++) {
				t_m[pi[i]][pi[j]] = m[i][j];
				t_m[pi[j]][pi[i]] = m[j][i];
			}
		vector<bool> ret;
		for(int i = 0; i < n; i++)
			for(int j = i + 1; j < n; j++)
				ret.push_back(t_m[i][j]);
		return ret;
	}

private:
	int n;
	vector< vector<int> > m;
	vector< vector<int> > g;
};

#endif
