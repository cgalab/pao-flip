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
: config(args) {
	data = new Data(gui);
	if(config.isValid()) {
		data->initialize(config);
	}
}

Pao::~Pao() {
	if(data != nullptr) { delete data; }
}


void Pao::run() {
	if(!config.isValid()) {
		LOG(ERROR) << "Pao::run no valid config!";
	} else {
		tri.runTriangle(*data);
	}

}

