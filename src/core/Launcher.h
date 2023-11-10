//
// Copyright (c) 2023 Julian Hinxlage. All rights reserved.
//

#include "core/ModuleManager.h"
#include "core/FileWatcher.h"
#include "core/Config.h"
#include "common/Log.h"
#include <filesystem>
#include <thread>

namespace baseline {

	class Launcher {
	public:
		Launcher();
		void init(int argc, char* argv[], const std::string& configFile = "launch.cfg");
		void load(const std::string& name);
		void run(const std::string& name, const std::string& function, bool onOwnThread = false);
		void joinAll();
		void shutdown();

	private:
		std::vector<std::thread*> threads;
	};

}
