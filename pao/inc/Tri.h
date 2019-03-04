#pragma once

/* interact with triangle */

#include <string>
#include <assert.h>

#include "Definitions.h"
#include "Data.h"
#include "Config.h"
#include "cgTypes.h"

#ifdef __cplusplus
extern "C" {
	#include "../../triangle/triangle.h"
}
#endif

class Triangle {
public:
	Triangle(ul id=0, ul a=0, ul b=0, ul c=0, ul nAB=0, ul nBC=0, ul nCA=0)
	: id(id)
	, a(a), b(b), c(c)
	, nAB(nAB), nBC(nBC), nCA(nCA)	{}
	~Triangle() {}

	ul id;
	ul a, b, c;
	sl nAB, nBC, nCA;

//	ul getThirdIdx(const ul x, const ul y) const {
//		std::set s = {a,b,c};
//		s.erase(x);
//		s.erase(y);
//		return *s.begin();
//	}

	friend std::ostream& operator<<(std::ostream& os, const Triangle& dt);
	friend bool operator==(const Triangle& a, const Triangle& b);
	friend bool operator!=(const Triangle& a, const Triangle& b);
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
		std::string mysw = "pznYQce";
		triswitches = new char[mysw.length()+1];
		strcpy(triswitches,mysw.c_str());
	}

	~Tri() {delete triswitches;}

	void runTriangle(Data& data);

	void aSingleFlip();

	void identifyTrisOnReflexInputVertices();

	bool isEmpty() { return tOUT.numberofpoints == 0; }
	bool isTriangulationDone() { return triangulationDone; }

	Triangle getTriangle(ul idx) const {
		assert(idx < tOUT.numberoftriangles);
		return Triangle(idx,tOUT.trianglelist[idx*3],tOUT.trianglelist[idx*3 + 1],tOUT.trianglelist[idx*3 + 2],
					    tOUT.neighborlist[idx*3 + 2],tOUT.neighborlist[idx*3],tOUT.neighborlist[idx*3 + 1]);
	}

	Triangle getNextCCWTriangleAroundVertex(const Triangle& tri, ul vertex) const;

	Exact getArea(const Triangle& tri) const;

	bool isTriOnBoundaryABAndReflexVertexC(const Triangle& tri) const;
	bool isTriIcidentOnReflexVertexAndBoundary(const Triangle& tri) const;

	bool triLiesOnReflexVertex(const Triangle& t) const {
		return     data->isReflexVertex(t.a)
				|| data->isReflexVertex(t.b)
				|| data->isReflexVertex(t.c);
	}

	bool isFlippable(const Triangle& tri, ul vertex) const;

	IndexEdge getBoundaryEdge(const Triangle& tri) const {
		if(data->hasEdge(tri.a,tri.b)) {
			return {{tri.a,tri.b}};
		}
		if(data->hasEdge(tri.b,tri.c)) {
			return {{tri.b,tri.c}};
		}
		if(data->hasEdge(tri.c,tri.a)) {
			return {{tri.c,tri.a}};
		}
		assert(false);
		return IndexEdge();
	}

	bool triLiesOnReflexVertex(const ul& idx) const {
		return triLiesOnReflexVertex(getTriangle(idx));
	}

	bool triOnBoundary(const Triangle& tri) const {
		return  data->hasEdge(tri.a,tri.b)
			 || data->hasEdge(tri.b,tri.c)
			 || data->hasEdge(tri.c,tri.a);
	}

	ul getReflexIndex(const Triangle& tri) const {
		assert(triLiesOnReflexVertex(tri));
		if(data->isReflexVertex(tri.a)) {
			return tri.a;
		} else if(data->isReflexVertex(tri.b)) {
			return tri.b;
		} else {
			return tri.c;
		}
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

		Point a(tOUT.pointlist[PaIdx], tOUT.pointlist[PaIdx+1]);
		Point b(tOUT.pointlist[PbIdx], tOUT.pointlist[PbIdx+1]);
		return Edge(a,b);
	}

	Point getPoint(ul idx) const {
		return Point(tOUT.pointlist[idx*2], tOUT.pointlist[idx*2 + 1]);
	}

	const triangulateio* getTriangleData() const { return &tOUT; }

	bool isFlippingDone() {return flippingDone; }

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

	void setConfig(Config* config_) {config = config_;}

private:
	void filltriangulateioIn(Data& data, triangulateio& tri);
	void inittriangulateioOut(Data& data, triangulateio& tri);

	triangulateio triangleIN, tOUT, vorout;
	Data* data = nullptr;
	Config* config = nullptr;

	char *triswitches;
	bool triangulationDone = false;
	bool flippingDone = false;

	ul lookupTriangles = 100;

	bool maximizing = true;
	std::vector<int> trisOnReflexVertex;
};
