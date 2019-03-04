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

#include <iterator>
#include <array>
#include <list>
#include <algorithm>
#include <cmath>
#include <iostream>

#include "Definitions.h"

//#include <CGAL/Exact_predicates_exact_constructions_kernel_with_sqrt.h>
#include <CGAL/Simple_cartesian.h>

#include <CGAL/intersections.h>
#include <CGAL/squared_distance_2.h>
#include <CGAL/Polygon_2_algorithms.h>

//using K 			 = CGAL::Exact_predicates_exact_constructions_kernel_with_sqrt;
using K 			 = CGAL::Simple_cartesian<double>;

using Vector         = K::Vector_2;
using Point          = K::Point_2;
using Line           = K::Line_2;
using Ray            = K::Ray_2;
using Circle         = K::Circle_2;
using Direction      = K::Direction_2;
using Edge       	 = K::Segment_2;
using Intersect		 = K::Intersect_2;
using Exact          = K::FT;

using InputPoints   	= std::vector<Point>;
using IndexEdge 		= std::array<ul,2>;
using Polygon   		= std::vector<IndexEdge>;
using ListPolygon  		= std::list<IndexEdge>;
using EdgeList  		= std::vector<IndexEdge>;
using PointIterator 	= std::vector<Point,std::allocator<Point>>::const_iterator;

using EdgeIterator = std::vector<IndexEdge,std::allocator<IndexEdge>>::iterator;

#include "gml/BasicInput.h"

static Point ORIGIN = Point(0,0);
