# Sunrayz Runtime

**Reader**s and **Task**s are the cornerstone of Sunrayz execution. There are two main Readers active in the runtime: executional and preparational.
- The executional Reader moves in real time, when the player interacts with the game, performing whatever the player wants.
- The preparational Reader moves several steps ahead from the executional one (and thus, from the player), and prepares in advance whatever the player will need to interact with soon. Be it background images, or character sprites, or music, or even complex game logic.

## Reader

A Reader is an autonomous entitity that contains a row of Tasks.
- Once created, an empty Reader starts reading the current execution script, parsing the commands in it. 