//
// Copyright (c) 2023 Julian Hinxlage. All rights reserved.
//

#include "Window.h"
#include "common/strutil.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#if WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#endif

static void glfw_error_callback(int error, const char* description) {
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

namespace baseline {

	bool Window::init(int width, int height, const std::string& title, int swapInterval) {
		//init glfw
		glfwSetErrorCallback(glfw_error_callback);
		if (!glfwInit()) {
			printf("failed to initialize GLFW\n");
			return false;
		}

		//create window
		const char* glsl_version = "#version 130";
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
		context = (void*)glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
		if (!context) {
			printf("failed to create Window\n");
			return false;
		}
		glfwMakeContextCurrent((GLFWwindow*)context);
		glfwSwapInterval(swapInterval);

		//init glew
		if (glewInit() != GLEW_OK) {
			printf("failed to initialize OpenGL\n");
			return false;
		}


		//init imgui
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		imguiContext = ImGui::GetCurrentContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		ImGui::StyleColorsDark();
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}
		io.IniFilename = "layout.ini";

		if (io.BackendPlatformUserData == nullptr) {
			ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)context, true);
			ImGui_ImplOpenGL3_Init(glsl_version);
		}

		//set imgui style colors
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 1, 1, 0.3));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1, 1, 1, 0.4));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1, 1, 1, 0.5));
		ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(1, 1, 1, 0.2));
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(1, 1, 1, 0.3));
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(1, 1, 1, 0.4));
		ImGui::PushStyleColor(ImGuiCol_Tab, ImVec4(1, 1, 1, 0.1));
		ImGui::PushStyleColor(ImGuiCol_TabActive, ImVec4(1, 1, 1, 0.25));
		ImGui::PushStyleColor(ImGuiCol_TabHovered, ImVec4(1, 1, 1, 0.40));
		ImGui::PushStyleColor(ImGuiCol_TabUnfocused, ImVec4(1, 1, 1, 0.1));
		ImGui::PushStyleColor(ImGuiCol_TabUnfocusedActive, ImVec4(1, 1, 1, 0.25));
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1, 1, 1, 0.35));
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(1, 1, 1, 0.45));
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(1, 1, 1, 0.55));
		ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(1, 1, 1, 0.35));
		ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImVec4(1, 1, 1, 0.35));
		ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4(1, 1, 1, 0.45));
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.116, 0.125, 0.133, 1));
		ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.177, 0.177, 0.177, 1));
		ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.238, 0.238, 0.238, 1));
		ImGui::PushStyleColor(ImGuiCol_DragDropTarget, ImVec4(0.0, 0.32, 1.0, 1));
		style.WindowMenuButtonPosition = ImGuiDir_None;

		currentSubWindow = nullptr;
		initMenu();
	}

	void Window::beginFrame() {
		if (!alwaysRefresh) {
			glfwWaitEvents();
		}
		glfwPollEvents();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::DockSpaceOverViewport();

		updateMenu();
	}

	void Window::endFrame() {
		ImGui::Render();
		int x = 0;
		int y = 0;
		glfwGetFramebufferSize((GLFWwindow*)context, &x, &y);
		glViewport(0, 0, x, y);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		ImGuiIO& io = ImGui::GetIO(); (void)io;
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}

		glfwSwapBuffers((GLFWwindow*)context);
	}

	void Window::shutdown() {
		if (context) {
			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplGlfw_Shutdown();
			ImGui::DestroyContext();
			glfwDestroyWindow((GLFWwindow*)context);
			glfwTerminate();
		}
	}

	void Window::close() {
		glfwSetWindowShouldClose((GLFWwindow*)context, 1);
	}

	bool Window::isOpen() {
		return glfwWindowShouldClose((GLFWwindow*)context) == 0;
	}

	void* Window::getContext() {
		return context;
	}

	void* Window::getNativContext() {
#if WIN32
		return glfwGetWin32Window((GLFWwindow*)context);
#else
		return nullptr;
#endif
	}

	void Window::setPosition(int x, int y) {
		glfwSetWindowPos((GLFWwindow*)context, x, y);
	}

	void Window::setSize(int x, int y) {
		glfwSetWindowSize((GLFWwindow*)context, x, y);
	}

	void Window::setTitle(const std::string& title) {
		glfwSetWindowTitle((GLFWwindow*)context, title.c_str());
	}

	bool Window::beginWindow(const std::string& name, int flags) {
		auto sub = subWindows[name];
		if (!sub) {
			subWindows[name] = std::make_shared<SubWindow>();
			sub = subWindows[name];
			sub->name = name;
			sub->open = false;
		}

		currentSubWindow = nullptr;
		sub->called = true;
		if (sub->open) {
			currentSubWindow = sub;
			ImGui::SetCurrentContext((ImGuiContext*)imguiContext);
			if (ImGui::Begin(sub->name.c_str(), &sub->open, ImGuiWindowFlags_NoCollapse | flags)) {
				return true;
			}
		}

		return false;
	}

	void Window::endWindow() {
		if (currentSubWindow) {
			ImGui::End();
			currentSubWindow = nullptr;
		}
	}

	void Window::refresh() {
		glfwPostEmptyEvent();
	}

	int Window::addUpdateCallback(const std::function<void()>& callback) {
		UpdateCallback update;
		update.callback = callback;
		update.id = nextUpdateCallbackId++;
		updateCallbacks.push_back(update);
		return update.id;
	}

	void Window::removeUpdateCallback(int id) {
		for (int i = 0; i < updateCallbacks.size(); i++) {
			if (updateCallbacks[i].id == id) {
				updateCallbacks.erase(updateCallbacks.begin() + i);
				break;
			}
		}
	}

	void Window::invokeUpdateCallbacks() {
		for (int i = 0; i < updateCallbacks.size(); i++) {
			auto& update = updateCallbacks[i];
			if (update.callback) {
				update.callback();
			}
		}
	}

	void Window::updateMenu() {
		if (ImGui::BeginMainMenuBar()) {
			if (subWindows.size() > 0) {
				if (ImGui::BeginMenu("View")) {
					for (auto& sub : subWindows) {
						if (sub.second->called) {
							ImGui::MenuItem(sub.second->name.c_str(), nullptr, &sub.second->open);
						}
					}
					ImGui::EndMenu();
				}
			}
			ImGui::EndMainMenuBar();
		}

		for (auto& sub : subWindows) {
			sub.second->called = false;
		}
	}

	void Window::initMenu() {
		ImGuiSettingsHandler handler;
		handler.UserData = this;
		handler.TypeName = "OpenFlags";
		handler.TypeHash = ImHashStr("OpenFlags");
		handler.ReadOpenFn = [](ImGuiContext* ctx, ImGuiSettingsHandler* handler, const char* name) -> void* {
			if (std::string(name) == "") {
				return (void*)1;
			}
			else {
				return nullptr;
			}
		};

		handler.ReadLineFn = [](ImGuiContext* ctx, ImGuiSettingsHandler* handler, void* entry, const char* line) {
			Window* window = (Window*)handler->UserData;
			auto parts = split(line, "=");
			if (parts.size() >= 2) {
				std::string name = parts[0];
				auto sub = window->subWindows[name];
				if (!sub) {
					window->subWindows[name] = std::make_shared<SubWindow>();
					sub = window->subWindows[name];
					sub->name = name;
					sub->open = false;
				}

				try {
					sub->open = std::stoi(parts[1]);
				}
				catch (...) {}
			}
		};

		handler.WriteAllFn = [](ImGuiContext* ctx, ImGuiSettingsHandler* handler, ImGuiTextBuffer* buf) {
			Window* window = (Window*)handler->UserData;
			buf->append("[OpenFlags][]\n");
			for (auto& sub : window->subWindows) {
				buf->appendf("%s=%i\n", sub.second->name.c_str(), (int)sub.second->open);
			}
			buf->appendf("\n");
		};

		ImGui::GetCurrentContext()->SettingsHandlers.push_back(handler);
	}

}
