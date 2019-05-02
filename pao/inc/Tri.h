#pragma once

#include <functional>
#include <queue>
#include <vector>
#include <string>
#include <assert.h>
#include <iostream>
#include <random>

#include "Definitions.h"
#include "Data.h"
#include "Config.h"
#include "cgTypes.h"

/* interact with triangle */
#ifdef __cplusplus
extern "C" {
	#include "../../triangle/triangle.h"
}
#endif

class Triangle {
public:
	Triangle(ul id=0, ul a=0, ul b=0, ul c=0, sl nAB=0, sl nBC=0, sl nCA=0)
	: id(id)
	, a(a), b(b), c(c)
	, nAB(nAB), nBC(nBC), nCA(nCA)	{}
	~Triangle() {}

	void setClassic(ul id_, ul a_, ul b_, ul c_, sl nBC_, sl nCA_, sl nAB_) {
		id=id_;a=a_;b=b_;c=c_;nAB=nAB_;nBC=nBC_;nCA=nCA_;
	}

	void setNeighbors(sl nBC_, sl nCA_, sl nAB_) {nAB=nAB_;nBC=nBC_;nCA=nCA_;}

	ul id;
	ul a, b, c;
	sl nAB, nBC, nCA;

	sl diagonalNeighbor(ul i) const {
		if(i == a) { return nBC; }
		if(i == b) { return nCA; }
		if(i == c) { return nAB; }
		return NIL;
	}

	ul getCWCorner(ul i) const {
		assert(i!=a && i!=b && i!=c);
		if(i == a) {return c;}
		if(i == b) {return a;}
		if(i == c) {return b;}
		return ULMAX;
	}
	ul getCCWCorner(ul i) const {
		assert(i!=a && i!=b && i!=c);
		if(i == a) {return b;}
		if(i == b) {return c;}
		if(i == c) {return a;}
		return ULMAX;
	}

	void updateNeibhorFromTo(sl nOld, sl nNew) {
		if(nAB == nOld) {nAB=nNew;}
		else if(nBC == nOld) {nBC=nNew;}
		else if(nCA == nOld) {nCA=nNew;}
	}

	bool isValid() { return a != b && b != c && a != c; }

	friend std::ostream& operator<<(std::ostream& os, const Triangle& dt);
	friend bool operator==(const Triangle& a, const Triangle& b);
	friend bool operator!=(const Triangle& a, const Triangle& b);
};

class FlipElement {
public:
	FlipElement(const EdgeIterator eidx, const Exact area)
	: edgeIt(eidx)
	, areaChange(area) {}

	EdgeIterator 	edgeIt;
	Exact			areaChange;

	friend bool operator<(const FlipElement& lhs, const FlipElement& rhs);
};

struct CompareFlipElement : public std::binary_function<FlipElement, FlipElement, bool> {
	bool operator()(const FlipElement& lhs, const FlipElement& rhs) const {
		return lhs.areaChange < rhs.areaChange;
	}
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

	~Tri() {delete triswitches; }

	void runTriangle(Data& data);

	void aSingleFlip();

	void resetForSortedFlipping();

	bool isEmpty() { return tOUT.numberofpoints == 0; }
	bool isTriangulationDone() { return triangulationDone; }

	Triangle getTriangle(ul idx) const {
		assert(idx < (ul)tOUT.numberoftriangles);
		return Triangle(idx,tOUT.trianglelist[idx*3],tOUT.trianglelist[idx*3 + 1],tOUT.trianglelist[idx*3 + 2],
					    tOUT.neighborlist[idx*3 + 2],tOUT.neighborlist[idx*3],tOUT.neighborlist[idx*3 + 1]);
	}

	Triangle findTriangleWithCorner(const ul idx) const;

	void writeBack(const Triangle& tri);

	Triangle getNextCCWTriangleAroundVertex(const Triangle& tri, ul vertex) const;
	Triangle getNextCWTriangleAroundVertex(const Triangle& tri, ul vertex) const;

	Exact getArea(const Triangle& tri) const;

	bool isTriOnBoundaryABAndReflexVertexC(const Triangle& tri) const;
	bool isTriIcidentOnReflexVertexAndBoundary(const Triangle& tri) const;

	bool triLiesOnReflexVertex(const Triangle& t) const {
		return     data->isReflexVertex(t.a)
				|| data->isReflexVertex(t.b)
				|| data->isReflexVertex(t.c);
	}

	bool isFlippable(const Triangle& tri, ul vertex) const;
	bool isInvertable(const Triangle& tri, ul vertex) const;

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

	bool hasCorner(const Triangle& tri, const ul a) const {
		return tri.a == a || tri.b == a || tri.c == a;
	}

	void updateModifiedCorner(const EdgeIterator edgeIt);
	void updateModifiedCorners(const EdgeIterator twoIncident, const EdgeIterator nextThree);

	bool isOnVertices(const Triangle& tri, const ul a, const ul b, const ul c) const {
		return hasCorner(tri,a) &&  hasCorner(tri,b) && hasCorner(tri,c);
	}

	std::vector<Edge> getTriangleEdgesNotOnInput(ul idx) const {
		Triangle t = getTriangle(idx);
		Edge a(getPoint(t.a), getPoint(t.b));
		Edge b(getPoint(t.b), getPoint(t.c));
		Edge c(getPoint(t.c), getPoint(t.a));

		std::vector<Edge> ar;
		if(!data->hasEdge(t.a,t.b)) {ar.push_back(a);}
		if(!data->hasEdge(t.b,t.c)) {ar.push_back(b);}
		if(!data->hasEdge(t.c,t.a)) {ar.push_back(c);}
		return ar;
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

	ul vA(const EdgeIterator it) const {return (*it)[0];}
	ul vB(const EdgeIterator it) const {return (*it)[1];}
	Point p(const ul idx) const {return data->v(idx);}

	const triangulateio* getTriangleData() const { return &tOUT; }

	void repairTriangulationOn(std::list<ul> tris, const ul vertex);

	void reflexSensitiveFlipping(Triangle tri, const ul vertex);

	void flipPair(Triangle ta, Triangle tb);
	void flipPair(ul a, ul b) {flipPair(getTriangle(a),getTriangle(b));}

	std::array<ul,2> getCommonPair(const Triangle& ta, const Triangle& tb) const;

	ul getMissingCorner(const Triangle& tri, ul x, ul y) const {
		std::array<ul,3> array = {{tri.a,tri.b,tri.c}};
		for(auto i : array) {
			if(i != x && i != y) {
				return i;
			}
		}
		assert(false);
		return 0;
	}

	Triangle getNeighborWithoutVertex(const Triangle t, const ul vertex);

	bool isConvexQuad(const Triangle& ta, const Triangle& tb) const;

	void setConfig(Config* config_)  {config = config_;}

	void setMaximizingStrategy() 	 {maximizing = true;}
	void setMinimizingStrategy() 	 {maximizing = false;}
	void setReflexSensitive(bool rs) {isReflexSensitiveFlipping = rs;}
	void setSortingStratey(bool rs)  {sortingStrategyEnabled = rs;   }
	void setRandomSelection(bool rs) {randomSelection = rs;          }
	void setInverterSelection(bool rs) {inverterEnabled = rs;          }

	void printTriangles() const;
	void printEdges() const;

	void setFlippingDone() {flippingDone = true;}
	bool isFlippingDone() const {return flippingDone; }
	ul getFlipCnt() const {return flipCnt;		}
	bool hasReflexVertices() const {return !allReflexVertices.empty();}

	bool isStillSorting() const { return sortingStrategyEnabled && !sortingDone; }
	bool isSortingDone() const { return sortingDone; }
	bool isFlipQueueEmpty() const {return flipQueue.empty();}
	//void setReflexVertices(std::vector<EdgeIterator> list) {allReflexVertices = list;}
	void setReflexVertices() {
		std::vector<EdgeIterator> list = data->identifiyReflexVertices();
		if(randomSelection) {
			std::shuffle(list.begin(), list.end(), std::mt19937{ rd() });
		}
		allReflexVertices = list;
	}

private:
	void filltriangulateioIn(Data& data, triangulateio& tri);
	void inittriangulateioOut(Data& data, triangulateio& tri);

	bool getBestTriAroundVertex(const ul vertex, Triangle tri, const IndexEdge& a, const IndexEdge& b,
			std::list<ul>& outsideTrisToRepair, ul& triIdxChosen, std::vector<ul>& triIndices);

	void applyPolygonalFlip(const Triangle& tri, EdgeIterator edgeIt, const ul vertex,
			std::list<ul>& outsideTrisToRepair);

	bool haveEnteredPolyCCW(const Triangle& tri, const IndexEdge& ie) const;
	bool haveLeftPolyCCW(const Triangle& tri, const IndexEdge& ie) const;

	triangulateio triangleIN, tOUT, vorout;
	Data* data = nullptr;
	Config* config = nullptr;

	char *triswitches;
	bool triangulationDone = false;
	bool flippingDone = false;

	std::vector<EdgeIterator> allReflexVertices;
	ul flipCnt   = 0;

	bool maximizing 				= true;
	bool isReflexSensitiveFlipping 	= false;
	bool inverterEnabled			= false;
	bool randomSelection 			= false;
	std::random_device rd;


	/* for sorting strategy '-sort' argument */
	bool sortingStrategyEnabled = false;
	bool sortingDone 			= false;
    std::priority_queue<FlipElement, std::vector<FlipElement>, CompareFlipElement> flipQueue;

};
