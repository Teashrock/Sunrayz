#include "typing.h"

#include <stdlib.h>
#include <string.h>
#include <raylib.h>

/// Creates an instance of Sunrayz visible type.
SzType* CreateType(char* typeName)
{
    SzType* t = (SzType*)MemAlloc(sizeof(SzType));
    *t = (SzType){
        .entity = NULL,
        .type = (char*)MemAlloc((sizeof(char) * strlen(typeName)) + 1),
        .id = rand(),
    };
    t->type = typeName;
    return t;
}