//
// Copyright (c) 2023 Julian Hinxlage. All rights reserved.
//

#include "core/Launcher.h"
#include "common/Log.h"

using namespace baseline;

#define STR(value) #value
#define STR2(value) STR(value)
#ifndef BL_LAUNCH_CONFIG
#define BL_LAUNCH_CONFIG launch.cfg
#endif
#define BL_LAUNCH_CONFIG_STR STR2(BL_LAUNCH_CONFIG)

int main(int argc, char* argv[]) {
	auto* launcher = Singleton::get<Launcher>();

	Log::info("directory:  %s", std::filesystem::current_path().string().c_str());
	Log::info("executable: %s", argv[0]);

	launcher->init(argc, argv, BL_LAUNCH_CONFIG_STR);

	system("pause");

	launcher->joinAll();
	launcher->shutdown();
	return 0;
}
