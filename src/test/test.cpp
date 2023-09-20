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
		ImGui::Text("hello from Test");
		window->endWindow();
	}
}

int id = -1;

extern "C" void unload() {
	if (id != -1) {
		auto* window = Singleton::get<baseline::Window>();
		window->removeUpdateCallback(id);
	}
}

extern "C" void load() {
	auto* window = Singleton::get<baseline::Window>();
	id = window->addUpdateCallback([]() {
		updateTestWindow();
	});
}
