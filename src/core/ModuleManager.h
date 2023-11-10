//
// Copyright (c) 2023 Julian Hinxlage. All rights reserved.
//

#pragma once

#include "Module.h"
#include "Singleton.h"
#include <vector>
#include <memory>
#include <functional>

namespace baseline {

	class ModuleManager {
	public:
		bool enableHotReloading = false;

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

	class OnModuleLoadCallback {
	public:
		std::function<void()> onLoad;
		std::function<void()> onUnload;

		OnModuleLoadCallback(const std::function<void()> &onLoad, const std::function<void()> &onUnload) {
			this->onLoad = onLoad;
			this->onUnload = onUnload;
			if (onLoad) {
				onLoad();
			}
		}

		~OnModuleLoadCallback() {
			if (onUnload) {
				onUnload();
			}
		}
	};

}

#define BL_UNIQUE_IDENTIFIER_IMPL_2(base, counter, line) base##_##line##_##counter
#define BL_UNIQUE_IDENTIFIER_IMPL(base, counter, line) BL_UNIQUE_IDENTIFIER_IMPL_2(base, counter, line)
#define BL_UNIQUE_IDENTIFIER(base) BL_UNIQUE_IDENTIFIER_IMPL(base, __COUNTER__, __LINE__)

#define ON_MODULE_LOAD_IMPL(func) void func(); baseline::OnModuleLoadCallback BL_UNIQUE_IDENTIFIER(onModuleLoadCallback)([]() { func(); }, nullptr); void func()
#define ON_MODULE_LOAD() ON_MODULE_LOAD_IMPL(BL_UNIQUE_IDENTIFIER(onModuleLoad))
#define ON_MODULE_UNLOAD_IMPL(func) void func(); baseline::OnModuleLoadCallback BL_UNIQUE_IDENTIFIER(onModuleUnloadCallback)(nullptr, []() { func(); }); void func()
#define ON_MODULE_UNLOAD() ON_MODULE_UNLOAD_IMPL(BL_UNIQUE_IDENTIFIER(onModuleUnload))
