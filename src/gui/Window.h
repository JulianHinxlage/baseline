//
// Copyright (c) 2023 Julian Hinxlage. All rights reserved.
//

#pragma once

#include <string>
#include <unordered_map>
#include <memory>

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

		bool beginWindow(const std::string &name);
		void endWindow();
		void refresh();

	private:
		class SubWindow {
		public:
			bool open = false;
			bool called = false;
			std::string name = "";
		};
		std::unordered_map<std::string, std::shared_ptr<SubWindow>> subWindows;
		std::shared_ptr<SubWindow> currentSubWindow;
		void *context;

		void updateMenu();
		void initMenu();
	};

}
