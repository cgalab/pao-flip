# PAO-FLIP

PAO-FLIP (polygon area optimization using fliping) 

# Algorithm

Given a simple polygon P, pao-flip computes the triangulation of P and CH(P). It tries to increase/decrease the bounded area of P by "flipping" triangles. That is, a reflex vertex and an edge that lie on the boundary of a common triangle are identified. Then, the boundary of P is modified as discussed in the meeting.

# Input/Output

Reads GraphML .gml or  wavefront .obj format that describes a polygon. Writes the
same format; .obj gets 3D coordinates which can be imported to programs like
Blender.

# Requirements 
- C++17 enabled complier (gcc,clang)
- CGAL 
- Linux or Mac OS


# Usage

<code>pao [-h] [-v|-s] [-obj &lt;filename&gt;] &lt;filename&gt;</code>

| options        | description           |
| -------------:|:------------- |
|  -h           |         print this help |
|  -v           |         verbose mode, shows more information about the computation |
|  -s           |         silent mode, shows no information |
|  -obj      |            write output in wavefront obj format (3D coordinates) |
|  &lt;filename&gt; |           input type is either wavefront obj or GML format |

# Submodules

- easyloggingpp
- triangle 


# License
pao-f is written in C++ and uses CGAL.  
Copyright (C) 2019 - Günther Eder - geder@cs.sbg.ac.at

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
