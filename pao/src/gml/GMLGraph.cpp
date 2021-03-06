
#include "GMLGraph.h"


GMLGraph
GMLGraph::
create_from_graphml(std::istream &istream) {
  typedef GMLGraph::vertex_property_type VertexPropertyType;
  typedef GMLGraph::edge_property_type   EdgePropertyType;

  GMLGraph graph;
  graph.dp.property("vertex-coordinate-x", boost::get(&VertexPropertyType::x, graph));
  graph.dp.property("vertex-coordinate-y", boost::get(&VertexPropertyType::y, graph));
  graph.dp.property("edge-weight",          boost::get(&EdgePropertyType::weight, graph));
  graph.dp.property("edge-weight-additive", boost::get(&EdgePropertyType::weight_additive, graph));

  boost::read_graphml(istream, graph, graph.dp);
  return graph;
}


std::ostream&
operator<<(std::ostream& os, const GMLGraph& g) {
  typedef GMLGraph::vertex_descriptor VertexType;
  auto index_map = boost::get(boost::vertex_index, g);

  os << "vertices(g):" << std::endl;
  for (auto vp = boost::vertices(g); vp.first != vp.second; ++vp.first) {
    const VertexType v = *vp.first;
    os << index_map[v] <<  " " << g[v].x << " " << g[v].y << std::endl;
  }
  return os;
}
