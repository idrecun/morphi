#ifndef __GRAPH_INVARIANT_HPP__
#define __GRAPH_INVARIANT_HPP__

#include <vector>

#include "graph.hpp"

class graph;

class graph_invariant {
	public:
		virtual void calculate() = 0;
		virtual uint32_t get(int v, const vector<int>& W) const = 0;
};


class invariant_adjacent : public graph_invariant {
	public:
		invariant_adjacent(const graph& g);
		void calculate() override;
		uint32_t get(int v, const vector<int>& W) const override;
	private:
		const graph& g;
	//mozda cuvati matricu povezanosti kao bitove pa ubrzati preko toga?
};

class invariant_distance : public graph_invariant {
	public:
		invariant_distance(const graph& g);
		void calculate() override;
		uint32_t get(int v, const vector<int>& W) const override;
	private:
		const graph& g;
		vector< vector<int> > distance_matrix;
};

#endif
