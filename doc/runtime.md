# Sunrayz Runtime

The main runtime routine of Sunrayz is held by three pillars: ****Reader****, ****Task****, and ****Actor****.

## **Task**

A **Task** is a certain instruction or a set of instructions that is present in the dynamic memory of Sunrayz. Multiple **Task**s are attached to each other in a linked list, forming a sequence of **Task**s to perform.
- Usually, **Task**s are performed by **Actor**s, which move upon certain player actions. When the player advances on, reading through the plot or doing anything else **Task**-related, they trigger the **Task** to activate the piece of code it contains, allowing the player to jump to the next **Task**. And so on.
- Multiple **Task** queues can be present in the engine memory at the same time. They can be accessed by **Actor**, who stores their memory addresses in its memory.

## **Reader**

A **Reader** is an autonomous, asynchronous entity that traverses a game execution script and ensures on-demand loading of commands and resources.
- Once created, an empty **Reader** starts reading the current execution script, parsing the commands in it.
- Upon encountering a piece of code, a **Reader** evaluates it and decides where to put it: directly to Lua interpreter or to a new **Task** of **Actor**'s queue. The decision depends on whether the code is dependent on another code, explicitly or implicitly.
- **Reader** has multiple "eyes" to traverse script files, that read it in various different places simultaneously. These eyes read their scripts until a certain distance is reached from the player's current position, which is presented by **Actor**. Once the player's (and thus, **Actor**'s) position changes, **Reader**'s eyes move as well, reading and evaluating the code.

## **Actor**

**Actor** is an entity that takes a position on its **Task** queue, awaiting for the player's certain actions. It represents the current state of runtime and keeps the dynamically changing game data.
- One of **Actor**'s kept data is a list of trigger events. Once the player triggers an event, **Actor** checks if the triggered event is present in the list, and if it does, **Actor** picks from it the memory address of a **Task** to move towards.
- Upon picking a **Task** memory address, **Actor** performs a movement by it, triggering **Reader**'s movement, and accessing the code kept in the **Task**. The code is then immediately sent to Lua interpreter, and **Actor** stands by until the player triggers another event.
