//
// Copyright (c) 2023 Julian Hinxlage. All rights reserved.
//

#pragma once

#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace baseline {

	class ThreadPool {
	public:
		ThreadPool(int threadCount = 0);

		~ThreadPool();

		void stop();

		void start(int threadCount);

		void joinAllTasks();

		void joinTask(int taskId);

		int addTask(const std::function<void()>& callback);

		int getThreadCount();

	private:
		class Worker {
		public:
			std::thread* thread = nullptr;
			int activeTaskId = -1;

			~Worker();

			void stop();
		};

		class Task {
		public:
			int id;
			std::function<void()> callback;
		};

		std::vector<std::shared_ptr<Worker>> workers;
		std::vector<Task> tasks;
		std::mutex mutex;
		std::condition_variable wakeupWorker;
		std::atomic_bool running;
		std::atomic_int activeWorkerCount;
		std::condition_variable taskFinished;
		std::condition_variable allTasksFinished;
		int nextTaskId = 0;

		void runWorker(Worker* worker);
	};

}
