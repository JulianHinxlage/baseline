//
// Copyright (c) 2023 Julian Hinxlage. All rights reserved.
//

#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <functional>

namespace baseline {

	class Window {
	public:
		bool alwaysRefresh = false;

		bool init(int width, int height, const std::string &title, int swapInterval = 1);
		void beginFrame();
		void endFrame();
		void shutdown();
		void close();
		bool isOpen();
		void* getContext();

		void setPosition(int x, int y);

		bool beginWindow(const std::string &name);
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
			std::string name = "";
		};
		std::unordered_map<std::string, std::shared_ptr<SubWindow>> subWindows;
		std::shared_ptr<SubWindow> currentSubWindow;
		void* context = nullptr;
		void* imguiContext = nullptr;

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
