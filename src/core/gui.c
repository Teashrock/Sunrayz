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

SzEntity* CreateEntity(SzConstruct* parent, SzEntityType type) {
    SzEntity* e = (SzEntity*)MemAlloc(sizeof(SzEntity));
    *e = (SzEntity){
        .parent = parent,
        .essence = NULL,
        .type = type,
        .next = NULL,
        .id = 0
    };
    return e;
}