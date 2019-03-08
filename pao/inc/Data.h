/*
 * monos is written in C++.  It computes the weighted straight skeleton
 * of a monotone polygon in asymptotic n log n time and linear space.
 * Copyright (C) 2018 - Günther Eder - geder@cs.sbg.ac.at
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

#include <memory>
#include <assert.h>
#include <ctime>
#include <fstream>

#include <iterator>
#include <algorithm>

#include <sys/stat.h>

#include <vector>
#include <list>
#include <set>
#include <string>

#include "Definitions.h"
#include "cgTypes.h"
#include "Config.h"

class Data {

public:
	Data(bool gui = false):gui(gui)  {}
	~Data() {}

	void initialize(const Config& config);

	const InputPoints&  getVertices() const { return inputVertices; }
	const Polygon&      getPolygon()  const { return polygon; }

	const IndexEdge e(const ul& idx) const { assert(idx < polygon.size() ); return polygon[idx]; }
	const Point& v(const ul& idx) const { assert(idx < inputVertices.size()); return inputVertices[idx]; }
	Edge getEdge(const ul& idx) const { return Edge( eA(idx), eB(idx) ); }
	Edge getEdge(const EdgeIterator& it) const { return Edge( eA((*it)[0]), eB((*it)[1]) ); }

	const Point& eA(const ul& edgeIdx) const {return v(e(edgeIdx)[0]);}
	const Point& eB(const ul& edgeIdx) const {return v(e(edgeIdx)[1]);}

	EdgeIterator findEdgeBefore(const ul& vertexIndex);
	EdgeIterator nextEdge(const EdgeIterator& it) { return (it+1 != polygon.end()) ? it+1 : polygon.begin(); }
	EdgeIterator prevEdge(const EdgeIterator& it) { return (it != polygon.begin()) ? it-1 : polygon.end()-1; }
	EdgeIterator findEdgeIterator(const IndexEdge& ie);

	bool isReflexVertex(const ul& idx) const {return IVreflex[idx];}

	void removePolygonCorner(EdgeIterator afterIt);
	void addPolygonCorner(EdgeIterator betweenIt, const ul& vertexIdx);
	void invertEdge(EdgeIterator edgeIt);

	bool isNextVertexReflex(EdgeIterator& it);

//	void writePolyToOptPoly();

	void addPolyToOBJ(const Config& cfg) const;
	void printInput() const;

	void printPermutation() const;

	const BasicInput& getBasicInput() const {return basicInput;}
	const bool gui;

	bool hasEdge(const ul idxA, const ul idxB) const {
		return basicInput.has_edge(idxA,idxB);
	}

	bool isEqual(const IndexEdge& a, const IndexEdge& b) const {
		return     ( (a)[0] == (b)[0] && (a)[1] == (b)[1] )
				|| ( (a)[0] == (b)[1] && (a)[1] == (b)[0] );

	}
	bool isEqual(const EdgeIterator& a, const EdgeIterator& b) const {
		return isEqual(*b,*b);

	}

	Exact getPolygonArea() const;

	void identifyConvexReflexInputVertices();

	void setVertex(ul idx, bool reflex) {IVreflex[idx] = reflex;}

private:
	bool loadFile(const std::string& fileName);

	bool parseOBJ(const std::vector<std::string>& lines);
	bool parseGML(std::istream &istream);
	bool parsePOLY(const std::vector<std::string>& lines);
	/** Input: vertices as 2D coordinates, polygon as edge "list"
	 *  with index to the inputVertices
	 **/
	InputPoints			inputVertices;
	std::vector<bool> 	IVreflex;
	std::vector<bool> 	IVmodified;
	Polygon 			polygon;
//	ListPolygon			optPoly;

	BasicInput			basicInput;
	GMLGraph			gml;
};
