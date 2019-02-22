#pragma once

#include <map>
#include <set>
#include <utility>

#include "GMLGraph.h"
#include "Definitions.h"

#ifndef CORE_ONE
#define CORE_ONE NT(1.0)
#endif

class BasicVertex {
public:
	const Point p;
	const unsigned degree;
	const uint id;
	const unsigned reflex_beveling_add;

	BasicVertex(const Point& p, unsigned degree, uint id)
	: p(p)
	, degree(degree)
	, id(id)
	, reflex_beveling_add(0) // number of kinetic vertices to create additionally at reflex vertices.
	// At degree one vertices, we always add at least one.
	{}
};

class BasicEdge {
public:
	uint u, v;
	uint id;

	BasicEdge(uint u, uint v, uint id)
	: u(u)
	, v(v)
	, id(id) {}
};

class BasicInput {
	using VertexList     = std::vector<BasicVertex>;
	using EdgeList       = std::vector<BasicEdge>;
	using VertexIdxPair  = std::pair<unsigned,unsigned>;

	using PointList      = std::vector<Point>;
	using SimpleEdgeList = std::vector<std::array<uint,2> >;

private:
	unsigned num_of_deg1_vertices = 0;
	VertexList vertices_;
	EdgeList edges_;
	std::map<VertexIdxPair, unsigned> edge_map;
	/* keep a list of instances of our number type for the different weights. */

	void assert_valid() const;

public:
	/** Add an input vertex to the vertexlist */
	inline void add_vertex(BasicVertex&& p) {
		vertices_.emplace_back(std::forward<BasicVertex>(p));
	}
	/** Add an input edge between vertices to the edgelist */
	inline void add_edge(unsigned u, unsigned v) {
		sort_tuple(u,v);
		assert(u < vertices_.size());
		assert(v < vertices_.size());
		assert(u!=v);

		edges_.emplace_back(BasicEdge(u,v,edges_.size()));
		auto res = edge_map.emplace(std::pair<VertexIdxPair,unsigned>(VertexIdxPair(u,v), edges().size()-1));
		assert(res.second);
	}

	const VertexList& vertices() const { return vertices_; };
	const EdgeList& edges() const { return edges_; };
	void add_graph(const GMLGraph& graph);
	void add_list(const PointList& points, const SimpleEdgeList& edges);
	unsigned get_num_of_deg1_vertices() const {
		return num_of_deg1_vertices;
	}
	unsigned get_total_degree() const {
		return edges().size() * 2;
	}
	unsigned get_num_extra_beveling_vertices() const {
		/* Includes the extra one vertex we'll need at a minimum for degree-1 vertices. */
		return num_of_deg1_vertices;
	}
	bool has_edge(unsigned u, unsigned v) const {
		sort_tuple(u,v);
		auto findres = edge_map.find(VertexIdxPair(u, v));
		return findres != edge_map.end();
	}
	const BasicEdge& get_edge(unsigned u, unsigned v) const {
		sort_tuple(u,v);
		assert(has_edge(u,v));
		auto findres = edge_map.find(VertexIdxPair(u, v));
		assert(findres != edge_map.end());
		return edges_[findres->second];
	}

	void update_edge(unsigned idx, unsigned u, unsigned v) {
		auto edge = &edges_[idx];
		sort_tuple(u,v);
		edge->u = u;
		edge->v = v;
	}

	Edge get_segment(const BasicEdge& e) const {
		return Edge(vertices_[e.u].p, vertices_[e.v].p);
	}
};
