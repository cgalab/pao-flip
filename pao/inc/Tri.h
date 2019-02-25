#pragma once

/* interact with triangle */

#include <string>
#include <assert.h>

#include "Definitions.h"
#include "Data.h"

#ifdef __cplusplus
extern "C" {
	#include "../../triangle/triangle.h"
}
#endif

class Tri {
public:
	Tri() {
		/* z..index vertices from zero
		 * p..Triangulate (PSLG)
		 * n..compute triangle neighbors
		 * V..Verbose
		 * Q..quiet
		 * */
		std::string mysw = "pznV";
		triswitches = new char[mysw.length()+1];
		strcpy(triswitches,mysw.c_str());
	}

	~Tri() {delete triswitches;}

	void runTriangle(const Data& data);

	triangulateio triangleIN, triangleOUT, vorout;

private:
	void filltriangulateioIn(const Data& data, triangulateio& tri);
	void inittriangulateioOut(const Data& data, triangulateio& tri);

	char *triswitches;
};
