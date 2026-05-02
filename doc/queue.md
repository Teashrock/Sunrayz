# Queue-Task System

**Queue**s and **Task**s are the cornerstone of Sunrayz execution. There are two main queues active in the runtime: executional and preparational.
- The executional Queue moves in real time, when the player interacts with the game, performing whatever the player wants.
- Preparational Queue moves several steps ahead from the executional one (and thus, from the player), and prepares in advance whatever the player will need to interact with soon. Be it background images, or character sprites, or music, or even complex game logic.

## Queue

