//
// Copyright (c) 2023 Julian Hinxlage. All rights reserved.
//

#include "core/ModuleManager.h"
#include <filesystem>

using namespace baseline;

int main(int argc, char* argv[]) {
	ModuleManager moduleManger;
	moduleManger.addModuleDirectory(".");
	moduleManger.addModuleDirectory(std::filesystem::path(argv[0]).parent_path().string());
	Module *module = moduleManger.loadModule("test");
	if (module) {
		module->invoke("main");
		moduleManger.unloadModule(module);
	}
	return 0;
}
