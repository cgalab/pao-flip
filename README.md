# PAO-FLIP

PAO-FLIP (polygon area optimization using fliping) 

# Algorithm

Given a simple polygon P, pao-flip computes the triangulation of P and CH(P). It tries to increase/decrease the bounded area of P by "flipping" reflex vertices of P and boundary edges using the triangles provided by Triangle. 

# Input/Output

Reads GraphML .gml or  wavefront .obj format that describes a simple polygon. Writes the permutation of the polygon (or
a wavefront .obj file).

# Requirements 
- C++17 enabled complier (gcc,clang)
- CGAL 
- Linux or Mac OS


# Usage

<code>pao [-h] [-v|-s] [-r] [-min|-max] [-obj &lt;filename&gt;] &lt;filename&gt;</code>

| options        | description           |
| -------------:|:------------- |
|  -h           |         print this help |
|  -v           |         verbose mode, shows more information about the computation |
|  -s           |         silent mode, keep the output quiet except for the final permutation |
|  -r           |         switch reflex sensitive flipping ON |
|  -min,-max    |         computes a smaller/larger poylgon (default max) |
|  -obj      |            write output in wavefront obj format (3D coordinates) |
|  &lt;filename&gt; |           input type is either wavefront obj or GML format |

# Submodules

- easyloggingpp
- triangle 

# License
pao-flip is written in C++ and uses CGAL.  
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
