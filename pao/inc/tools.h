#ifndef TOOLS_H_
#define TOOLS_H_

#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <memory>
#include <assert.h>
#include <ctime>
#include <fstream>

#include "cgTypes.h"

bool fileExists(const std::string& fileName);

std::string currentTimeStamp();

void setupEasylogging(int argc, char** argv);

#endif
