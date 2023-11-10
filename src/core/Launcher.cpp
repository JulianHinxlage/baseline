//
// Copyright (c) 2023 Julian Hinxlage. All rights reserved.
//

#include "Launcher.h"
#include "Config.h"

namespace baseline {

	Launcher::Launcher() {
		auto* config = Singleton::get<Config>();
		config->addCommand("runModule", [&](const std::vector<std::string>& args) {
			if (args.size() > 0) {
				std::string function = "main";
				if (args.size() > 1) {
					function = args[1];
				}
				run(args[0], function, true);
			}
		});
	}

	void Launcher::init(int argc, char* argv[], const std::string& configFile) {
		auto* moduleManager = Singleton::get<ModuleManager>();
		moduleManager->enableHotReloading = true;
		moduleManager->addModuleDirectory(".");
		moduleManager->addModuleDirectory(std::filesystem::path(argv[0]).parent_path().string());

		auto* config = Singleton::get<Config>();
		config->loadFirstFileFound({ configFile, std::string("../") + configFile , std::string("../../") + configFile, std::string("../../../") + configFile });
	}

	void Launcher::load(const std::string& name) {
		auto* moduleManager = Singleton::get<ModuleManager>();
		moduleManager->loadModule(name);
	}

	void Launcher::run(const std::string& name, const std::string& function, bool onOwnThread) {
		auto* moduleManager = Singleton::get<ModuleManager>();
		Module* module = moduleManager->loadModule(name);
		if (module) {
			if (function != "") {
				if (onOwnThread) {
					threads.push_back(new std::thread([module, function]() {
						if (module) {
							module->invoke(function);
						}
						}));
				}
				else {
					module->invoke(function);
				}
			}
		}
	}

	void Launcher::joinAll() {
		for (auto& thread : threads) {
			if (thread) {
				thread->join();
			}
			delete thread;
		}
		threads.clear();
	}

	void Launcher::shutdown() {
		auto* moduleManager = Singleton::get<ModuleManager>();
		auto modules = moduleManager->getLoadedModules();
		for (int i = modules.size() - 1; i >= 0; i--) {
			moduleManager->unloadModule(modules[i]);
		}
	}

}
