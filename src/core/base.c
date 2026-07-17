#include "base.h"

/// Checks if a string can be seamlessly converted into an integer.
/// Warning: expects a zstring.
bool StringIsInteger(char* string) {
    for (int i = 0; string[i] != '\0'; i++) {
        if (!(string[i] >= '0' && string[i] <= '9'))
            return false;
    }
    return true;
}

/// Checks if a string can be seamlessly converted into a boolean.
/// Warning: expects a zstring.
bool StringIsBool(char* string) {
    char* tokenTrue = "true";
    char* tokenFalse = "false";
    for (int i = 0; string[i] != '\0'; i++) {
        if (string[i] != tokenTrue[i] || string[i] != tokenFalse[i])
            return false;
    }
    return true;
}

SzConstruct* CreateConstruct(SzConstruct* parent)
{
    SzConstruct* c = (SzConstruct*)MemAlloc(sizeof(SzConstruct));

    *c = (SzConstruct){
        .parent = parent,
        .child = NULL,
        .parts = NULL
    };

    return c;
}
