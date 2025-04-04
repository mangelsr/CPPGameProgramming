# Assignment 3

## Program Specification

In this assignment you will be writing the game that was presented in class.  
This game must have the following features:  

### Assets  

- Entities in the game will be rendered using various **Textures** and **Animations**, which we will be calling **Assets** (along with **Fonts** and **Sounds**).  
- Assets are loaded once at the beginning of the program and stored in the `Assets` class, which is managed by the `GameEngine` class.  
- All Assets are defined in `assets.txt`, with the syntax defined below.  

### Player

- The player Entity in the game is represented by NotLink, which has several different Animations: RunDown, RunUp, RunRight, StandDown, StandUp, StandRight, AtkUp, AtkDown, AtkRight. You must determine which direction and state the player is currently in and assign the correct Animation.
- The player is assigned the following animations in the direction facing. Please note that Left will be accomplished by mirroring Right (x scale -1) StandDir - When no input (or both opposite) is being given to the player RunDir - When move input is given to the player in a direction AttackDir - When the player's sword is currently visible from an attack
- The player moves with the following controls:
  Left: A key, Right: D key, Up: W Key, Down: S Key, Attack: Space Key
- The player can move up, left, down, right, at any time during the game
- The player can only move either horizontally or vertically at a given time
- If opposite directions are held, the player will be given a Stand anim
- The player can only attack once its current attack animation has finished
- The player collides with 'Tile' entities in the level (see level syntax) whose bounding box blocks movement and cannot move through them.
- The player will be given a CBoundingBox of a size specified in the level file  
- The player will be given a CHealth component with a max health that is specified in the level file  
- When the player's current health is <= 0, the player respawns, and the LinkDie sound should play  
- If the player has a CInvincibility component:  
  - Takes no damage from enemies  
  - Sprite is made transparent  
  - Duration is reduced each frame  
  - When duration reaches 0, the component is removed (player becomes vulnerable again)  
- When the player collides with an enemy while not invincible:  
  - Player's current health is reduced by the enemy damage component  
  - The LinkHit sound should play  
  - Player is given a CInvincibility component lasting 30 frames  

### Other Entities

- Each tile and NPC in the game specifies two boolean values: whether it blocks movement and whether it blocks vision.
  - If a tile blocks movement, the player collides with it and cannot pass through.
  - If an entity blocks vision, then it affects enemy line of sight. (See spec below.)
- CBoundingBox has two new booleans to reflect these changes.
- When the player steps onto a tile with the "Black" animation, they are teleported to a random "Black" tile on the map.

### Attacking

- When the player attacks:
  - A sword appears for 10 frames and then disappears, positioned approximately 1 tile away from the player in the direction they are facing.
  - The player's sword should be given a bounding box equal to the animation size.
- Sword collision and damage:
  - When the sword collides with an enemy, it deals damage equal to its damage component to the enemy's health.
  - The sword should only deal damage for one frame, after which its damage component is removed to prevent multiple damage instances from overlapping frames.
- Sound effects:
  - When the sword is swung, the "Slash" sound should play.
  - When the sword collides with an enemy, the "EnemyHit" sound should play.
  - When an enemy's health is reduced to 0, it is destroyed, and the "EnemyDie" sound should play.

### NPC Entities

- NPC entities will each be given an 'ai' behavior in the level file, either:
  - 'Follow':
    - Follows the player when in direct line of sight
    - Returns to its original position when not in line of sight
    - Does not oscillate when at its home position
  - 'Patrol':
    - Moves toward a series of patrol positions
    - Loops back to the start when reaching the final position
    - Considered to have reached a patrol position if within distance of 5 from it

### Tiles

- Tiles are Entities that define the level geometry and interact with players
- Tiles can be given any Animation that is defined in the Assets file
- Tiles will be given a CBoundingBox equal to the size of the animation:  
  `tile->getComponent<CAnimation>().animation.getSize()`
- The current animation displayed for a tile can be retrieved with:  
  `tile->getComponent<CAnimation>().animation.getName()`
- Special behavior for Heart animation:
  - Any NPC that collides with a Heart tile has its health filled to maximum
  - The Heart tile is destroyed after collision

### Drawing

- The game window is given a fixed size of 1280x768 pixels (20x12 tiles)
- Rendering of entities is provided for you, as well as a 'debug' rendering mode which can be toggled with the 'F' key, and the 'R' key toggling texture rendering.
- You are required to change the camera position (window.view()) of the game window to alternate between two modes based on the m_follow variable:
  true: Follow Mode - the camera is always centered on NotLink
  false: Room Mode - the camera is centered on the middle of the room

### Rooms

- Each level is split up into individual 'rooms' given an (x,y) coordinate
- Room (x,y) is defined by the rectangle (x*w, y*h, w, h) where w and h are the window width and height respectively.
- When NotLink moves to a new room, the camera should center to that room

### Custom Level

- Include your own custom level as `level2.txt` which contains at least 2 rooms worth of interesting gameplay

### Misc

- The 'P' key should pause the game
- The 'F' key toggles wireframe / debug rendering
- The 'R' key toggles texture rendering
- The 'Y' key should toggle between follow camera or 'room' camera
- The 'ESC' key should go 'back' to the Main Menu, or quit if on the Main Menu

## Config Files

There will be two configuration files in this assignment. The Assets config file, and the Level configuration file

### Assets File Specification

There will be three different line types in the Assets file, each of which correspond to a different type of Asset. They are as follows:

#### Texture Asset Specification

Texture N P

| Attribute         | Abbreviation | Type          | Notes                  |
|-------------------|--------------|---------------|------------------------|
| Texture Name      | N            | `std::string` | It will have no spaces |
| Texture File Path | P            | `std::string` | It will have no spaces |

#### Animation Asset Specification

Animation N T F S

| Attribute      | Abbreviation | Type          | Notes                                     |
|----------------|--------------|---------------|-------------------------------------------|
| Animation Name | N            | `std::string` | It will have no spaces                    |
| Texture Name   | T            | `std::string` | Refers to an existing texture             |
| Frame Count    | F            | `int`         | Number of frames in the animation         |
| Anim Speed     | S            | `int`         | Number of game frames between anim frames |

#### Font Asset Specification

Font N P

| Attribute      | Abbreviation | Type          | Notes                  |
|----------------|--------------|---------------|------------------------|
| Font Name      | N            | `std::string` | It will have no spaces |
| Font File Path | P            | `std::string` | It will have no spaces |

#### Sound Asset Specification

Sound N P

| Attribute       | Abbreviation | Type          | Notes                  |
|-----------------|--------------|---------------|------------------------|
| Sound Name      | N            | `std::string` | It will have no spaces |
| Sound File Path | P            | `std::string` | It will have no spaces |

### Level Specification File

#### Player Specification

Player X Y BX BY S H I

| Attribute         | Abbreviation | Type     | Notes                      |
|-------------------|--------------|--------- |----------------------------|
| Spawn Position    | X Y          | int, int | Coordinates on the map     |
| Bounding Box Size | BX BY        | int, int | Width and height in pixels |
| Speed             | S            | float    | Movement speed             |
| Max Health        | H            | int      | Initial health points      |

#### Tile Specification

Tile Name RX RY TX TY BM BV

| Attribute       | Abbreviation | Type     | Notes                        |
|---------------- |--------------|----------|------------------------------|
| Animation Name  | Name         | string   | Reference to animation asset |
| Room Coordinate | RX RY        | int, int | Room position in grid        |
| Tile Position   | TX TY        | int, int | Position within room         |
| Blocks Movement | BM           | int      | 1 = true, 0 = false          |
| Blocks Vision   | BV           | int      | 1 = true, 0 = false          |

#### NPC Specification

NPC Name RX RY TX TY BM BV H D AI ...

| Attribute        | Abbreviation | Type     | Notes                        |
|------------------|--------------|----------|------------------------------|
| Animation Name   | Name         | string   | Reference to animation asset |
| Room Coordinate  | RX RY        | int, int | Room position in grid        |
| Tile Position    | TX TY        | int, int | Position within room         |
| Blocks Movement  | BM           | int      | 1 = true, 0 = false          |
| Blocks Vision    | BV           | int      | 1 = true, 0 = false          |
| Max Health       | H            | int      | Must be > 0                  |
| Damage           | D            | int      | Must be > 0                  |
| AI Behavior Name | AI           | string   | Follow or Patrol             |

#### AI Follow Specification

AI = Follow
... = S

| Attribute    | Abbreviation | Type  | Notes                  |
|--------------|--------------|-------|------------------------|
| Follow Speed | S            | float | Speed to follow player |

#### AI Patrol Specification

AI = Patrol  
... = S N X1 Y1 X2 Y2 ... XN YN  

| Attribute           | Abbreviation | Type     | Notes                                 |
|---------------------|--------------|----------|---------------------------------------|
| Patrol Speed        | S            | float    | Movement speed between positions      |
| Number of Positions | N            | int      | Total patrol points in path           |
| Patrol Position i   | Xi Yi        | int, int | Tile coordinates for position i (1-N) |

#### For Example

NPC Tektite 0 0 15 10 0 0 2 1 Patrol 2 4 15 10 15 7 17 7 17 10

- Spawn an NPC with animation name "Tektite" in room (0,0) with tile position (15,10)
- This NPC has max health of 2 and damage of 1
- This NPC does not block movement (BM=0) or vision (BV=0)
- This NPC has a Patrol AI with:
  - Speed: 2
  - 4 patrol positions in room (0,0):
    (15,10) → (15,7) → (17,7) → (17,10)

## Assignment Hints

You can use all of your collision code from Assignment 3 in this assignment.

I recommend approaching this assignment in the following order:

1. Implement WASD movement for the player such that it moves up/left/down/right when those keys are pressed. This will allow you to test rectangular collisions as soon as possible.
2. Implement `GameState_Play::loadLevel()`, which will allow you to load the assets for the level given to you to test the rest of the assignment.
3. Implement `spawnPlayer()` so that it correctly uses the configuration specified by the level file so you can start testing collisions.
4. Implement the camera changes described above.
5. Implement `spawnSword()` so that it spawns a sword in the correct position with the correct lifespan.
6. Implement player attacking / sword collision with NPCs / health / damage systems.
7. Implement player animations to correctly animate based on player state.
8. Implement NPC Patrol behavior.
9. Implement `Physics::lineIntersect()` which will allow you to test whether or not two line segments AB and CD intersect, and where they intersect.
10. Implement `Physics::EntityIntersect()` which will allow you to test whether line segment AB intersects with any lines of the bounding box of `Entity e`.
11. Implement NPC Follow Behavior:
    - First without vision checks
    - Then using EntityIntersect() to check line of sight
12. Implement invincibility frames.
13. Implement heart pickups.
14. Implement black tile teleporting.
