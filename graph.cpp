#include <vector>
#include <queue>
#include <algorithm>
#include <string>

#include "graph.hpp"
#include "permutation.hpp"
#include "utility.hpp"

using std::vector;
using std::queue;
using std::string;
using std::pair;

graph::graph() {
	n = 0;
}

graph::graph(int n) {
	this->n = n;
	m = vector< vector<int> >(n, vector<int>(n, 0));
	g = vector< vector<int> >(n);
}

int graph::count(int v, const vector<int>& W) const {
	int cnt = 0;
	for(int w : W)
		cnt += m[v][w];
	return cnt;
}

uint32_t graph::dvector_hash(int v, const vector<int>& W) const {
	uint32_t hash = 0;
	for(int w : W)
		hash = (hash + modpow(1000003, d[v][w], 1000000007)) % 1000000007;
	return hash;
}

void graph::insert(int u, int v) {
	if(m[u][v] == 0) {
		m[u][v] = m[v][u] = 1;
		g[u].push_back(v);
		g[v].push_back(u);
	}
}

vector<bool> graph::permuted(const permutation& pi) const {
	vector<bool> ret(n * (n - 1) / 2);
	int i = 0, j = 1;
	for(int k = 0; k < ret.size(); k++) {
		ret[k] = m[pi.i(i)][pi.i(j)];
		if(++j == n) {
			i++;
			j = i + 1;
		}
	}
	return ret;
}

void graph::relabel(const permutation& pi) {
	vector< pair<int, int> > edges;
	for(int i = 0; i < n; i++)
		g[i].clear();
	for(int i = 0; i < n; i++)
		for(int j = i + 1; j < n; j++) {
			if(m[i][j])
				edges.push_back({pi[i], pi[j]});
			m[i][j] = m[j][i] = 0;
		}
	for(auto e : edges)
		insert(e.first, e.second);
}

void graph::init_distances() {
	d = m;
	for(int i = 0; i < n; i++)
		for(int j = i + 1; j < n; j++)
			d[i][j] = d[j][i] = n;
	queue<int> q;
	for(int v = 0; v < n; v++) {
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

int graph::v_count() const {
	return n;
}

istream& operator>>(istream& in, graph& g) {
	string s;
	int v, e, a, b;
	in >> s; in >> s;
	in >> v >> e;
	
	g = graph(v);

	while(e--) {
		in >> s >> a >> b;
		g.insert(a - 1, b - 1);
	}
	return in;
}
