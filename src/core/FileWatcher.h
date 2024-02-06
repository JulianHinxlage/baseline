//
// Copyright (c) 2023 Julian Hinxlage. All rights reserved.
//

#pragma once

#include <functional>
#include <thread>
#include <vector>
#include <mutex>

namespace baseline {

	class FileWatcher {
	public:
		double checkTimeInterval = 2;

		typedef std::function<void()> Callback;
		FileWatcher();
		~FileWatcher();
		void start();
		void stop();
		void addFile(const std::string& file, const Callback& callback, bool onlyOneInvoke = false);
		void removeFile(const std::string& file);
	private:
		class Handle {
		public:
			std::string file = "";
			Callback callback = nullptr;
			uint64_t timestamp = 0;
			bool onlyOneInvoke = false;
		};
		std::vector<Handle> handles;
		std::thread* thread = nullptr;
		std::mutex mutex;
		bool running = false;
	};

}
