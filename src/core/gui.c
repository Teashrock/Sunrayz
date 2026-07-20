#include "gui.h"
#include "raylib.h"

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

SzEntity* CreateVisualEntity(SzConstruct* parent, Texture2D texture, SzEntityType type) {
    SzEntity* e = (SzEntity*)MemAlloc(sizeof(SzEntity));
    *e = (SzEntity){
        .parent = parent,
        .essence = NULL,
        .type = type,
        .next = NULL,
        .id = 0
    };
    Texture2D* ptr = (Texture2D*)MemAlloc(sizeof(Texture2D));
    *ptr = texture;
    e->essence = ptr;
    return e;
}

void AttachEntity(SzConstruct* construct, SzEntity* entity) {
    if (construct->parts == NULL) {
        construct->parts = entity;
    }
    SzEntity* ent = construct->parts;
    while (ent->next != NULL) {
        ent = ent->next;
    }
    ent->next = entity;
}