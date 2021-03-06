/*
 * Copyright (C) 2019 - Günther Eder - geder@cs.sbg.ac.at
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

#include <iostream>
#include <vector>
#include <assert.h>

#include "../easyloggingpp/src/easylogging++.h"

#define NIL    -1
#define NOLIST -2
#define ZSCALE 0.5
#define OBJSCALE 5
#define OBJOFFSET -1


/*
 * OBJ files index starting at one, we read the input
 * and store the vertices starting at 0, thus we change
 * the reference from faces ot vertex by this offset
 **/

#define BOOST_VARIANT_USE_RELAXED_GET_BY_DEFAULT

#define INFPOINT Point(std::numeric_limits<double>::max(),std::numeric_limits<double>::max())

//using ul = unsigned long;
//using sl = signed long;
using ul = unsigned int;
using sl = signed int;
#define ULMAX std::numeric_limits<ul>::max()
#define SLMAX std::numeric_limits<sl>::max()
