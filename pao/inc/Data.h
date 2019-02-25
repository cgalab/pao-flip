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


#ifndef DATA_H_
#define DATA_H_

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

#include "cgTypes.h"
#include "Config.h"
#include "Definitions.h"

class Data {
	using EdgeIterator = std::vector<IndexEdge,CORE::allocator<IndexEdge>>::iterator;

public:
	Data(bool gui = false):gui(gui)  {}
	~Data() {}

	void initialize(const Config& cfg);

	const InputPoints&  getVertices() const { return inputVertices; }
	const Polygon&      getPolygon()  const { return polygon; }

	const IndexEdge e(const ul& idx) const { assert(idx < polygon.size() ); return polygon[idx]; }
	const Point& v(const ul& idx) const { assert(idx < inputVertices.size()); return inputVertices[idx]; }
	Edge getEdge(const ul& idx) const;
	Edge getEdge(const EdgeIterator& it) const;

	const Point& eA(const ul& edgeIdx) const {return v(e(edgeIdx)[0]);}
	const Point& eB(const ul& edgeIdx) const {return v(e(edgeIdx)[1]);}

	void addPolyToOBJ(const Config& cfg) const;
	void printInput() const;

	const BasicInput& getBasicInput() const {return basicInput;}

	void setGui(const bool _gui) { gui = _gui; }

	bool 			gui;

	// gui debug
	std::vector<Edge> lines;

private:
	bool loadFile(const std::string& fileName);

	bool parseOBJ(const std::vector<std::string>& lines);
	bool parseGML(std::istream &istream);
	bool parsePOLY(const std::vector<std::string>& lines);
	/** Input: vertices as 2D coordinates, polygon as edge "list"
	 *  with index to the inputVertices
	 **/
	InputPoints		inputVertices;
	Polygon 		polygon;
	GMLGraph		gml;
	BasicInput		basicInput;
};

#endif /* DATA_H_ */
