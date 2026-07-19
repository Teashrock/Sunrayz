#include "gui.h"

SzConstruct* CreateConstruct(SzConstruct* parent)
{
    SzConstruct* c = (SzConstruct*)MemAlloc(sizeof(SzConstruct));
    *c = (SzConstruct){
        .parent = parent,
        .child = NULL,
        .children = NULL,
        .parts = NULL
    };
    return c;
}