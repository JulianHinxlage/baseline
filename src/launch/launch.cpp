//
// Copyright (c) 2023 Julian Hinxlage. All rights reserved.
//

#include "core/ModuleManager.h"
#include "core/FileWatcher.h"
#include "core/Config.h"
#include "common/Log.h"
#include <filesystem>
#include <thread>

using namespace baseline;

class Launcher {
public:
	void init(int argc, char* argv[]) {
		auto* moduleManager = Singleton::get<ModuleManager>();
		moduleManager->enableHotReloading = true;
		moduleManager->addModuleDirectory(".");
		moduleManager->addModuleDirectory(std::filesystem::path(argv[0]).parent_path().string());

		auto* config = Singleton::get<Config>();
		config->loadFirstFileFound({ "launch.cfg", "../launch.cfg" , "../../launch.cfg" });
	}

	void load(const std::string& name) {
		auto* moduleManager = Singleton::get<ModuleManager>();
		moduleManager->loadModule(name);
	}

	void run(const std::string& name, const std::string& function, bool onOwnThread = false) {
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

	void joinAll() {
		for (auto& thread : threads) {
			if (thread) {
				thread->join();
			}
			delete thread;
		}
		threads.clear();
	}

private:
	std::vector<std::thread*> threads;
};

int main(int argc, char* argv[]) {
	auto* launcher = Singleton::get<Launcher>();

	auto* config = Singleton::get<Config>();
	config->addCommand("runModule", [&](const std::vector<std::string>& args) {
		if (args.size() > 0) {
			std::string function = "main";
			if (args.size() > 1) {
				function = args[1];
			}
			launcher->run(args[0], function, true);
		}
	});

	Log::info("directory:  %s", std::filesystem::current_path().string().c_str());
	Log::info("executable: %s", argv[0]);

	launcher->init(argc, argv);
	launcher->joinAll();

	auto* moduleManager = Singleton::get<ModuleManager>();
	for (auto& m : moduleManager->getLoadedModules()) {
		moduleManager->unloadModule(m);
	}
	return 0;
}
