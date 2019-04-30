/*
 * Pao is written in C++.  It computes the weighted straight skeleton
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


#include <iostream>
#include <exception>

#include "Pao.h"
#include "Data.h"


Pao::Pao(std::list<std::string>& args, bool gui)
: config(args,gui) {
	data = new Data(gui);
	if(config.isValid()) {
		/* initialize and load file into 'Data' */
		data->initialize(config);
	}
}

Pao::~Pao() {
	if(data != nullptr) { delete data; }
}


void Pao::run() {
	if(config.isValid()) {

		//if(config.verbose) {data->printInput();}

		if(config.verbose) {LOG(INFO) << "Running triangle...";}
		/* create triangulation of input */
		tri.runTriangle(*data);
		tri.setConfig(&config);

//		tri.printTriangles();
//		tri.printEdges();

		if(config.maximize) {
			if(config.verbose) {LOG(INFO) << "Set to maximizing...";}
			tri.setMaximizingStrategy();
		} else {
			if(config.verbose) {LOG(INFO) << "Set to minimizing...";}
			tri.setMinimizingStrategy();
		}

		tri.setReflexSensitive(config.reflexSensitiveFlipping);
		if(config.verbose && config.reflexSensitiveFlipping)  {
			LOG(INFO) << "reflex sensitive flipping ON...";
		}

		tri.setSortingStratey(config.enableSortingStrategy);
		if(config.verbose && config.enableSortingStrategy)  {
			LOG(INFO) << "sorting strategy ON...";
		}

		tri.setRandomSelection(config.chooseByRandom);
		if(config.verbose && config.chooseByRandom)  {
			LOG(INFO) << "choose randomly ON...";
		}

		tri.setInverterSelection(config.inverterEnabled);
		if(config.verbose && config.inverterEnabled)  {
			LOG(INFO) << "inversions (also) ON...";
		}

//		data->writePolyToOptPoly();

		if(config.verbose) {LOG(INFO) << "Flipping...";}
		tri.setReflexVertices();

		if(!config.gui) {

				auto cnt = tri.getFlipCnt();

				while(!tri.isFlippingDone()) {

					cnt = tri.getFlipCnt();

					while(tri.hasReflexVertices() || (config.enableSortingStrategy && !tri.isFlipQueueEmpty())) {
						tri.aSingleFlip();
					}

					if(!config.enableSortingStrategy && cnt < tri.getFlipCnt()) {
						tri.setReflexVertices();
						if(config.verbose) {
							LOG(INFO) << "refill reflex vertex list";
						}
					} else {
						tri.setFlippingDone();
					}

					if(config.enableSortingStrategy && cnt < tri.getFlipCnt()) {
						tri.resetForSortedFlipping();
					}
				}


			if(!config.silent) {std::cout << std::endl;}

			bool printPolygon = true;

			Exact area = 0.0;

			if(config.isAreaBoundSet) {
				printPolygon = false;

				area = data->getPolygonArea();

				if(config.areaBound == 0) {
					printPolygon = true;
				} else {
					if(config.maximize && area > config.areaBound) {
						printPolygon = true;
					}
					if(!config.maximize && area < config.areaBound) {
						printPolygon = true;
					}
				}
				if(config.verbose) {
					LOG(INFO) << "area-change: " << std::abs(config.areaBound - area);
				}
			}

			if(printPolygon) {
				data->printPermutation();
				if(config.isAreaBoundSet) {
					std::cout <<  std::setprecision(20) << area << std::endl;
				}
			}
		}
	} else {
		if(config.verbose) {LOG(ERROR) << "Pao::run no valid config!";}
	}
}

