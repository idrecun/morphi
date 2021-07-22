#ifndef __GRAPH_INVARIANT_HPP__
#define __GRAPH_INVARIANT_HPP__

#include <vector>

#include "graph.hpp"
#include "utility.hpp"

class graph;

class graph_invariant {
	public:
		virtual void calculate() = 0;
		virtual uint32_t get(int v, const cell_data& W) const = 0;
};


class invariant_adjacent : public graph_invariant {
	public:
		invariant_adjacent(const graph& g);
		void calculate() override;
		uint32_t get(int v, const cell_data& W) const override;
	private:
		const graph& g;
};

class invariant_bitvector : public graph_invariant {
	public:
		invariant_bitvector(const graph& g);
		void calculate() override;
		uint32_t get(int v, const cell_data& W) const override;
	private:
		const graph& g;
		vector< vector<uint64_t> > adjacency_bitvector;
};

class invariant_distance : public graph_invariant {
	public:
		invariant_distance(const graph& g);
		void calculate() override;
		uint32_t get(int v, const cell_data& W) const override;
	private:
		const graph& g;
		compressed_matrix distance_matrix;
};

class invariant_paths : public graph_invariant {
	public:
		invariant_paths(const graph& g, int d);
		void calculate() override;
		uint32_t get(int v, const cell_data& W) const override;
	private:
		int d;
		const graph& g;
		vector< vector<uint32_t> > path_matrix;
};

#endif
