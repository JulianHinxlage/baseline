//
// Copyright (c) 2023 Julian Hinxlage. All rights reserved.
//

#include <stdio.h>
#include "common/Log.h"
#include "core/Config.h"
#include "core/ModuleManager.h"
#include "Window.h"
#include <imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

using namespace baseline;

int main(int arc, char* argv[]) {
	auto* window = Singleton::get<Window>();
	auto* config = Singleton::get<Config>();
	std::string title = config->getValue<std::string>("title", "Baseline GUI");

	window->alwaysRefresh = false;
	window->init(1080, 720, title, 2);

	while (window->isOpen()) {
		window->beginFrame();
		window->invokeUpdateCallbacks();
		window->endFrame();
	}

	window->shutdown();
	return 0;
}