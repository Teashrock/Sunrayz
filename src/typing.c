#include "typing.h"

#include <stdlib.h>
#include <raylib.h>

/*int CreateSignal(SzVisible* object, char* name)
{
    
}*/

/// Creates an instance of Sunrayz visible type.
SzType* CreateType(char* typeName)
{
    SzType* t = (SzType*)MemAlloc(sizeof(SzType));
    *t = (SzType){0};
    t->entity = NULL;
    t->type = typeName;
    t->id = rand();

    return t;
}