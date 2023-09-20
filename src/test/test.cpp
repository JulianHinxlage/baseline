//
// Copyright (c) 2023 Julian Hinxlage. All rights reserved.
//

#include <stdio.h>
#include "common/Log.h"
#include "core/ModuleManager.h"
#include "gui/Window.h"
#include <imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

using namespace baseline;

void updateTestWindow() {
	auto* window = Singleton::get<baseline::Window>();
	if (window->beginWindow("Test")) {
		window->endWindow();
	}
}

int index = -1;

extern "C" void unload() {
	if (index != -1) {
		auto* window = Singleton::get<baseline::Window>();
		window->updateCallbacks.erase(window->updateCallbacks.begin() + index);
	}
}

extern "C" void load() {
	auto* window = Singleton::get<baseline::Window>();
	window->updateCallbacks.push_back([]() {
		updateTestWindow();
	});
	index = window->updateCallbacks.size() - 1;
}
