//
// Copyright (c) 2023 Julian Hinxlage. All rights reserved.
//

#pragma once

#include "Module.h"
#include <vector>
#include <memory>

namespace baseline {

	class ModuleManager {
	public:
		ModuleManager();

		void addModuleDirectory(const std::string &directory);
		Module *loadModule(const std::string &name);
		Module* getModule(const std::string& name);
		void unloadModule(Module* module);
		void unloadModule(const std::string& name);
		std::vector<Module*> getLoadedModules();
		std::vector<std::string> getInstalledModules();


	private:
		std::vector<std::string> moduleDirectories;
		std::vector<std::shared_ptr<Module>> modules;
	};

	ModuleManager* getModuleManager();
}

static baseline::ModuleManager *moduleManager = baseline::getModuleManager();
