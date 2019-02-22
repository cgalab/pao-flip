#include "tools.h"

#include <fstream>
#include <iostream>

#include "Config.h"
#include "Pao.h"

int main(int argc, char *argv[]) {
	setupEasylogging(argc, argv);

	static std::list<std::string> args;
	for(auto i = 1; i < argc; ++i) { args.push_back(std::string(argv[i])); }

	/* start */
	Pao pao(args);

	pao.run();

	return 0;
}
