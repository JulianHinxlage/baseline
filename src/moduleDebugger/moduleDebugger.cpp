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

int main(int arc, char* argv[]) {
	baseline::Window window;

	window.alwaysRefresh = false;
	window.init(1080, 720, "Baseline Module Debugger", 2);

	while (window.isOpen()) {
		window.beginFrame();

		if (window.beginWindow("modules")) {

			if (ImGui::Button("load")) {
				ImGui::OpenPopup("load module");
			}

			if (ImGui::BeginPopup("load module")) {
				for (auto& file : moduleManager->getInstalledModules()) {
					if (ImGui::MenuItem(file.c_str())) {
						moduleManager->loadModule(file);
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

			window.endWindow();
		}

		window.endFrame();
	}

	window.shutdown();
	return 0;
}