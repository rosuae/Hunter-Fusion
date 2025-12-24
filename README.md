# Hunter Fusion

Hunter Fusion is a C++ platformer inspired by the Metroid series, featuring a structure based on interconnected rooms. The game utilizes the SFML library for rendering, input, and audio. Core elements include a simple engine with a tilemap and sprite-based rendering system, 2D physics (gravity, velocity, collisions), a dynamic camera that tracks the player, and modular room loading.

The gameplay focuses on controlling a character capable of walking, jumping, firing projectiles, and acquiring upgrades. The level structure is divided into rooms connected by doors, with progression initially restricted by the player's abilities. The game will feature various enemies with basic behaviors (patrolling, chasing, attacking), as well as boss battles utilizing scripted attacks. The collision detection is implemented using an Axis-Aligned Bounding Box (AABB) system.

The current objective of the game is to accumulate the highest possible Bounty by defeating enemies. Each enemy is assigned a bounty value based on its difficulty. This currency facilitates game progression; reaching specific bounty thresholds unlocks superior abilities (such as new suits, increased speed, or higher jump height). Additionally, the game features a Pet system, which will provide functional benefits to the player in future updates.
## Controls:
- `A, D` Move Left / Right.
- `S` Crouch.
- `S + A / D` Dodge / Morphball.
- `W / Space (Hold)` Jump.
- `Left / Right / Up Arrow` Shoot.
- `R` Reload.
- `Escape` Menu.

## Assets:
### Sprites:
- *Player:* 
  - https://www.spriters-resource.com/browser_games/supermariobroscrossover/asset/177645/
  - https://www.spriters-resource.com/browser_games/supermariobroscrossover/asset/177036/
- *Enemies:* https://www.spriters-resource.com/snes/smetroid/asset/1725/
- *Tiles:* https://www.spriters-resource.com/snes/smetroid/asset/144515/
- *More Sprites:* https://www.spriters-resource.com/snes/smetroid/
- *Sounds:*
  - https://sounds.spriters-resource.com/wii_u/nintendoland/asset/426887/
  - https://sounds.spriters-resource.com/snes/smetroid/asset/439917/
  - https://sounds.spriters-resource.com/wii_u/supermariomaker/asset/400349/


## To do:
 
- More enemies + Bosses
- Drops based on Player luck
- Option to change the game volume
- Observer system
- Better game state system
- Save and Load game feature

## Tema 3

#### Cerințe
- [ ] 2 șabloane de proiectare (design patterns)
- [ ] o clasă șablon cu sens; minim **2 instanțieri**
  - [ ] preferabil și o funcție șablon (template) cu sens; minim 2 instanțieri
- [ ] minim 85% din codul propriu să fie C++
<!-- - [ ] o specializare pe funcție/clasă șablon -->
- [ ] tag de `git` pe commit cu **toate bifele**: de exemplu `v0.3` sau `v1.0`
- [ ] code review #3 2 proiecte

## Known Bugs:

- Player can get stuck in tiles while performing actions like shooting while jumping (Rare)
- One frame drop spike when entering the game

## Resources
<!-- renovate: datasource=github-tags depName=SFML/SFML versioning=loose -->
- SFML3 Documentaion
  - https://www.sfml-dev.org/tutorials/3.0/
  - https://github.com/SFML/SFML/tree/3.0.2
- MAP CREATION
  - https://code.tutsplus.com/how-to-use-tile-bitmasking-to-auto-tile-your-level-layouts--cms-25673t
- COLLISION LOGIC
  - https://www.youtube.com/watch?v=l2iCYCLi6MU
  - https://manbeardgames.github.io/docs/tutorials/monogame-3-8/collision-detection/aabb-collision/

![metroid-samus-aran-vs-alien-4k-wallpaper-uhdpaper com-572@2@a](https://github.com/user-attachments/assets/b9edee4b-5e42-41d3-9d38-f47047061e10)