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
		tri.setRandomSelection(config.chooseByRandom);

//		data->writePolyToOptPoly();

		if(config.verbose) {LOG(INFO) << "Flipping...";}
		tri.identifyTrisOnReflexInputVertices();

		if(!config.gui) {
			sl retryCnt = (config.enableSortingStrategy) ? 10 : 0;
			do {
				while(!tri.isFlippingDone()) {
					tri.aSingleFlip();
				}
				if(config.enableSortingStrategy) {
					tri.resetForSortedFlipping();
				}
			} while(--retryCnt > 0);

			if(!config.silent) {std::cout << std::endl;}

			data->printPermutation();
		}
	} else {
		if(config.verbose) {LOG(ERROR) << "Pao::run no valid config!";}
	}
}

