#pragma once

#include <cstddef>
#include <vector>

/// we can use macro to escape the pain of writing the same code over and over again
/// the #field will be replace with the field's literal string name, instead of the field itself
#define REFLECT_FIELD(Type, field)    \
    FieldInfo {                       \
        #field, offsetof(Type, field) \
    } // macro func to generate a FieldInfo

#define DECLARE_FIELD(type, name) \
    type name; // use this to declare the field

struct FieldInfo
{
    const char* name;
    std::size_t offset;
};

struct TypeInfo
{
    const char*            name;
    std::vector<FieldInfo> fieldInfo;
};

/**
 * @brief get field's pointer through the pointer of object, field refers to the field's index at correcsponding TypeInfo
 * @note  the static_cast should be used to convert the pointer to the correct type
 *
 * @param `obj`:   the input object to get the field
 * @param `field`: the field to get
 */
void* get_field(void* obj, const FieldInfo& field);
