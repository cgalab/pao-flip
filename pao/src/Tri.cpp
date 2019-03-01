
#include "Tri.h"

std::ostream& operator<<(std::ostream& os, const Triangle& dt) {
    os << dt.a << ',' << dt.b << ',' << dt.c;
    return os;
}

void Tri::runTriangle(Data& data) {
	this->data = &data;

	/* init/fill the triangle structure with data */
	filltriangulateioIn( data,triangleIN);
	inittriangulateioOut(data,tOUT 	    );

	/* triangulate */
	triangulate(triswitches,&triangleIN,&tOUT,&vorout);

	triangulationDone = true;
}

void Tri::testSomeFlips() {
	identifyTrisOnReflexInputVertices();

}




void Tri::identifyTrisOnReflexInputVertices() {
	for(ul i=0; i < tOUT.numberoftriangles; ++i) {
		if(triLiesOnReflexVertex(i)) {
			trisOnReflexVertex.push_back(i);
		}
	}
}

bool Tri::isTriOnBoundaryAndReflexVertex(const Triangle& tri) const {
	if((data->hasEdge(tri.a,tri.b) && data->isReflexVertex(tri.c)) ||
	   (data->hasEdge(tri.b,tri.c) && data->isReflexVertex(tri.a)) ||
	   (data->hasEdge(tri.c,tri.a) && data->isReflexVertex(tri.b))
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


