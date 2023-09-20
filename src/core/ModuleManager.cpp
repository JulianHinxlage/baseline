//
// Copyright (c) 2023 Julian Hinxlage. All rights reserved.
//

#include "ModuleManager.h"
#include "common/Log.h"
#include "Singleton.h"
#include "FileWatcher.h"
#include "Config.h"
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
		auto* config = Singleton::get<Config>();
		config->addVar("enableHotReloading", enableHotReloading);
		config->addCommand("loadModule", [&](const std::vector<std::string>& args) {
			if (args.size() > 0) {
				loadModule(args[0]);
			}
		});
		config->addCommand("unloadModule", [&](const std::vector<std::string>& args) {
			if (args.size() > 0) {
				unloadModule(args[0]);
			}
		});
	}

	void ModuleManager::addModuleDirectory(const std::string& directory) {
		std::string absolut = std::filesystem::absolute(directory).string();
		for (auto& directory : moduleDirectories) {
			if (directory == absolut) {
				return;
			}
		}
		moduleDirectories.push_back(absolut);
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
			Log::warning("module %s: file not found", name.c_str());
			return nullptr;
		}

		for (auto& module : modules) {
			if (module->file == file) {
				Log::warning("module %s: already loaded", name.c_str());
				return module.get();
			}
		}

		auto module = std::make_shared<Module>();
		module->name = name;
		module->file = file;
		module->runtimeFile = file;

		if (enableHotReloading) {
			for (int i = 0; i < 10; i++) {
				std::filesystem::path path(file);
				path = path.parent_path() / "runtime_files" / std::to_string(i) / path.filename();

				if (!std::filesystem::exists(path.parent_path())) {
					std::filesystem::create_directories(path.parent_path());
				}
				try {
					std::filesystem::copy(file, path, std::filesystem::copy_options::overwrite_existing);
					module->runtimeFile = path.string();
					break;
				}
				catch (...) {}
			}
		}

#if WIN32
		module->handle = (void*)LoadLibrary(module->runtimeFile.c_str());
		if (!module->handle) {
			Log::error("module %s could not be loaded", name.c_str());
		}
#else
		module->handle = dlopen(runtimePath.c_str(), RTLD_NOW | RTLD_LOCAL);
#endif

		if (enableHotReloading) {
			Singleton::get<FileWatcher>()->addFile(module->file, [&, name = module->name]() {
				bool tmp = enableHotReloading;
				enableHotReloading = false;
				unloadModule(name);
				enableHotReloading = tmp;
				loadModule(name);
			});
		}

		if (module->handle) {
			Log::info("module %s loaded, file: %s", name.c_str(), file.c_str());
		}

		modules.push_back(module);
		if (module) {
			module->invoke("load");
		}
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
		if (module) {
			module->invoke("unload");
		}
#ifdef WIN32
		FreeLibrary((HINSTANCE)module->handle);
#else      
		dlclose(module->handle);
#endif
		module->handle = nullptr;
		Log::info("module %s unloaded", module->name.c_str());

		if (enableHotReloading) {
			Singleton::get<FileWatcher>()->removeFile(module->file);
		}

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

}
