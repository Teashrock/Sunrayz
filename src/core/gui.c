#include "gui.h"

#include "raylib.h"
#include "runtime.h"

SzConstruct* CreateConstruct(SzConstruct* parent) {
    SzConstruct* c = (SzConstruct*)MemAlloc(sizeof(SzConstruct));
    *c = (SzConstruct){
        .parent = parent,
        .child = NULL,
        .children = NULL,
        .parts = NULL
    };
    return c;
}

SzEntity* CreateVisualEntity(SzConstruct* parent, Texture2D texture, Rectangle sizeRect) {
    SzEntity* e = (SzEntity*)MemAlloc(sizeof(SzEntity));
    *e = (SzEntity){
        .parent = parent,
        .essence = NULL,
        .type = ENTITY_TYPE_TEXTURE,
        .next = NULL,
        .id = 0
    };
    Texture2D* ptr = (Texture2D*)MemAlloc(sizeof(Texture2D));
    *ptr = texture;
    e->essence = ptr;
    AddVariable(
        &e->params,
        CreateIntVariable("width", sizeRect.width, VAR_CLASS_SUNRAYZ)
    );
    AddVariable(
        &e->params,
        CreateIntVariable("height", sizeRect.height, VAR_CLASS_SUNRAYZ)
    );
    AddVariable(
        &e->params,
        CreateIntVariable("x", sizeRect.x, VAR_CLASS_SUNRAYZ)
    );
    AddVariable(
        &e->params,
        CreateIntVariable("y", sizeRect.y, VAR_CLASS_SUNRAYZ)
    );
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