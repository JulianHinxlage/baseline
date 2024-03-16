//
// Copyright (c) 2024 Julian Hinxlage. All rights reserved.
//

#include "TaskManager.h"
#include "common/Clock.h"
#include "common/strutil.h"

#define LOCK(mutexName) std::unique_lock<std::mutex> lock_##mutexName(mutexName);

namespace baseline {

	thread_local TaskManager::Thread* TaskManager::currentThread = nullptr;

	int TaskManager::addTask(const std::function<void()>& callback, TaskType type, const std::string& name, uint64_t delayMillis, void* owner, bool singleThreading) {
		int id = 0;
		{
			LOCK(taskDataMutex);
			id = nextTaskId++;

			Task task;
			task.taskId = id;
			task.name = name;
			task.createTime = Clock::nowMilli();
			task.startTime = task.createTime + delayMillis;
			task.reccuringInterval = delayMillis;
			task.type = type;
			task.owner = owner;
			task.singleThreading = singleThreading;
			task.callback = callback;
			task.state = TaskState::CREATED;
			tasks[id] = task;
		}
		scheduleTask(id);
		return id;
	}

	int TaskManager::getCurrentTaskId() {
		if (currentThread) {
			return currentThread->taskId;
		}
		else {
			return 0;
		}
	}

	void TaskManager::joinTask(int taskId) {
		std::unique_lock<std::mutex> lock(taskDataMutex);
		Task& task = getTask(taskId);
		while (task.state != TaskState::FINIESHED && task.state != TaskState::TERMINATED) {

			//stop reccuring task on join
			if (task.type == RECURRING) {
				task.type = TaskType::DELAYED;
				if (!(task.state == TaskState::RUNNING || task.state == TaskState::PAUSED)) {
					task.state = TaskState::FINIESHED;
					break;
				}
			}

			//run joining task on thread if it would lock up another task
			int currentTaskId = getCurrentTaskId();
			if (currentTaskId != 0) {
				Task& currentTask = getTask(currentTaskId);
				if (task.state == TaskState::SCHEDULED && currentTask.state == TaskState::RUNNING) {
					currentTask.state = TaskState::PAUSED;
					lock.unlock();
					runTask(taskId);
					lock.lock();
					currentThread->taskId = currentTaskId;
					currentTask.state = TaskState::RUNNING;
					continue;
				}
			}

			taskFinished.wait(lock);
		}
	}

	void TaskManager::joinTasksByOwner(void* owner) {
		for (auto& i : tasks) {
			Task& task = i.second;
			if (task.owner == owner) {
				joinTask(task.taskId);
			}
		}
	}

	void TaskManager::terminateTask(int taskId) {
		std::unique_lock<std::mutex> lock(taskDataMutex);
		Task& task = getTask(taskId);

		if (task.state == TaskState::RUNNING || task.state == TaskState::PAUSED) {
			std::unique_lock<std::mutex> threadLock(threadDataMutex);

			for (auto& thread : threads) {
				if (thread->taskId == taskId) {
					thread->terminate();
					if (thread->isWorker) {
						threadLock.unlock();
						addThread([&]() {
							runWorker();
						}, thread->name);
						break;
					}
				}
			}
		}
		if (task.state != TaskState::FINIESHED) {
			task.state = TaskState::TERMINATED;
		}
	}

	void TaskManager::start(int workerCount) {
		stop();
		currentThread = &defaultThread;

		for (int i = 0; i < workerCount; i++) {
			addThread([&]() {
				runWorker();
			}, "worker_" + toString(i));
		}
		addThread([&]() {
			runTimer();
		}, "timer");
	}

	void TaskManager::stop(bool joinTasks, bool runAllTasks) {
		LOCK(threadDataMutex);

		if (runAllTasks) {
			for (auto& i : tasks) {
				joinTask(i.first);
			}
		}

		for (auto& thread : threads) {
			thread->running = false;
		}
		wakeupWorker.notify_all();
		wakeupTimer.notify_all();


		for (auto& thread : threads) {
			if (joinTasks) {
				thread->join();
			}
			else {
				thread->terminate();
				if (thread->taskId != 0) {
					getTask(thread->taskId).state = TaskState::TERMINATED;
				}
			}
		}
		threads.clear();
	}

	TaskManager::Task& TaskManager::getTask(int taskId) {
		auto i = tasks.find(taskId);
		if (i == tasks.end()) {
			return defaultTask;
		}
		return i->second;
	}

	int TaskManager::addThread(const std::function<void()>& callback, const std::string& name) {
		LOCK(threadDataMutex);
		int id = nextThreadId++;

		auto thread = std::make_shared<Thread>();
		thread->threadId = id;
		thread->name = name;
		thread->state = ThreadState::CREATED;
		thread->running = true;
		thread->thread = new std::thread([thread, callback]() {
			currentThread = thread.get();
			thread->state = ThreadState::WAIT_FOR_TASK;
			if (callback) {
				callback();
			}
			if (thread->state != ThreadState::TERMINATED) {
				thread->state = ThreadState::FINISHED;
			}
		});
		threads.push_back(thread);

		return id;
	}

	void TaskManager::runTask(int taskId) {
		std::unique_lock<std::mutex> lock(taskDataMutex);
		Task& task = getTask(taskId);
		task.state = TaskState::RUNNING;
		if (currentThread) {
			currentThread->taskId = task.taskId;
		}
		if (task.callback) {
			lock.unlock();
			task.callback();
			lock.lock();
		}
		if (currentThread) {
			currentThread->taskId = 0;
		}

		if (task.type == TaskType::RECURRING) {
			task.state = TaskState::CREATED;
			task.startTime = task.startTime + task.reccuringInterval;
			scheduleTask(task.taskId);
		}
		else {
			task.state = TaskState::FINIESHED;
			taskFinished.notify_all();
		}
	}

	void TaskManager::runWorker() {
		if (currentThread) {
			currentThread->isWorker = true;
			while (currentThread->running) {
				bool hasRunTask = false;

				std::unique_lock<std::mutex> lock(taskDataMutex);
				for (auto& i : tasks) {
					Task& task = i.second;
					if (task.state == TaskState::SCHEDULED) {
						hasRunTask = true;
						currentThread->state = ThreadState::RUNNING_TASK;
						task.state = TaskState::RUNNING;
						lock.unlock();
						runTask(task.taskId);
						lock.lock();
						if (currentThread->state != ThreadState::TERMINATED) {
							currentThread->state = ThreadState::WAIT_FOR_TASK;
						}
						break;
					}
				}

				if (!hasRunTask) {
					wakeupWorker.wait(lock);
				}
			}
		}
	}

	void TaskManager::runTimer() {
		if (currentThread) {
			while (currentThread->running) {
				uint64_t now = Clock::nowMilli();
				uint64_t minWait = -1;
				bool hasScheduledTask = false;

				std::unique_lock<std::mutex> lock(taskDataMutex);
				for (auto& i : tasks) {
					Task& task = i.second;
					if (task.state == TaskState::WAITING) {
						if (task.startTime <= now) {
							lock.unlock();
							scheduleTask(task.taskId);
							lock.lock();
							hasScheduledTask = true;
						}
						else {
							uint64_t wait = task.startTime - now;
							if (wait < minWait) {
								minWait = wait;
							}
						}
					}
				}

				if (!hasScheduledTask) {
					if (minWait == -1) {
						wakeupTimer.wait(lock);
					}
					else {
						wakeupTimer.wait_for(lock, std::chrono::microseconds(minWait));
					}
				}
			}
		}
	}

	void TaskManager::scheduleTask(int taskId) {
		Task &task = getTask(taskId);
		bool schedule = false;

		if (task.state == TaskState::CREATED) {
			if (task.type == TaskType::DELAYED || task.type == TaskType::RECURRING) {
				task.state = TaskState::WAITING;
				wakeupTimer.notify_one();
			}
			else if (task.type == TaskType::THREAD) {
				addThread([&, taskId]() {
					currentThread->state = ThreadState::RUNNING_TASK;
					runTask(taskId);
				}, task.name);
			}
			else {
				schedule = true;
			}
		}
		else if (task.state == TaskState::WAITING) {
			schedule = true;
		}


		if (schedule) {
			if (task.singleThreading) {
				runTask(taskId);
			}
			else {
				task.state = TaskState::SCHEDULED;
				wakeupWorker.notify_one();
			}
		}
	}

	void TaskManager::Thread::join() {
		if (thread) {
			if (thread->joinable()) {
				thread->join();
			}
			else {
				thread->detach();
			}
			delete thread;
			thread = nullptr;
		}
	}

	void TaskManager::Thread::terminate() {
		running = false;
		if (thread) {
			thread->detach();
			delete thread;
			thread = nullptr;
			state = ThreadState::TERMINATED;
		}
	}

	TaskManager::Thread& TaskManager::getThread(int threadId) {
		for (auto& thread : threads) {
			if (thread->threadId == threadId) {
				return *thread.get();
			}
		}
		return defaultThread;
	}

	std::vector<int> TaskManager::getTaskIds() {
		LOCK(taskDataMutex)
		std::vector<int> ids;
		for (auto& i : tasks) {
			ids.push_back(i.first);
		}
		return ids;
	}

	std::vector<int> TaskManager::getThreadIds() {
		LOCK(threadDataMutex)
		std::vector<int> ids;
		for (auto& thread : threads) {
			ids.push_back(thread->threadId);
		}
		return ids;
	}

	TaskState TaskManager::getTaskState(int taskId) {
		return getTask(taskId).state;
	}

	ThreadState TaskManager::getThreadState(int threadId) {
		return getThread(threadId).state;
	}

	int TaskManager::getTaskByThread(int threadId) {
		return getThread(threadId).taskId;
	}

	std::string TaskManager::getTaskName(int taskId) {
		return getTask(taskId).name;
	}

	std::string TaskManager::getThreadName(int threadId) {
		return getThread(threadId).name;
	}

}
