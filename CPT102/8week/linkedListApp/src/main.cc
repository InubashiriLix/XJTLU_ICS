#include <iostream>
#include <scheduler.hpp>

void test1_1() { std::cout << "test1_1\n" << std::endl; }
void test1_2() { std::cout << "test1_2\n" << std::endl; }
void test2_1() { std::cout << "test2_1\n" << std::endl; }
void test2_2() { std::cout << "test2_2\n" << std::endl; }
void test3_1() { std::cout << "test3_1\n" << std::endl; }
void test3_2() { std::cout << "test3_2\n" << std::endl; }
void test4_1() { std::cout << "test4_1\n" << std::endl; }
void test4_2() { std::cout << "test4_2\n" << std::endl; }

int main() {
    auto scheduler = Scheduler();

    auto task1_1 = Task(test1_1);
    scheduler.addTask(task1_1);
    auto task1_2 = Task(test1_2);
    scheduler.addTask(task1_2);
    auto task2_1 = Task(test2_1);
    scheduler.addTask(task2_1);
    auto task2_2 = Task(test2_2);
    scheduler.addTask(task2_2);
    auto task3_1 = Task(test3_1);
    scheduler.addTask(task3_1);
    auto task3_2 = Task(test3_2);
    scheduler.addTask(task3_2);
    auto task4_1 = Task(test4_1);
    scheduler.addTask(task4_1);
    auto task4_2 = Task(test4_2);
    scheduler.addTask(task4_2);

    // schduler.execute();
    scheduler.singleStepExecute();

    return 0;
}
