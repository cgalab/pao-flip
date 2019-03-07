#include "Config.h"

bool Config::evaluateArguments(std::list<std::string> args) {
	std::string argument;
	bool ret = false;

	if(args.empty()) {
		printHelp();
		validConfig = false;
		return false;
	} else {
		while(!args.empty()) {
			argument = args.front();
			args.pop_front();
		    if (argument == "-help" || argument == "-h") {
				printHelp();
				printHelpLong();
				validConfig = false;
				return false;
			} else if (argument == "-v") {
				if(silent) {
					std::cout << "Use either verbose or silent, -v or -s, not both!"<< std::endl;
				}
				verbose 		= true;
			} else if (argument == "-max") {
				maximize 		= true;
			} else if (argument == "-min") {
				maximize 		= false;
			} else if (argument == "-r") {
				reflexSensitiveFlipping = true;
			} else if (argument == "-sort") {
				if(chooseByRandom) {
					std::cout << "choose -rand or -sort, now -rand is enabled" << std::endl;
				} else {
					enableSortingStrategy = true;
				}
			} else if (argument == "-rand") {
				if(enableSortingStrategy) {
					std::cout << "choose -rand or -sort, now -sort is enabled" << std::endl;
				} else {
					chooseByRandom = true;
				}
			} else if (argument == "-s") {
				if(verbose) {
					std::cout << "Use either verbose or silent, -v or -s, not both!"<< std::endl;
				}
				silent 		= true;
			} else if (argument == "-obj") {
				if(args.empty()) {return false;}
				argument = args.front();
				args.pop_front();
				outputFileName = argument;
			} else if (argument == "-poly") {
				std::cout << ".poly is not supported yet!"<< std::endl;
				validConfig = false;
				return false;
			} else if(args.empty()) {
				if(!fileExists(argument)) {
					std::cout << argument << " is no valid option or filename!"<< std::endl;
					validConfig = false;
					return false;
				} else {
					fileName = argument;
					ret = true;
				}
			}
		}
	}

	return ret;
}

