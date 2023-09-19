//
// Copyright (c) 2023 Julian Hinxlage. All rights reserved.
//

#include "ModuleManager.h"
#include "common/Log.h"
#include <filesystem>

#if WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#if WIN32
	#define FILE_PATH_SEPERATOR "\\"
	#define MODULE_FILE_EXTENSION ".dll"
#else
	#define FILE_PATH_SEPERATOR "/"
	#define MODULE_FILE_EXTENSION ".so"
#endif

namespace baseline {

	ModuleManager::ModuleManager() {
		
	}

	void ModuleManager::addModuleDirectory(const std::string& directory) {
		moduleDirectories.push_back(std::filesystem::absolute(directory).string());
	}

	Module* ModuleManager::loadModule(const std::string& name) {
		std::string baseFile = name;
		std::string extension = std::filesystem::path(name).extension().string();

		if (extension == "") {
			baseFile += MODULE_FILE_EXTENSION;
		}

		std::string file = "";
		for (auto& directory : moduleDirectories) {
			file = directory + FILE_PATH_SEPERATOR + baseFile;
			if (std::filesystem::exists(file)) {
				break;
			}
			file = "";
		}

		if (file == "") {
			Log::warning("module %s: file not found\n", name.c_str());
			return nullptr;
		}

		auto module = std::make_shared<Module>();
		module->name = name;
		module->file = file;

#if WIN32
		module->handle = (void*)LoadLibrary(file.c_str());
		if (!module->handle) {
			Log::error("module %s could not be loaded\n", name.c_str());
		}
#else
		module->handle = dlopen(runtimePath.c_str(), RTLD_NOW | RTLD_LOCAL);
#endif

		if (module->handle) {
			Log::info("module %s loaded, file: %s\n", name.c_str(), file.c_str());
		}

		modules.push_back(module);
		return module.get();
	}

	Module* ModuleManager::getModule(const std::string& name) {
		for (auto& module : modules) {
			if (module->name == name) {
				return module.get();
			}
		}
		return nullptr;
	}

	void ModuleManager::unloadModule(Module* module) {
#ifdef WIN32
		FreeLibrary((HINSTANCE)module->handle);
#else      
		dlclose(module->handle);
#endif
		module->handle = nullptr;
		Log::info("module %s unloaded\n", module->name.c_str());

		for (int i = 0; i < modules.size(); i++) {
			if (modules[i].get() == module) {
				modules.erase(modules.begin() + i);
				break;
			}
		}
	}

	void ModuleManager::unloadModule(const std::string& name) {
		for (auto& module : modules) {
			if (module->name == name) {
				unloadModule(module.get());
				break;
			}
		}
	}

	std::vector<Module*> ModuleManager::getLoadedModules() {
		std::vector<Module*> list;
		for (auto& m : modules) {
			list.push_back(m.get());
		}
		return list;
	}

	std::vector<std::string> ModuleManager::getInstalledModules() {
		std::vector<std::string> list;

		for (auto& directory : moduleDirectories) {
			for (auto &file : std::filesystem::directory_iterator(directory)) {
				if (file.is_regular_file()) {
					if (file.path().extension() == MODULE_FILE_EXTENSION) {
						list.push_back(std::filesystem::relative(file.path(), directory).stem().string());
					}
				}
			}
		}

		return list;
	}

	ModuleManager* getModuleManager() {
		static ModuleManager moduleManager;
		return &moduleManager;
	}

}
