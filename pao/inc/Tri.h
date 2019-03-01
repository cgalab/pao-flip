#pragma once

/* interact with triangle */

#include <string>
#include <assert.h>

#include "Definitions.h"
#include "Data.h"
#include "cgTypes.h"

#ifdef __cplusplus
extern "C" {
	#include "../../triangle/triangle.h"
}
#endif

class Triangle {
public:
	Triangle(ul a=0, ul b=0, ul c=0)
	: a(a)
	, b(b)
	, c(c) {}
	~Triangle() {}
	const ul a, b, c;

	friend std::ostream& operator<<(std::ostream& os, const Triangle& dt);
};

class Tri {
public:
	Tri() {
		/* z..index vertices from zero
		 * p..Triangulate (PSLG)
		 * n..compute triangle neighbors
		 * V..Verbose
		 * Q..quiet
		 * */
		std::string mysw = "pznYVce";
		triswitches = new char[mysw.length()+1];
		strcpy(triswitches,mysw.c_str());
	}

	~Tri() {delete triswitches;}

	void runTriangle(Data& data);

	void testSomeFlips();

	void identifyTrisOnReflexInputVertices();

	bool isEmpty() { return tOUT.numberofpoints == 0; }
	bool isTriangulationDone() { return triangulationDone; }

	Triangle getTriangle(ul idx) const {
		return Triangle(tOUT.trianglelist[idx*3],tOUT.trianglelist[idx*3 + 1],tOUT.trianglelist[idx*3 + 2]);
	}

	bool isTriOnBoundaryAndReflexVertex(const Triangle& tri) const;

	bool triLiesOnReflexVertex(const ul& idx) const {
		auto t = getTriangle(idx);
		return     data->isReflexVertex(t.a)
				|| data->isReflexVertex(t.b)
				|| data->isReflexVertex(t.c);
	}

	std::array<Edge,3> getTriangleEdges(ul idx) const {
		Triangle t = getTriangle(idx);
		Edge a(getPoint(t.a), getPoint(t.b));
		Edge b(getPoint(t.b), getPoint(t.c));
		Edge c(getPoint(t.c), getPoint(t.a));
		return {{a,b,c}};
	}

	Edge getEdge(ul idx) const {
		ul PaIdx = tOUT.edgelist[idx*2];
		ul PbIdx = tOUT.edgelist[idx*2+1];

		std::cout << " idx: " << idx << " Paidx: " << PaIdx << " Pbidx: " << PbIdx << std::endl;
		Point a(tOUT.pointlist[PaIdx], tOUT.pointlist[PaIdx+1]);
		Point b(tOUT.pointlist[PbIdx], tOUT.pointlist[PbIdx+1]);
		return Edge(a,b);
	}

	Point getPoint(ul idx) const {
		return Point(tOUT.pointlist[idx*2], tOUT.pointlist[idx*2 + 1]);
	}

	const triangulateio* getTriangleData() const { return &tOUT; }

	void setMaximizingStrategy() {maximizing = true;}
	void setMinimizingStrategy() {maximizing = false;}

	void printTriangles() const;
	void printEdges() const;

//	/* for gui */
//	void fillEdges() {
//		for (ul i = 0; i < tOUT.numberofedges; ++i) {
//			edges.push_back(getEdge(i));
//		}
//	}
//
//	std::vector<Edge> edges;

private:
	void filltriangulateioIn(Data& data, triangulateio& tri);
	void inittriangulateioOut(Data& data, triangulateio& tri);

	triangulateio triangleIN, tOUT, vorout;
	Data* data = nullptr;
	char *triswitches;
	bool triangulationDone = false;

	bool maximizing = true;
	std::vector<int> trisOnReflexVertex;
};
