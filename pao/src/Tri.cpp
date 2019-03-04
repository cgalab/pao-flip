
#include "Tri.h"

std::ostream& operator<<(std::ostream& os, const Triangle& dt) {
    os << "(" << dt.id << ") " << dt.a << ',' << dt.b << ',' << dt.c << " N[" << dt.nAB << "," << dt.nBC << "," << dt.nCA << "]";
    return os;
}
bool operator==(const Triangle& a, const Triangle& b) {return a.id == b.id;}
bool operator!=(const Triangle& a, const Triangle& b) {return a.id != b.id;}

void Tri::runTriangle(Data& data) {
	this->data = &data;

	/* init/fill the triangle structure with data */
	filltriangulateioIn( data,triangleIN);
	inittriangulateioOut(data,tOUT 	    );

	/* triangulate */
	triangulate(triswitches,&triangleIN,&tOUT,&vorout);

	triangulationDone = true;
}

void Tri::aSingleFlip() {

	if(!trisOnReflexVertex.empty()) {
		/* sort reflex triangle indeces*/
		//std::sort(trisOnReflexVertex.begin(),trisOnReflexVertex.end(),std::greater<>());

//		ul selectIdx = (ul)(std::rand() % trisOnReflexVertex.size());
//		auto tri = getTriangle(trisOnReflexVertex[selectIdx]);
//		trisOnReflexVertex.erase(trisOnReflexVertex.begin()+selectIdx);

		/* obtain last triangle */
		ul selectIdx = trisOnReflexVertex.back();

		if(config->verbose) {LOG(INFO) << "triangle " << selectIdx;}

		trisOnReflexVertex.pop_back();
		auto tri = getTriangle(selectIdx);
		if(!triLiesOnReflexVertex(tri)) {
			if(config->verbose) {
				LOG(INFO) << "triangle " << tri << " no longer on reflex vertex.";
			}
			return;
		}
		ul vertex = getReflexIndex(tri);
		if(!data->isReflexVertex(vertex)) {
			if(config->verbose) {
				LOG(INFO) << "vertex " << vertex << " no longer reflex.";
			}
			return;
		}

		auto edgeIt = data->findEdgeBefore(vertex);

		assert(edgeIt != data->getPolygon().end());

		IndexEdge a = *edgeIt;
		edgeIt = data->nextEdge(edgeIt);
		IndexEdge b = *edgeIt;


		if(config->verbose) {
			std::cout << "circle around: " << vertex << " edge vertices: " << a[0] << " "
					  << b[0] << " " << b[1] << std::endl;
		}

		bool inPoly;
		auto triStart = tri;
		/* find out if we are inside or outside*/
		do {
			if((tri.a == vertex && b[1] == tri.b) ||
			   (tri.b == vertex && b[1] == tri.c) ||
			   (tri.c == vertex && b[1] == tri.a) ) {
				inPoly = true;
				break;
			}
			if((tri.a == vertex && a[0] == tri.b) ||
			   (tri.b == vertex && a[0] == tri.c) ||
			   (tri.c == vertex && a[0] == tri.a) ) {
				inPoly = false;
				break;
			}

			tri = getNextCCWTriangleAroundVertex(tri,vertex);
		} while( tri.id != triStart.id );

		auto outSideArea = CGAL::area(data->v(a[0]),data->v(b[1]),data->v(b[0]));
		Exact triArea = 0.0;
		ul triIdx = tri.id;

		/* if a vertex is above line a[0]-b[1], vertex inside(on outside) */
		bool isValidVertex = true;
		Line referenceLine(data->v(a[0]),data->v(b[1]));

		triStart = tri;
		do {
			if((tri.a == vertex && b[1] == tri.b) ||
			   (tri.b == vertex && b[1] == tri.c) ||
			   (tri.c == vertex && b[1] == tri.a) ) {
				inPoly = true;
			}
			if((tri.a == vertex && a[0] == tri.b) ||
			   (tri.b == vertex && a[0] == tri.c) ||
			   (tri.c == vertex && a[0] == tri.a) ) {
				inPoly = false;
			}

			if(inPoly) {
				if(isFlippable(tri, vertex)) {
					/* find max area triangle */
					auto area = getArea(tri);
					if(maximizing) {
						if(triArea < area) {
							triArea = area;
							triIdx  = tri.id;
						}
					} else {
						if(triArea < area) {
							triArea = area;
							triIdx  = tri.id;
						}
					}
				}
			} else {
				/* check outside triangles */
				Point check;
				if(tri.a == vertex) {
					check = data->v(tri.b);
				} else if(tri.b == vertex) {
					check = data->v(tri.c);
				} else if(tri.c == vertex) {
					check = data->v(tri.a);
				}
				if(referenceLine.has_on_positive_side(check)) {
					isValidVertex = false;
				}
			}

			tri = getNextCCWTriangleAroundVertex(tri,vertex);
		} while( tri.id != triStart.id && isValidVertex );

		/* change polygon "flip" */
		if(isValidVertex && triArea > 0.0) {
			if( (maximizing && triArea < outSideArea) ||
			   (!maximizing && triArea > outSideArea)) {
				tri = getTriangle(triIdx);

				if(config->verbose) {
					std::cout << "looking good for " << vertex << std::endl;
					std::cout << "area change: " << CGAL::abs(outSideArea-triArea) << std::endl;
					std::cout << "tri " << tri << std::endl;
				}

				edgeIt = data->prevEdge(edgeIt);
				data->removePolygonCorner(edgeIt);

				auto indexEdge = getBoundaryEdge(tri);
				auto edgeItA = data->findEdgeBefore(indexEdge[0]);
				auto edgeItB = data->findEdgeBefore(indexEdge[1]);

				if(data->isEqual(*edgeItB,indexEdge)) {
					edgeItA = edgeItB;
				}

				data->addPolygonCorner(edgeItA, vertex);

				/* TODO: expensive ... just update the 4 vertices! */
				data->identifyConvexReflexInputVertices();
			}
		}
	} else {
		if(--lookupTriangles > 0) {
			identifyTrisOnReflexInputVertices();
		} else {
			flippingDone = true;
		}
	}
}

bool Tri::isFlippable(const Triangle& tri, ul vertex) const {
	if(tri.a == vertex) {
		return data->hasEdge(tri.b,tri.c)
				&& !data->hasEdge(tri.c,tri.a)
				&& !data->hasEdge(tri.a,tri.b);
	} else if(tri.b == vertex) {
		return data->hasEdge(tri.c,tri.a)
				&& !data->hasEdge(tri.a,tri.b)
				&& !data->hasEdge(tri.b,tri.c);

	} else if(tri.c == vertex) {
		return data->hasEdge(tri.a,tri.b)
				&& !data->hasEdge(tri.b,tri.c)
				&& !data->hasEdge(tri.c,tri.a);

	}
	return false;
}


void Tri::identifyTrisOnReflexInputVertices() {
	for(ul i=0; i < tOUT.numberoftriangles; ++i) {
		auto tri = getTriangle(i);
		if(isTriOnBoundaryABAndReflexVertexC(tri)) {
			trisOnReflexVertex.push_back(i);
		}
	}
}

Exact Tri::getArea(const Triangle& tri) const {
	return CGAL::area(getPoint(tri.a),getPoint(tri.b),getPoint(tri.c));
}

Triangle Tri::getNextCCWTriangleAroundVertex(const Triangle& tri, ul vertex) const {
	if(vertex == tri.a) {
		return getTriangle(tri.nCA);
	} else if(vertex == tri.b) {
		return getTriangle(tri.nAB);
	} else if(vertex == tri.c) {
		return getTriangle(tri.nBC);
	}
	assert(false);
	return Triangle();
}

bool Tri::isTriOnBoundaryABAndReflexVertexC(const Triangle& tri) const {
	if((data->hasEdge(tri.a,tri.b) && data->isReflexVertex(tri.c)) ||
	   (data->hasEdge(tri.b,tri.c) && data->isReflexVertex(tri.a)) ||
	   (data->hasEdge(tri.c,tri.a) && data->isReflexVertex(tri.b))
	) {
		return true;
	}
	return false;
}

bool Tri::isTriIcidentOnReflexVertexAndBoundary(const Triangle& tri) const {
	if((data->hasEdge(tri.a,tri.b) && (data->isReflexVertex(tri.a) || data->isReflexVertex(tri.b) ) ) ||
  	   (data->hasEdge(tri.b,tri.c) && (data->isReflexVertex(tri.b) || data->isReflexVertex(tri.c) ) ) ||
  	   (data->hasEdge(tri.c,tri.a) && (data->isReflexVertex(tri.c) || data->isReflexVertex(tri.a) ) )
	) {
		return true;
	}
	return false;
}

void Tri::filltriangulateioIn(Data& data, triangulateio& tri) {
	auto& vertices 		= data.getVertices();
	auto& polygon  		= data.getPolygon();

	tri.numberofpoints	= vertices.size();
	tri.pointlist    	= new double[tri.numberofpoints * 2];
	tri.pointmarkerlist = new int[tri.numberofpoints];

	for(ul i = 0; i < vertices.size(); ++i) {
		tri.pointlist[2*i]     = vertices[i].x();
		tri.pointlist[2*i + 1] = vertices[i].y();
		/* one is default for CH boundary vertices */
		tri.pointmarkerlist[i] = i+2;
	}

	tri.numberofholes 			= 0;
	tri.holelist				= NULL;
	tri.numberofregions			= 0;
	tri.regionlist				= NULL;
	tri.numberofcorners			= 3;

	tri.numberofpointattributes = 0;
	tri.pointattributelist 		= NULL;

	tri.numberofsegments = polygon.size();
	tri.segmentlist = new int[tri.numberofsegments * 2];
	tri.segmentmarkerlist = new int[tri.numberofsegments];
	for(ul i = 0; i < polygon.size(); ++i) {
		tri.segmentlist[2*i]     = polygon[i][0];
		tri.segmentlist[2*i + 1] = polygon[i][1];

		/* one is default for CH boundary segments */
		tri.segmentmarkerlist[i] = i+2;
	}
}

void Tri::inittriangulateioOut(Data& data, triangulateio& tri) {
	auto& vertices   	  = data.getVertices();
	tri.numberofpoints	  = vertices.size();
	tri.pointlist    	  = NULL; //new double[tri.numberofpoints * 2];
	tri.pointmarkerlist   = NULL; //new int[tri.numberofpoints];

	assert(vertices.size() > 2);
	tri.numberoftriangles = vertices.size() - 2;
	tri.trianglelist 	  = NULL; //new int[tri.numberoftriangles * 3];
	tri.neighborlist 	  = NULL; //new int[tri.numberoftriangles * 3];
	tri.numberofcorners	  = 3;

	tri.numberofedges 	  = 2 * vertices.size() - 2;
	tri.edgelist 		  = NULL; //new int[tri.numberofedges * 2];
	tri.edgemarkerlist 	  = NULL; //new int[tri.numberofedges];

	auto& polygon  	 	  = data.getPolygon();
	tri.numberofsegments  = polygon.size();
	tri.segmentlist 	  = NULL; //new int[tri.numberofsegments * 2];
	tri.segmentmarkerlist = NULL; //new int[tri.numberofsegments];
}


void Tri::printTriangles() const {
	for (ul i = 0; i < tOUT.numberoftriangles; ++i) {
		Triangle t = getTriangle(i);
		std::cout << t << " ";
	}
	std::cout << std::endl;
}


void Tri::printEdges() const {
	std::cout << "edgelist idx: ";
	for (ul i = 0; i < tOUT.numberofedges*2; ++i) {
		std::cout << tOUT.edgelist[i] << ", ";
	}
	std::cout << std::endl;

	std::cout << std::endl;for (ul i = 0; i < tOUT.numberofedges; ++i) {
		Edge e = getEdge(i);
		std::cout << e << " / ";
	}
	std::cout << std::endl;
}


