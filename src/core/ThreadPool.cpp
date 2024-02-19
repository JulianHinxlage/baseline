//
// Copyright (c) 2023 Julian Hinxlage. All rights reserved.
//

#include "ThreadPool.h"

namespace baseline {

	ThreadPool::Worker::~Worker() {
		stop();
	}

	void ThreadPool::Worker::stop() {
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

	ThreadPool::ThreadPool(int threadCount) {
		if (threadCount != 0) {
			start(threadCount);
		}
	}

	ThreadPool::~ThreadPool() {
		stop();
	}

	void ThreadPool::stop() {
		running = false;
		wakeupWorker.notify_all();
		workers.clear();
		tasks.clear();
	}

	void ThreadPool::start(int threadCount) {
		workers.resize(threadCount);
		running = true;
		activeWorkerCount = 0;
		for (auto& w : workers) {
			if (w == nullptr) {
				w = std::make_shared<Worker>();
			}
			w->stop();
			w->thread = new std::thread([&]() {
				runWorker(w.get());
			});
		}
	}

	void ThreadPool::joinAllTasks() {
		std::unique_lock<std::mutex> lock(mutex);
		if (activeWorkerCount > 0 || tasks.size() != 0) {
			allTasksFinished.wait(lock);
		}
	}

	void ThreadPool::joinTask(int taskId) {
		std::unique_lock<std::mutex> lock(mutex);
		bool isTaskFinished = false;
		while (!isTaskFinished) {
			isTaskFinished = true;
			for (auto& t : tasks) {
				if (t.id == taskId) {
					isTaskFinished = false;
					break;
				}
			}
			if (isTaskFinished) {
				for (auto& w : workers) {
					if (w->activeTaskId == taskId) {
						isTaskFinished = false;
						break;
					}
				}
			}

			if (!isTaskFinished) {
				taskFinished.wait(lock);
			}
		}
	}

	int ThreadPool::addTask(const std::function<void()>& callback) {
		std::unique_lock<std::mutex> lock(mutex);
		Task task;
		task.callback = callback;
		task.id = nextTaskId++;
		tasks.push_back(task);
		wakeupWorker.notify_one();
		return task.id;
	}

	int ThreadPool::getThreadCount() {
		return workers.size();
	}

	void ThreadPool::runWorker(Worker* worker) {
		while (running) {
			bool needToWait = true;

			std::unique_lock<std::mutex> lock(mutex);
			if (!tasks.empty()) {
				Task task = tasks.front();
				int id = task.id;
				tasks.erase(tasks.begin());
				if (task.callback) {
					activeWorkerCount++;
					worker->activeTaskId = id;
					lock.unlock();
					task.callback();
					lock.lock();
					activeWorkerCount--;
					worker->activeTaskId = -1;
					taskFinished.notify_all();
					if (activeWorkerCount == 0 && tasks.size() == 0) {
						allTasksFinished.notify_all();
					}
					needToWait = false;
				}
			}

			if (needToWait) {
				wakeupWorker.wait(lock);
			}
		}
	}

}
