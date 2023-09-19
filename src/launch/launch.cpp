//
// Copyright (c) 2023 Julian Hinxlage. All rights reserved.
//

#include "core/ModuleManager.h"
#include <filesystem>

using namespace baseline;

int main(int argc, char* argv[]) {
	moduleManager->addModuleDirectory(".");
	moduleManager->addModuleDirectory(std::filesystem::path(argv[0]).parent_path().string());
	Module* module = nullptr;

	module = moduleManager->loadModule("moduleDebugger");
	if (module) {
		module->invoke("main");
		moduleManager->unloadModule(module);
	}

	return 0;
}
