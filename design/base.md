# The Basic Entity

Every visual thing in Sunrayz consists of two basic things: **SzConstruct** and **SzEntity**.

## SzConstruct

SzConstruct is a holder of many SzEntities. It may be created and used by itself, and is able to contain SzEntities and other SzConstructs.

## SzEntity

SzEntity represents anything that explicitly interacts with the user. It can be any visual element, any sound effect, or anything else like that. SzEntity by itself is bare, but it's flexible enough to grow any functionality for certain purposes.

## Example tree

```
SzConstruct
        ├───SzEntity
        ├───SzEntity
        ├───SzEntity
        ├───...
        ├────SzConstruct
        │           ├───SzEntity
        │           ├───SzEntity
        │           ├───SzEntity
        │           ┖───...
        ├────SzConstruct
        │           ├───SzEntity
        │           ├───SzEntity
        │           ├───SzEntity
        │           ┖───...
        ...
```