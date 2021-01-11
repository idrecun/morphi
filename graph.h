#ifndef __GRAPH_H__
#define __GRAPH_H__

#include <vector>
#include <queue>

#include "permutation.h"

using std::vector;
using std::queue;

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

	uint32_t countD(int v, const vector<int>& W) const {
		vector<int> dists;
		for(int w : W)
			dists.push_back(d[v][w]);
		sort(dists.begin(), dists.end());

		uint32_t hash = dists.size();
		for(int x : dists)
			hash ^= (uint32_t) x + 0x9e3779b7 + (hash << 6) + (hash >> 2);
		return hash;
	}

	void insert(int u, int v) {
		m[u][v] = m[v][u] = 1;
		g[u].push_back(v);
		g[v].push_back(u);
	}

	vector<bool> permute(const permutation& pi) const {
		vector< vector<bool> > t_m(n, vector<bool>(n, 0));
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

	void init_distances() {
		d = m;
		for(int i = 0; i < n; i++)
			for(int j = i + 1; j < n; j++)
				d[i][j] = d[j][i] = n;
		for(int v = 0; v < n; v++) {
			queue<int> q;
			q.push(v);
			while(!q.empty()) {
				int u = q.front();
				q.pop();

				for(int x : g[u])
					if(d[v][x] == n) {
						d[v][x] = d[v][u] + 1;
						q.push(x);
					}
			}
		}
	}

private:
	int n;
	vector< vector<int> > m;
	vector< vector<int> > g;
	vector< vector<int> > d;
};

#endif
