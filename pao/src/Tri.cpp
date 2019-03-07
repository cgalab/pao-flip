
#include "Tri.h"

std::ostream& operator<<(std::ostream& os, const Triangle& dt) {
    os << "(" << dt.id << ") " << dt.a << ',' << dt.b << ',' << dt.c << " N[" << dt.nBC << "," << dt.nCA << "," << dt.nAB << "]";
    return os;
}
bool operator==(const Triangle& a, const Triangle& b) {return a.id == b.id;}
bool operator!=(const Triangle& a, const Triangle& b) {return a.id != b.id;}

bool operator<(const FlipElement& lhs, const FlipElement& rhs) {
  return lhs.areaChange < rhs.areaChange;
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

void Tri::resetForSortedFlipping() {
	if(sortingStrategyEnabled) {
		identifyTrisOnReflexInputVertices();

		flippingDone = false;
		sortingDone  = false;
	}
}

void Tri::aSingleFlip() {
	if(isStillSorting() && trisOnReflexVertex.empty()) {
		sortingDone = true;
	}

	if(!trisOnReflexVertex.empty() || (sortingStrategyEnabled && !flipQueue.empty())) {

		Triangle tri;
		EdgeIterator edgeIt;
		ul selectIdx, vertex;

		if(!sortingStrategyEnabled || (sortingStrategyEnabled && !sortingDone)) {
			/* obtain last triangle */

			if(randomSelection) {
				std::mt19937 gen(rd());
				std::uniform_int_distribution<> dis(0, trisOnReflexVertex.size()-1);
				ul pos = dis(gen);
				selectIdx = trisOnReflexVertex[pos];
				trisOnReflexVertex.erase(trisOnReflexVertex.begin() + pos);
			} else {
				selectIdx = trisOnReflexVertex.back();
				trisOnReflexVertex.pop_back();
			}

			tri = getTriangle(selectIdx);

			if(!triLiesOnReflexVertex(tri)) {
				if(config->verbose) {
					LOG(INFO) << "triangle " << tri << " no longer on reflex vertex.";
				}
				return;
			}
			vertex = getReflexIndex(tri);
		} else {
			do {
				/* we use sortingStrategy and we are done sorting */
				auto top = flipQueue.top();
				flipQueue.pop();
				vertex = top.vertexIdx;
			} while(!flipQueue.empty() && !data->isReflexVertex(vertex));
			tri = findTriangleWithCorner(vertex);
		}
		edgeIt = data->findEdgeBefore(vertex);

		if(!data->isReflexVertex(vertex)) {
			if(config->verbose) {
				LOG(INFO) << "vertex " << vertex << " no longer reflex.";
			}
			return;
		}

		if(!data->isNextVertexReflex(edgeIt)) {
			LOG(ERROR) << "vertex not reflex?";
			return;
		}

		assert(edgeIt != data->getPolygon().end());

		IndexEdge a = *edgeIt;
		IndexEdge b = *(data->nextEdge(edgeIt));

		if(config->verbose) {
			std::cout << "circle around: " << vertex << " edge vertices: " << a[0] << " "
					  << b[0] << " " << b[1] << std::endl;
			LOG(INFO) << "vertex: " << vertex;
		}

		/*************************************************************/
		/* iterates around 'vertex' and finds most extreme triangle  */
		/*************************************************************/
		ul triIdx = ULMAX;
		std::list<ul> outsideTrisToRepair;
		bool isValidVertex = getBestTriAroundVertex(vertex,tri,a,b, outsideTrisToRepair,triIdx);

		if(isValidVertex && triIdx != ULMAX) {
			tri = getTriangle(triIdx);
			Exact triArea = getArea(tri);
			Exact outSideArea = CGAL::area(data->v(a[0]),data->v(b[1]),data->v(b[0]));

			/*************************************************************/
			/* with '-sort' we first sort int a priority queue 			 */
			/* we do the actual flipping in an additional run 			 */
			/*************************************************************/
			if(isStillSorting()) {
				if(config->verbose) {
					LOG(INFO) << "addeing vertex " << (*edgeIt)[1] << " to flipqueue!";
				}
				Exact areaMod = (triArea > outSideArea) ? triArea-outSideArea : outSideArea-triArea;
				flipQueue.push( FlipElement(vertex,areaMod) );
			} else {
				/* update polygon "flip" (the actual flip) */
				if( (maximizing && triArea < outSideArea) ||
				   (!maximizing && triArea > outSideArea)) {

					if(config->verbose) {
						std::cout << "looking good for " << vertex ;
						std::cout << " // area out: " << outSideArea << " in tri: " << triArea ;
						std::cout << " // triangle " << tri << std::endl;
					}

					/* 'flipping-the-polygon' */
					applyPolygonalFlip(tri,edgeIt, vertex,outsideTrisToRepair);
					++flipCnt;


					if(!config->silent) {
						if(flipCnt%1000 == 0) {	std::cout << flipCnt; }
						else {std::cout << ".";	}
					}
				}
			}
		}
	} else {
		if(flipCnt > flipCheck && !sortingStrategyEnabled) {
			identifyTrisOnReflexInputVertices();
			flipCheck = flipCnt;
		} else {
			flippingDone = true;
		}
	}
}

bool Tri::getBestTriAroundVertex(const ul vertex, Triangle tri, const IndexEdge& a, const IndexEdge& b,
		std::list<ul>& outsideTrisToRepair, ul& triIdxChosen) {

	bool inPoly;
	auto triStart = tri;

	/********************************************************/
	/* 		find out if we are inside or outside			*/
	/********************************************************/
	do {
		if(haveEnteredPolyCCW(tri,b)) {inPoly = true;  break;}
		if(haveLeftPolyCCW(tri,a)) 	  {inPoly = false; break;}

		tri = getNextCCWTriangleAroundVertex(tri,vertex);
	} while( tri.id != triStart.id );


	/********************************************************/
	/* 		find out extreme triangle inside, check if		*/
	/* 		outside is an ear.								*/
	/********************************************************/
	auto outSideArea = CGAL::area(data->v(a[0]),data->v(b[1]),data->v(b[0]));
	Exact triArea = (maximizing) ? std::numeric_limits<Exact>::max() : 0.0;

	/* if a vertex is 'above' line a[0],b[1], vertex inside the outside ear */
	bool isValidVertex = true;
	Line referenceLine(data->v(a[0]),data->v(b[1]));

	triStart = tri;
	do {
		if(haveEnteredPolyCCW(tri,b)) {inPoly = true;}
		if(haveLeftPolyCCW(tri,a)) 	  {inPoly = false;}

		if(inPoly) {
			/* add -r argument when running pao-flip to activat this option */
			if(isReflexSensitiveFlipping) {
				auto tri_prev = getNextCWTriangleAroundVertex(tri,vertex);
				reflexSensitiveFlipping(tri,vertex);
				tri = getNextCCWTriangleAroundVertex(tri_prev,vertex);
			}

			if(isFlippable(tri, vertex)) {
				/* find max area triangle */
				auto area = getArea(tri);
				if(maximizing) {
					if(area < triArea) {
						triArea = area;
						triIdxChosen  = tri.id;
					}
				} else {
					if(area > triArea) {
						triArea = area;
						triIdxChosen  = tri.id;
					}
				}
			}
		} else {
			/* check outside triangles */
			Point check;
			ul checkIdx = ULMAX;
			if(tri.a == vertex) {
				checkIdx = tri.b;
				check = data->v(tri.b);
			} else if(tri.b == vertex) {
				checkIdx = tri.c;
				check = data->v(tri.c);
			} else if(tri.c == vertex) {
				checkIdx = tri.a;
				check = data->v(tri.a);
			}
			if(referenceLine.has_on_positive_side(check)) {
				isValidVertex = false;
			}
			/* check for collinearity of external boundary vertex */
			if(a[0] != checkIdx && b[1] != checkIdx &&
					CGAL::collinear(p(a[0]),check,p(b[1]))) {
				isValidVertex = false;
			}

			/* if outside tri is not a[0]a[1]b[1] */
			if(!isOnVertices(tri,a[0],a[1],b[1])) {
				if(config->verbose) {
					//LOG(INFO) << "outside tri " << tri << " added to repair stack";
				}
				outsideTrisToRepair.push_back(tri.id);
			}
		}

		/* iterate around revlex 'vertex' triangle by triangle */
		tri = getNextCCWTriangleAroundVertex(tri,vertex);

	} while( tri.id != triStart.id && isValidVertex );

	return isValidVertex;
}


Triangle Tri::findTriangleWithCorner(const ul idx) const {
	for(sl i = 0; i < (sl) tOUT.numberoftriangles; ++i) {
		auto tri = getTriangle(i);
		if(hasCorner(tri,idx)) {
			return tri;
		}
	}
	assert(false);
	return Triangle();
}

bool Tri::haveEnteredPolyCCW(const Triangle& tri, const IndexEdge& ie) const {
	if((tri.a == ie[0] && ie[1] == tri.b) ||
	   (tri.b == ie[0] && ie[1] == tri.c) ||
	   (tri.c == ie[0] && ie[1] == tri.a) ) {
		return true;
	}
	return false;
}

bool Tri::haveLeftPolyCCW(const Triangle& tri, const IndexEdge& ie) const {
	if( (tri.a == ie[1] && ie[0] == tri.b) ||
		(tri.b == ie[1] && ie[0] == tri.c) ||
		(tri.c == ie[1] && ie[0] == tri.a) ) {
			return true;
	}
	return false;
}

void Tri::applyPolygonalFlip(const Triangle& tri, EdgeIterator edgeIt, const ul vertex, std::list<ul>& outsideTrisToRepair) {
	if(config->verbose) {
		std::cout << "edgeit " << (*edgeIt)[0] << ", " << (*edgeIt)[1] << std::endl;
	}
	data->removePolygonCorner(edgeIt);

	auto indexEdge = getBoundaryEdge(tri);

	if(config->verbose) {
		std::cout << "tri-b-edge " << indexEdge[0] << ", " << indexEdge[1] << std::endl;
	}

	auto edgeItA = data->findEdgeIterator(indexEdge);
	data->addPolygonCorner(edgeItA, vertex);

	if(config->verbose) {LOG(INFO) << "flip " << flipCnt;}

	if(outsideTrisToRepair.size() > 0) {
		repairTriangulationOn(outsideTrisToRepair, vertex);
	}

	if(sortingStrategyEnabled) {
		updateModifiedCorners(edgeIt,edgeItA);
	}
}



void Tri::updateModifiedCorners(const EdgeIterator twoIncident, const EdgeIterator nextThree) {
	assert(sortingStrategyEnabled);
	/* update twoIncident [0] and [1] */
	/* update nextThree [0] and [1] and next [0] */
	for(ul vertex : {vA(twoIncident), vB(twoIncident), vA(nextThree), vB(nextThree), vA(data->nextEdge(nextThree))}) {

		if(!data->isReflexVertex(vertex)) {continue;}

		Triangle tri = findTriangleWithCorner(vertex);
		auto edgeIt  = data->findEdgeBefore(vertex);
		auto a 		 = *edgeIt;
		auto b 		 = *(data->nextEdge(edgeIt));
		ul triIdx 	 = ULMAX;
		std::list<ul> outsideTrisToRepair;

		bool isValidVertex = getBestTriAroundVertex(vertex,tri,a,b, outsideTrisToRepair,triIdx);

		if(isValidVertex && triIdx != ULMAX) {
			tri = getTriangle(triIdx);
			Exact triArea = getArea(tri);
			Exact outSideArea = CGAL::area(data->v(a[0]),data->v(b[1]),data->v(b[0]));

			/*************************************************************/
			/* with '-sort' we first sort int a priority queue 			 */
			/* we do the actual flipping in an additional run 			 */
			/*************************************************************/
			if(config->verbose) {
				LOG(INFO) << "addeing vertex " << (*edgeIt)[1] << " to flipqueue!";
			}
			Exact areaMod = (triArea > outSideArea) ? triArea-outSideArea : outSideArea-triArea;
			flipQueue.push( FlipElement(vertex,areaMod) );
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
	for(ul i=0; i < (ul)tOUT.numberoftriangles; ++i) {
		auto tri = getTriangle(i);
		if(isTriOnBoundaryABAndReflexVertexC(tri)) {
			trisOnReflexVertex.push_back(i);
		}
	}
}

Exact Tri::getArea(const Triangle& tri) const {
	return CGAL::area(getPoint(tri.a),getPoint(tri.b),getPoint(tri.c));
}

void Tri::repairTriangulationOn(std::list<ul> tris, const ul vertex) {
	assert(tris.size() > 1);

	while(tris.size() > 1) {
		auto t1 = tris.begin();
		auto t2 = t1; ++t2;
		sl checkCnt = tris.size();

		while(t2 != tris.end()) {
			if(config->verbose) {
				LOG(INFO) << "t1  " << getTriangle(*t1);
				LOG(INFO) << "t2  " << getTriangle(*t2);
			}

			auto tri1 = getTriangle(*t1);
			auto tri2 = getTriangle(*t2);

			/* check if the two triangles form a convex quadrilateral */
			if(hasCorner(tri1,vertex) && hasCorner(tri2,vertex) && isConvexQuad(getTriangle(*t1),getTriangle(*t2))) {
				/* if so we flip them */
				flipPair( *t1, *t2 );

				tri1 = getTriangle(*t1);
				tri2 = getTriangle(*t2);

				/* find the triangle that is still connected to 'vertex'
				 * and erase the other from the list 					*/
				if(!hasCorner(tri1,vertex)) {
					tris.erase(t1);
					t1 = t2;
					++t2;
				} else {
					tris.erase(t2);
					t2 = t1; ++t2;
				}
			} else {
				t1 = t2;
				++t2;
			}
		}
		assert(checkCnt != (sl)tris.size());
	}
}

void Tri::reflexSensitiveFlipping(Triangle tri, const ul vertex) {
	auto triCP = getNeighborWithoutVertex(tri,vertex);
	if(triCP.isValid()) {
		auto ie = getCommonPair(tri, triCP);
		/* check to avoid flipping input edges */
		if(!data->hasEdge(ie[0],ie[1]) ) {
			if(isConvexQuad(tri,triCP)) {
				flipPair(tri,triCP);
			}
		}
	}
}

Triangle Tri::getNeighborWithoutVertex(const Triangle t, const ul vertex) {
	for(auto i : {t.nAB, t.nBC, t.nCA} ) {
		if(i != NIL) {
			auto tri = getTriangle(i);
			if(!hasCorner(tri,vertex)) {
				return tri;
			}
		}
	}
	return Triangle();
}

void Tri::flipPair(Triangle ta, Triangle tb) {
	auto cp = getCommonPair(ta,tb);
	ul taM = getMissingCorner(ta,cp[0],cp[1]);
	ul tbM = getMissingCorner(tb,cp[0],cp[1]);

	Triangle taNew(ta.id, taM,cp[0],tbM, NIL,NIL,NIL); //ta.diagonalNeighbor(cp[0]), tb.diagonalNeighbor(cp[0]) ,tb.id );
	Triangle tbNew(tb.id, tbM,cp[1],taM, NIL,NIL,NIL); //tb.diagonalNeighbor(cp[1]), ta.diagonalNeighbor(cp[1]), ta.id );

	taNew.setNeighbors(tb.diagonalNeighbor(cp[1]),tb.id,ta.diagonalNeighbor(cp[1]));
	tbNew.setNeighbors(ta.diagonalNeighbor(cp[0]),ta.id,tb.diagonalNeighbor(cp[0]));

	if(config->verbose && (ta.id == 65 || ta.id==62)) {
		LOG(INFO) << "Old ta: " << ta;
		LOG(INFO) << "Old tb: " << tb;
		LOG(INFO) << "New ta: " << taNew;
		LOG(INFO) << "New tb: " << tbNew;
	}

	writeBack(taNew);
	writeBack(tbNew);

	/* repair back links of neighbours */
	std::vector<Triangle> nUpdate;
	for(auto nid : { tb.diagonalNeighbor(cp[1]), ta.diagonalNeighbor(cp[1]) } ) {
		if(nid != NIL) {
			auto tN = getTriangle(nid);
			tN.updateNeibhorFromTo(tb.id,ta.id);
			nUpdate.push_back(tN);
		}
	}
	for(auto nid : { ta.diagonalNeighbor(cp[0]), tb.diagonalNeighbor(cp[0]) } ) {
		if(nid != NIL) {
			auto tN = getTriangle(nid);
			tN.updateNeibhorFromTo(ta.id,tb.id);
			nUpdate.push_back(tN);
		}
	}
	for(auto t : nUpdate) {
		writeBack(t);
	}
}

bool Tri::isConvexQuad(const Triangle& ta, const Triangle& tb) const {
	auto cp = getCommonPair(ta,tb);
	ul taM = getMissingCorner(ta,cp[0],cp[1]);
	ul tbM = getMissingCorner(tb,cp[0],cp[1]);
	return CGAL::left_turn(p(taM),p(cp[0]),p(tbM)) && CGAL::left_turn(p(tbM),p(cp[1]),p(taM));
}

void Tri::writeBack(const Triangle& tri) {
	tOUT.trianglelist[tri.id*3    ] = tri.a;
	tOUT.trianglelist[tri.id*3 + 1] = tri.b;
	tOUT.trianglelist[tri.id*3 + 2] = tri.c;
	tOUT.neighborlist[tri.id*3    ] = tri.diagonalNeighbor(tri.a);
	tOUT.neighborlist[tri.id*3 + 1] = tri.diagonalNeighbor(tri.b);
	tOUT.neighborlist[tri.id*3 + 2] = tri.diagonalNeighbor(tri.c);
}

/* returns the common two corners of ta, tb in the CCW order of 'ta' */
std::array<ul,2> Tri::getCommonPair(const Triangle& ta, const Triangle& tb) const {
	std::vector<ul> vect;
	if(hasCorner(ta,tb.c)) {vect.push_back(tb.c);}
	if(hasCorner(ta,tb.b)) {vect.push_back(tb.b);}
	if(hasCorner(ta,tb.a)) {vect.push_back(tb.a);}
	assert(vect.size() == 2);

	Line l(p(vect[0]),p(vect[1]));
	if(!l.has_on_positive_side( p( getMissingCorner(ta,vect[0],vect[1])) ) ) {
		std::swap(vect[0],vect[1]);
	}

	return {{ vect[0],vect[1] }};
}

/* CCW !! rotation */
Triangle Tri::getNextCCWTriangleAroundVertex(const Triangle& tri, ul vertex) const {
	if(vertex == tri.a && tri.nCA != NIL) {
		return getTriangle(tri.nCA);
	} else if(vertex == tri.b && tri.nAB != NIL) {
		return getTriangle(tri.nAB);
	} else if(vertex == tri.c && tri.nBC != NIL) {
		return getTriangle(tri.nBC);
	}
	if(config->verbose) {
		LOG(INFO) << "CCW: getNextCCWTriangleAroundVertex failed for vertex " << vertex << " on tri: " << tri;
		if(tri.nBC != NIL)  {LOG(INFO) << "aN: " << getTriangle(tri.nBC);}
		if(tri.nCA != NIL)  {LOG(INFO) << "bN: "  << getTriangle(tri.nCA);}
		if(tri.nAB != NIL)  {LOG(INFO) << "cN: "  << getTriangle(tri.nAB);}
	}
	assert(false);
	return Triangle();
}

/* CW !! rotation */
Triangle Tri::getNextCWTriangleAroundVertex(const Triangle& tri, ul vertex) const {
	if(vertex == tri.a && tri.nAB != NIL) {
		return getTriangle(tri.nAB);
	} else if(vertex == tri.b && tri.nBC != NIL) {
		return getTriangle(tri.nBC);
	} else if(vertex == tri.c && tri.nCA != NIL) {
		return getTriangle(tri.nCA);
	}
	if(config->verbose) {
		LOG(INFO) << "CW: getNextCWTriangleAroundVertex failed for vertex " << vertex << " on tri: " << tri;
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
	for (ul i = 0; i < (ul)tOUT.numberoftriangles; ++i) {
		Triangle t = getTriangle(i);
		std::cout << t << " ";
	}
	std::cout << std::endl;
}


void Tri::printEdges() const {
	std::cout << "edgelist idx: ";
	for (ul i = 0; i < (ul)tOUT.numberofedges*2; ++i) {
		std::cout << tOUT.edgelist[i] << ", ";
	}
	std::cout << std::endl;

	std::cout << std::endl;
	for (ul i = 0; i < (ul)tOUT.numberofedges; ++i) {
		Edge e = getEdge(i);
		std::cout << e << " / ";
	}
	std::cout << std::endl;
}


