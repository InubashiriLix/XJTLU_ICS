#include "simple_swap.h"

#include <stdio.h>

#include <iostream>

void test1() {
    printf("test1\n");
    auto a = 10;
    auto b = 20;
    std::cout << "a: " << a << "\tb: " << b << std::endl;

    auto temp = a;
    a = b;
    b = temp;
    std::cout << "a: " << a << "\tb: " << b << std::endl;
}

void test2() {
    printf("test2\n");
    auto a = 10;
    auto b = 20;
    std::cout << "a: " << a << "\tb: " << b << std::endl;

    a = a - b;
    b = a + b;
    a = b - a;
    std::cout << "a: " << a << "\tb: " << b << std::endl;
}
