#include "scheduler.hpp"

#include <functional>
#include <iostream>

Task::Task(std::function<void()> task, Priority priority) : task_(task), pri(priority){};

void Task::execute() {
    if (!done) {
        task_();
        done = true;
    }
}

Priority Task::getPriority() const { return pri; }

Scheduler::Scheduler() {
    tasks[Priority::IRQ] = tasks_irq;
    tasks[Priority::FIRST] = tasks_first;
    tasks[Priority::SECOND] = tasks_second;
    tasks[Priority::THIRD] = tasks_third;
}

void Scheduler::addTask(Task task) {
    const Priority priority = task.getPriority();
    tasks[task.getPriority()].push_back(task);
}

void Scheduler::clear() {
    for (auto& task_level : tasks) task_level.second.clear();
}

void Scheduler::execute() {
    std::cout << "Executing tasks in order of priority:\n";
    for (auto& task_level : tasks) {
        for (auto& task : task_level.second) {
            task.execute();
        }
    }
}

void Scheduler::singleStepExecute() {
    std::cout << "Single Step Executing" << std::endl;
    // iterate the priority
    for (auto& task_level : tasks) {
        if (!task_level.second.empty()) {
            auto& task = task_level.second.front();
            task.execute();
            task_level.second.erase(task_level.second.begin());
            break;
        }
    }
}
