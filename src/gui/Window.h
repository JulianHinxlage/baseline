//
// Copyright (c) 2023 Julian Hinxlage. All rights reserved.
//

#pragma once

#include <string>
#include <map>
#include <memory>
#include <functional>
#include <glm/glm/glm.hpp>

namespace baseline {

	class Window {
	public:
		bool alwaysRefresh = false;
		bool enableLayout = true;

		bool init(int width, int height, const std::string &title, int swapInterval = 1, bool maximized = false);
		void beginFrame();
		void endFrame();
		void shutdown();
		void close();
		bool isOpen();
		void* getContext();
		void* getNativContext();

		void setPosition(int x, int y);
		void setSize(int x, int y);
		void setTitle(const std::string& title);
		glm::vec2 getMonitorResolution();

		bool beginWindow(const std::string &name, int flags = 0, const std::string &menu = "View");
		void endWindow();
		void refresh();

		int addUpdateCallback(const std::function<void()> &callback);
		void removeUpdateCallback(int id);
		void invokeUpdateCallbacks();
	private:
		class SubWindow {
		public:
			bool open = false;
			bool called = false;
			bool visible = false;
			std::string name = "";
			std::string menu = "";
		};
		std::map<std::string, std::shared_ptr<SubWindow>> subWindows;
		std::shared_ptr<SubWindow> currentSubWindow;
		void* context = nullptr;
		void* imguiContext = nullptr;
		std::string layoutFile;

		class UpdateCallback {
		public:
			int id = 0;
			std::function<void()> callback = nullptr;
		};
		std::vector<UpdateCallback> updateCallbacks;
		int nextUpdateCallbackId = 0;

		void updateMenu();
		void initMenu();
	};

}
