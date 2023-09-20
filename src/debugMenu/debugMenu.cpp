//
// Copyright (c) 2023 Julian Hinxlage. All rights reserved.
//

#include <stdio.h>
#include "common/Log.h"
#include "core/ModuleManager.h"
#include "core/Config.h"
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
					ImGui::Text("file %s", module->file.c_str());
					ImGui::TreePop();
				}
			}
		}

		window->endWindow();
	}
}

void updateCommandsWindow() {
	auto* window = Singleton::get<baseline::Window>();
	if (window->beginWindow("commands")) {
		auto* config = Singleton::get<Config>();
		for(auto &command : config->getCommandList()){
			if (ImGui::Button(command.c_str())) {
				config->execute(command);
			}
		}

		static std::string command;
		ImGui::InputText("command", &command);
		if (ImGui::Button("execute")) {
			config->execute(command);
		}

		window->endWindow();
	}
}

void updateVarsWindow() {
	auto* window = Singleton::get<baseline::Window>();
	if (window->beginWindow("variables")) {
		auto* config = Singleton::get<Config>();
		for (auto& var : config->getVarList()) {
			ImGui::Text("%s = %s", var.c_str(), config->getVar(var)->getString().c_str());
		}
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
		updateModulesWindow();
		updateCommandsWindow();
		updateVarsWindow();
	});
}


