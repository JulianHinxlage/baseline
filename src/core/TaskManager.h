//
// Copyright (c) 2024 Julian Hinxlage. All rights reserved.
//

#pragma once

#include <vector>
#include <map>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <memory>

namespace baseline {

	enum class TaskState {
		UNKNOWN,
		CREATED,
		WAITING,
		SCHEDULED,
		RUNNING,
		PAUSED,
		FINIESHED,
		TERMINATED,
	};

	enum TaskType {
		UNKONWN,
		NORMAL,
		THREAD,
		DELAYED,
		RECURRING,
	};

	enum class ThreadState {
		UNKNOWN,
		CREATED,
		WAIT_FOR_TASK,
		RUNNING_TASK,
		FINISHED,
		TERMINATED,
	};

	class TaskManager {
	public:
		int addTask(const std::function<void()> &callback, TaskType type = TaskType::NORMAL, const std::string &name = "", uint64_t delayMillis = 0, void *owner = nullptr, bool singleThreading = false);
		int getCurrentTaskId();
		void joinTask(int taskId);
		void joinTasksByOwner(void *owner);
		void terminateTask(int taskId);
		void start(int workerCount);
		void stop(bool joinTasks = true, bool runAllTasks = false);


		std::vector<int> getTaskIds();
		std::vector<int> getThreadIds();
		TaskState getTaskState(int taskId);
		ThreadState getThreadState(int threadId);
		int getTaskByThread(int threadId);
		std::string getTaskName(int taskId);
		std::string getThreadName(int threadId);

	private:
		class Thread {
		public:
			int threadId = 0;
			std::string name = "";
			ThreadState state = ThreadState::UNKNOWN;
			int taskId = 0;
			std::thread* thread = nullptr;
			std::atomic_bool running = false;
			bool isWorker = false;

			void join();
			void terminate();
		};

		class Task {
		public:
			int taskId = 0;
			std::string name = "";
			TaskState state = TaskState::UNKNOWN;
			TaskType type = TaskType::UNKONWN;
			void* owner = nullptr;
			bool singleThreading = false;
			std::function<void()> callback = nullptr;

			//unit: milliseconds
			uint64_t createTime = 0;
			uint64_t startTime = 0;
			uint64_t reccuringInterval = 0;
		};

		int nextThreadId = 1;
		int nextTaskId = 1;

		std::vector<std::shared_ptr<Thread>> threads;
		std::map<int, Task> tasks;
		static thread_local Thread* currentThread;

		std::mutex threadDataMutex;
		std::mutex taskDataMutex;
		std::condition_variable wakeupWorker;
		std::condition_variable taskFinished;
		std::condition_variable wakeupTimer;

		Task defaultTask;
		Thread defaultThread;
		
		Task &getTask(int taskId);
		Thread& getThread(int threadId);
		int addThread(const std::function<void()>& callback, const std::string& name = "");
		void runTask(int taskId);
		void runWorker();
		void runTimer();
		void scheduleTask(int taskId);
	};

}
