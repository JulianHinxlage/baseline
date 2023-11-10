//
// Copyright (c) 2023 Julian Hinxlage. All rights reserved.
//

#include "Timer.h"

namespace baseline {

	uint64_t nowMillis() {
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	}

	Timer::Timer() {
		start();
	}

	Timer::~Timer() {
		stop();
	}

	int Timer::add(int timeMilliseconds, const std::function<void()>& callback, bool repeat) {
		std::unique_lock<std::mutex> lock(mutex);
		Handle handle;
		handle.callback = callback;
		handle.id = nextId++;
		handle.duration = timeMilliseconds;
		handle.time = nowMillis() + timeMilliseconds;
		handle.repeat = repeat;
		handles.push_back(handle);
		cv.notify_one();
		return handle.id;
	}

	void Timer::remove(int id) {
		std::unique_lock<std::mutex> lock(mutex);
		for (int i = 0; i < handles.size(); i++) {
			if (handles[i].id == id) {
				handles.erase(handles.begin() + i);
				break;
			}
		}
		cv.notify_one();
	}

	void Timer::start() {
		stop();
		running = true;
		thread = new std::thread([&]() {
			while (running) {
				uint64_t now = nowMillis();
				std::unique_lock<std::mutex> lock(mutex);
				uint64_t next = -1;
				int nextIndex = -1;
				bool invoked = false;

				for (int i = 0; i < handles.size(); i++) {
					auto handle = handles[i];
					if (handle.time <= now) {
						if (handle.repeat) {
							handles[i].time = now + handle.duration;
						}
						else {
							handles.erase(handles.begin() + i);
						}
						invoked = true;
						if (handle.callback) {
							lock.unlock();
							handle.callback();
							lock.lock();
						}
						break;
					}
					if (handle.time < next || next == -1) {
						next = handle.time;
						nextIndex = i;
					}
				}

				if (!invoked) {
					if (nextIndex != -1) {
						cv.wait_for(lock, std::chrono::microseconds(next - now));
					}
					else {
						cv.wait(lock);
					}
				}
			}
		});
	}

	void Timer::stop() {
		running = false;
		if (thread) {
			cv.notify_one();
			thread->join();
			delete thread;
			thread = nullptr;
		}
	}

}