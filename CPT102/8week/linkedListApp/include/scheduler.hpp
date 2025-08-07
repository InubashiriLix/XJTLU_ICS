#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <functional>
#include <iostream>
#include <map>
#include <vector>

enum class Priority { IRQ = 1, FIRST = 2, SECOND = 3, THIRD = 4 };

class Task {
   public:
    Task(std::function<void()> task, Priority priority = Priority::IRQ);

    void execute();
    Priority getPriority() const;

   private:
    std::function<void()> task_;
    Priority pri;
    bool done = false;
};

class Scheduler {
   public:
    Scheduler();
    void addTask(Task task);
    void execute();
    void clear();
    void singleStepExecute();

   private:
    std::map<Priority, std::vector<Task>> tasks;
    std::vector<Task> tasks_irq;
    std::vector<Task> tasks_first;
    std::vector<Task> tasks_second;
    std::vector<Task> tasks_third;
};

#endif
