#pragma once

#include <string>
#include <list>

#include "Definitions.h"
#include "tools.h"

class Config {
public:
	Config(bool _gui = false)
	: fileName("")
	, verbose(false)
	, silent(false),gui(_gui)
	, maximize(true)
 	, outputFileName("")
	, validConfig(false) {
		printOptions =  "[-h] [-max|-min] [-v|-s] ";
		printOptions += "[-r|-rr] [-sort|-rand] [-invert] [-area <AREA>] ";
		printOptions += "[-poly|-obj <filename>] <filename>";
	}

	Config(std::list<std::string> args, bool gui = false):Config(gui) {
		validConfig = evaluateArguments(args);
	}

	void printHelp() const {
		std::cout << "usage: pao " << printOptions << std::endl;
	}

	void printHelpLong() const {
		std::cout << "  -h \t\t\tprint this help" << std::endl
			 << "  -max/-min \t\tmaxmize or minimize polygon (default: max)" << std::endl
			 << "  -sort \t\tenable sorting strategy (works with both min and max)" << std::endl
			 << "  -rand \t\trandomly access the vector of reflex vertices (not with -sort)" << std::endl
			 << "  -invert \t\talso enable inverter (weasel's strategy)" << std::endl
			 << "  -area <AREA> \t\tstate an area bound that has to be beaten" << std::endl
			 << "  -r \t\t\tenable reflex sensitive flipping in the polygon" << std::endl
			 << "  -rr \t\t\t-r plus choose a random triangle among the triangles on a vertex" << std::endl
			 << "  -v \t\t\tverbose mode, shows more information about the computation" << std::endl
			 << "  -s \t\t\tsilent mode, shows no information" << std::endl
			 << "  -obj <file> \t\twrite output in wavefront obj format (3D coordinates)" << std::endl
			 << "  <filename> \t\tinput type can be .gml (GraphML) or .obj (Wavefront Object)" << std::endl;
	}

	bool isValid() const { return validConfig; }

	void setNewInputfile(const std::string& _fileName) {
		if(fileExists(_fileName)) {
			fileName = _fileName;
			validConfig = true;
		}
	}

	std::string   	fileName;

	bool 			verbose;
	bool			silent;
	bool 			gui;

	bool 			maximize;
	bool 			enableSortingStrategy 	= false;
	bool 			reflexSensitiveFlipping = false;
	bool 			randomlyChooseTriangleAroundVertex = false;
	bool			chooseByRandom 			= false;

	bool			inverterEnabled 		= false;

	bool			isAreaBoundSet			= false;
	long long		areaBound				= 0.0;

	std::string		outputFileName;

private:
	bool evaluateArguments(std::list<std::string> args);

	std::string 	printOptions;
	bool	 		validConfig;
};
