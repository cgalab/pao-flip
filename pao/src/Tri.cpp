
#include "Tri.h"

void Tri::runTriangle(const Data& data) {
std::cout << "a"; fflush(stdout);
	/* init/fill the triangle structure with data */
	inittriangulateioOut(data,triangleIN );
	filltriangulateioIn( data,triangleOUT);

std::cout << "b"; fflush(stdout);
	/* triangulate */
	triangulate(triswitches,&triangleIN,&triangleOUT,&vorout);
std::cout << "c"; fflush(stdout);

	/* extract date */

}

void Tri::filltriangulateioIn(const Data& data, triangulateio& tri) {
	auto& vertices = data.getVertices();
	auto& polygon  = data.getPolygon();
	tri.pointlist = new double[vertices.size() * 2];

	for(ul i = 0; i < vertices.size(); ++i) {
		tri.pointlist[2*i] = vertices[i].x();
		tri.pointlist[2*i + 1] = vertices[i].y();
	}
	tri.numberofpoints = vertices.size();
	tri.numberofpointattributes = 0;
	tri.pointmarkerlist = NULL;

	tri.segmentlist = new int[polygon.size() * 2];
	for(ul i = 0; i < polygon.size(); ++i) {
		tri.segmentlist[2*i] =     polygon[i][0];
		tri.segmentlist[2*i + 1] = vertices[i][1];
	}
	tri.numberofsegments = polygon.size();
}

void Tri::inittriangulateioOut(const Data& data, triangulateio& tri) {
	auto& vertices = data.getVertices();
	tri.pointlist = NULL;
	assert(vertices.size() > 2);
	tri.trianglelist = new int[vertices.size() - 2];
	tri.neighborlist = new int[vertices.size() - 2];
	tri.numberoftriangles = vertices.size() - 2;
}
