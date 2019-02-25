#include "BasicInput.h"

void BasicInput::add_graph(const GMLGraph& graph) {
	assert(vertices_.size() == 0);
	assert(edges_.size() == 0);

	typedef GMLGraph::vertex_descriptor VertexType;
	typedef GMLGraph::edge_descriptor EdgeType;

	auto index_map = boost::get(boost::vertex_index, graph);

	for (auto vp = boost::vertices(graph); vp.first != vp.second; ++vp.first) {
		const VertexType v = *vp.first;
		Point p(std::atof(graph[v].x.c_str()), std::atof(graph[v].y.c_str()));
		ul degree = boost::degree(v, graph);
		add_vertex(BasicVertex(p, degree, vertices_.size()));
		assert(index_map[v] == vertices_.size()-1);
		if (degree == 1) num_of_deg1_vertices++;
	}
	for (auto ep = boost::edges(graph); ep.first != ep.second; ++ep.first) {
		const EdgeType e = *ep.first;
		add_edge(source(e, graph), target(e, graph));
	}

	assert_valid();
}

void BasicInput::assert_valid() const {
	ul* d = new ul[vertices_.size()]();
	ul deg1 = 0;

	assert(edge_map.size() == edges_.size());
	for (size_t i=0; i<edges_.size(); ++i) {
		const auto & e = edges_[i];
		assert(e.u < vertices_.size());
		assert(e.v < vertices_.size());
		d[e.u]++;
		d[e.v]++;
		auto findres = edge_map.find(VertexIdxPair(e.u,e.v));
		assert(findres != edge_map.end());
		assert(findres->second == i);
	}
	for (size_t i=0; i<vertices_.size(); ++i) {
		const auto & v = vertices_[i];
		assert(d[i] == v.degree);
		if (d[i] == 1) deg1++;
	}
	assert(deg1 == num_of_deg1_vertices);
	delete[] d;
}

void BasicInput::add_list(const PointList& points, const SimpleEdgeList& edges) {
	for(auto point : points) {
		add_vertex(BasicVertex(point,2,vertices_.size()));
	}
	for(auto edge : edges) {
		add_edge(edge[0],edge[1]);
	}
}
