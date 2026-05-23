# Sunrayz Runtime

The main runtime routine of Sunrayz is held by three pillars: **Reader**, **Task**, and **Actor**.

## Task

A Task is a certain instruction or a set of instructions that is present in the dynamic memory of Sunrayz. Multiple Tasks are attached to each other in a linked list, forming a sequence of Tasks to perform.
- Usually, Tasks are performed by Actors, which move upon certain player actions. When the player advances on, reading through the plot or doing anything else Task-related, they trigger the Task to activate the piece of code it contains, allowing the player to jump to the next Task. And so on.

## Reader

A Reader is an autonomous, asynchronous entitity that traverses a game execution script and ensures on-demand loading of commands and resources.
- Once created, an empty Reader starts reading the current execution script, parsing the commands in it.
- Upon encountering a piece of code, a Reader evaluates it and decides where to put it: directly to Lua interpreter or to a new Task of Actor's queue. The decision depends on whether the code is dependent on another code, explicitly or implicitly.
- Reader has two "eyes" to traverse a script, that read it in two different places simultaneously. These eyes are equidistant from the player's current position, which is presented by Actor.
