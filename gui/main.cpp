#include "mainwindow.h"

#include "tools.h"

#include <QApplication>
#include <fstream>
#include <iostream>

#include "Config.h"
#include "Pao.h"

int main(int argc, char *argv[]) {

	setupEasylogging(argc, argv);
	QApplication a(argc, argv);

	static std::list<std::string> args;
	for(auto i = 1; i < argc; ++i) { args.push_back(std::string(argv[i])); }

	Pao pao(args,true);

	if(pao.isRunnable()) {
		pao.run();

		std::string title =
#ifdef CMAKE_BUILD_TYPE
				"PAO GUI (" CMAKE_BUILD_TYPE ")"
#else
				"PAO GUI"
#endif
		;

		MainWindow w(title, pao);
		w.show();

		return a.exec();
	}
	return 0;
}
