#include <iostream>
#include "reflection.h"

struct FooType
{
    char* name;
    int   age;
    FooType(char* name, int age) : name(name), age(age) {}
    void bark() {
        std::cout << "I'm " << name << ", and I'm " << age << " years old." << std::endl;
    }
};

TypeInfo FooTypeInfo = {
    .name      = "FooType",
    .fieldInfo = {
        REFLECT_FIELD(FooType, name),
        REFLECT_FIELD(FooType, age),
    }};

int main() {
    std::cout << "Hello, World!" << std::endl;
    FooType foo((char*)"Foo", 10);
    foo.bark();
    auto name = get_field(&foo, FooTypeInfo.fieldInfo[0]);
    auto age  = get_field(&foo, FooTypeInfo.fieldInfo[1]);

    std::cout << *static_cast<char*>(name) << std::endl; // output "o"
    std::cout << *static_cast<int*>(age) << std::endl;   // output 10
}
