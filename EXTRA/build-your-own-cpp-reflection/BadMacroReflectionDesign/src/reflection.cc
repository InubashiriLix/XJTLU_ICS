#include "reflection.h"

void* get_field(void* obj, const FieldInfo& field) {
    return static_cast<char*>(obj) + field.offset;
}
