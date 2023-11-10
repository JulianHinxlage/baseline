//
// Copyright (c) 2023 Julian Hinxlage. All rights reserved.
//

#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace baseline {

	class Timer {
	public:
		typedef std::function<void()> Callback;
		
		Timer();
		~Timer();
		int add(int timeMilliseconds, const std::function<void()>& callback, bool repeat = false);
		void remove(int id);
		void start();
		void stop();

	private:
		class Handle {
		public:
			Callback callback = nullptr;
			int id = 0;
			uint64_t time = 0;
			uint64_t duration = 0;
			bool repeat = false;
		};
		std::vector<Handle> handles;
		std::thread* thread = nullptr;
		std::mutex mutex;
		std::condition_variable cv;
		bool running = false;
		int nextId = 1;
	};

}