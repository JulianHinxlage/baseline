//
// Copyright (c) 2023 Julian Hinxlage. All rights reserved.
//

#include "FileWatcher.h"
#include <filesystem>

namespace baseline {

	uint64_t getTimestamp(const std::string& file) {
		try {
			return std::filesystem::last_write_time(file).time_since_epoch().count();
		}
		catch (...) {
			return 0;
		}
	}

	FileWatcher::FileWatcher() {
		start();
	}

	FileWatcher::~FileWatcher() {
		stop();
	}

	void FileWatcher::start() {
		stop();
		running = true;

		thread = new std::thread([&]() {
			while (running) {
				{
					std::unique_lock<std::mutex> lock(mutex);
					for (int i = 0; i < handles.size(); i++) {
						auto& handle = handles[i];
						uint64_t timestamp = getTimestamp(handle.file);
						if (timestamp != handle.timestamp) {
							handle.timestamp = timestamp;
							if (handle.callback) {
								lock.unlock();
								handle.callback();
								lock.lock();
							}
							if (handle.onlyOneInvoke) {
								handle.callback = nullptr;
								handles.erase(handles.begin() + i);
								i--;
							}
						}
					}
				}
				std::this_thread::sleep_for(std::chrono::milliseconds((uint64_t)(checkTimeInterval * 1000)));
			}

		});
	}

	void FileWatcher::stop() {
		if (thread) {
			running = false;
			thread->join();
			delete thread;
			thread = nullptr;
		}
	}

	void FileWatcher::addFile(const std::string& file, const Callback& callback) {
		std::unique_lock<std::mutex> lock(mutex);

		std::string absolut = std::filesystem::absolute(file).string();
		for (auto& handle : handles) {
			if (handle.file == absolut) {
				handle.callback = callback;
				return;
			}
		}

		Handle handle;
		handle.file = absolut;
		handle.callback = callback;
		handle.timestamp = getTimestamp(absolut);

		handles.push_back(handle);
	}

	void FileWatcher::removeFile(const std::string& file) {
		std::unique_lock<std::mutex> lock(mutex);

		std::string absolut = std::filesystem::absolute(file).string();
		for (int i = 0; i < handles.size(); i++) {
			auto& handle = handles[i];
			if (handle.file == absolut) {
				handles.erase(handles.begin() + i);
				return;
			}
		}
	}

}
