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
        .id = (int*)MemAlloc(sizeof(int)),
    };
    t->type = typeName;
    t->id = rand();
    return t;
}