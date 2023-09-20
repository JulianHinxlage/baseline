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

void updateModulesWindow() {
	auto* window = Singleton::get<baseline::Window>();
	if (window->beginWindow("modules")) {
		auto* moduleManager = Singleton::get<ModuleManager>();

		if (ImGui::Button("load")) {
			ImGui::OpenPopup("load module");
		}

		if (ImGui::BeginPopup("load module")) {
			for (auto& file : moduleManager->getInstalledModules()) {
				if (ImGui::MenuItem(file.c_str())) {
					auto *module = moduleManager->loadModule(file);
					if (module) {
						module->invoke("main");
					}
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::EndPopup();
		}

		for (auto& module : moduleManager->getLoadedModules()) {
			if (module) {
				if (ImGui::TreeNode(module->name.c_str())) {
					if (ImGui::Button("unload")) {
						moduleManager->unloadModule(module);
						ImGui::TreePop();
						break;
					}
					if (ImGui::Button("execute")) {
						module->invoke("main");
					}

					ImGui::Text("file %s", module->file.c_str());
					ImGui::TreePop();
				}
			}
		}

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

int main(int arc, char* argv[]) {
	auto* window = Singleton::get<baseline::Window>();
	window->updateCallbacks.push_back([]() {
		updateModulesWindow();
	});
	index = window->updateCallbacks.size() - 1;
	return 0;
}


