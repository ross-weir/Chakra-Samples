#pragma once

template<class T>
class TaskQueue {
	void push(T&& task) {
		tasks.push_back(std::forward<T>task);
	}

	T pop() {
		T task = tasks.front();
		tasks.pop_front();

		return task;
	}
private:
	std::deque<T> tasks;
};