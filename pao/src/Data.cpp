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


#include "Data.h"
#include "tools.h"

void Data::initialize(const Config& config) {
	/* load input vertices/polygon and weights */
	if(config.isValid()) {
		if (!loadFile(config.fileName)) {
			assert(false); return;
		}

		/* identify convex/reflex vertices */
		identifyConvexReflexInputVertices();

	}
}

EdgeIterator Data::findEdgeBefore(const ul& vertexIndex) {
	for(auto it=polygon.begin(); it != polygon.end(); ++it) {
		if((*it)[1] == vertexIndex) {
			return it;
		}
	}
	assert(false);
	return polygon.begin();
}

void Data::removePolygonCorner(EdgeIterator afterIt) {
	auto& eA = *afterIt;
	auto& eB = *(afterIt+1);
	basicInput.remove_edge(eA[0],eA[1]);
	basicInput.remove_edge(eB[0],eB[1]);

	eA[1] = eB[1];
	polygon.erase(afterIt+1);
	basicInput.add_edge(eA[0],eA[1]);
}

void Data::addPolygonCorner(EdgeIterator betweenIt, const ul& vertexIdx) {
	auto& eOld= *betweenIt;
	ul targetIdx = eOld[1];

	/* update basic Input */
	basicInput.remove_edge(eOld[0],eOld[1]);
	basicInput.add_edge(eOld[0],vertexIdx);
	basicInput.add_edge(vertexIdx,eOld[1]);

	/* update polygon */
	eOld[1] = vertexIdx;
	polygon.insert(betweenIt, {{vertexIdx,targetIdx}} );
}


void Data::identifyConvexReflexInputVertices() {
	IVreflex   = std::vector<bool>(polygon.size(),false);
	IVmodified = std::vector<bool>(polygon.size(),false);

	auto it = polygon.begin();
	while(it != polygon.end()) {
		Point A = v( (*it)[0] );
		Point B = v( (*it)[1] );
		Point C;
		std::cout << (*it)[1];
		++it;
		if(it !=  polygon.end()) {
			C = v( (*it)[1] );
		} else {
			C = v( (*polygon.begin())[1] );
		}

		bool isReflex = (CGAL::right_turn(A,B,C)) ? true : false;
		std::cout << "(" <<isReflex << ") ";
		if(it == polygon.end()) {
			IVreflex[ (*polygon.begin())[0] ] = isReflex;
		} else {
			IVreflex[ (*it)[0] ] = isReflex;
		}
	}
}

bool Data::loadFile(const std::string& fileName) {
	if(fileExists(fileName)) {
		std::ifstream in(fileName);

		std::string extension = (fileName.find(".")  != std::string::npos) ? fileName.substr(fileName.find_last_of(".")+1) : "";
		transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

		if( extension == "gml" || extension == "graphml" ) {
			std::cout << "parsing gml" << std::endl;
			if(!parseGML(in)) {return false;}
		} else {
			std::vector<std::string> lines;
			for(std::string line; getline(in, line);) {
				lines.push_back(line);
			}

			if(extension == "obj") {
				/* wavefront obj format - https://en.wikipedia.org/wiki/Wavefront_.obj_file */
				if(!parseOBJ(lines)) {return false;}
			} else if(extension == "poly") {
				/* triangle's poly file format - https://www.cs.cmu.edu/~quake/triangle.poly.html */
				if(!parsePOLY(lines)) {return false;}
			}
		}
		return true;
	}

	return false;
}


bool Data::parseOBJ(const std::vector<std::string>& lines) {
	std::vector<std::vector<ul>> faces;

	InputPoints 	points;
	Polygon			poly;

	for(auto l : lines) {
		std::istringstream buf(l);
		std::istream_iterator<std::string> beg(buf), end;
		std::vector<std::string> tokens(beg, end);

		auto isVertex  = false;
		auto isFacet   = false;

		bool setX=true, setY=true, setZ=true;
		double p_x=-INFINITY, p_y=-INFINITY, p_z=-INFINITY;

		std::vector<ul> facetList;

		for(auto& s : tokens) {
			/* comments, and not supported or needed obj parameters */
			if(s == "#" || s == "o" || s == "vt" || s == "vn" ||
			   s == "g" || s == "usemtl" || s == "s" || s == "off" ||
			   s == "mtllib" || s == "Ka" || s == "Kd" || s == "Ks" ||
			   s == "d" || s == "newmtl" || s == "illum" || *s.begin() == 'm' ||
			   *s.begin() == '-' || *s.begin() == 'b' || s == "l") {
				break;
			}

			if(isVertex) {
				if(setX) {
					p_x = atof(s.c_str());
					setX = false;
				} else if(setY){
					p_y = atof(s.c_str());
					setY = false;
				} else if(setZ) {
					p_z = atof(s.c_str());
					setZ = false;
				}
			} else if(isFacet) {
				facetList.push_back(std::atoi(s.c_str()));
			}

			if(s == "v") {isVertex  = true;}
			if(s == "f") {isFacet   = true;}
		}

		if(!setX && !setY) {
			points.push_back(Point(p_x,p_y));
		}

		if(!facetList.empty()) {
			faces.push_back(facetList);
		}
	}

	if(!faces.empty()) {
		auto face = faces[0];
		for(ul i=0; i < face.size(); ++i) {
			poly.push_back( {{face[i]+OBJOFFSET,face[(i+1)%face.size()]+OBJOFFSET }} );
			/* input weights are realized via GraphML -- .gml files*/
		}
	} else {
		LOG(WARNING) << "More than one face!";
	}

	/* initialize const input variables */
	inputVertices 	= points;
	polygon 		= poly;
	basicInput.add_list(inputVertices,polygon);

	return !inputVertices.empty() && !polygon.empty();
}

bool Data::parsePOLY(const std::vector<std::string>& lines) {
	std::cout << "POLY: no yet supported!" << lines.size() << std::endl;
	return false;
}

//void Data::writePolyToOptPoly() {
//	for(auto e : polygon) {
//		optPoly.push_back(e);
//	}
//}

bool Data::parseGML(std::istream &istream) {
	gml = GMLGraph::create_from_graphml(istream);
	basicInput = BasicInput();
	basicInput.add_graph(gml);

	InputPoints 	points;

	for(auto v : basicInput.vertices()) {
		points.push_back(v.p);
	}

	/* iterate over edges to construct polygon */
	auto edges = basicInput.edges();

	Polygon			poly(edges.size());

	for(ul i=0; i < edges.size(); ++i) {
		auto edge = edges[i];
		auto min = std::min(edge.u,edge.v);
		auto max = std::max(edge.u,edge.v);
		assert(min != max);

		if(min != 0 || (min == 0 && max == 1) ) {
			poly[min] = {{edge.u, edge.v}};
		} else {
			poly[max] = {{edge.v, edge.u}};
		}
	}

	/* initialize const input variables */
	inputVertices 	= points;
	polygon 		= poly;

	return !inputVertices.empty() && !polygon.empty();
}


void Data::printInput() const {
	std::cout << "Input Vertices: " << std::endl;
	for(auto point : inputVertices) {
		std::cout << "(" << point.x() << "," << point.y() << ") ";
	}
	std::cout << std::endl << "Input Polygon: " << std::endl;
	for(auto edge : polygon) {
		std::cout  << "(" << edge[0] << "," << edge[1] << ") ";
	}
	std::cout << std::endl;
}


/*
 * NOTE: this only works if writeOBJ was invoced before, as we only
 * add a face with the indices that require the vertices already in
 * the file
 * */
void Data::addPolyToOBJ(const Config& cfg) const {
//	if(cfg.outputType == OutputType::OBJ) {
		double xt, yt, zt, xm, ym, zm;

		std::ofstream outfile (cfg.outputFileName,std::ofstream::binary | std::ofstream::app);

		outfile << "f";
		for(auto e : polygon) {
			outfile << " " << e[0]+1;
		}

		outfile << std::endl;
		outfile.close();
//	}
}
