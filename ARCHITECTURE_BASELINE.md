# Architecture Baseline

**Date:** 2026-08-24  
**Status:** Baseline updated after extraction step 1.

## Purpose

This document records the current architectural reading of the project and defines the initial boundary for separating the reusable engine from Ant Hero-specific gameplay. It is the reference point for future refactoring decisions.

## Current State

The project is currently one Visual Studio executable. There is no separate engine library or formal module boundary. The separation below is based on responsibility and dependency direction, not on existing folders.

The intended rule is:

> The engine knows technology and reusable mechanisms. The game knows rules, content, characters, progression, and the Ant Hero world.

The game may depend on the engine. The engine must not depend on game concepts such as `Hero`, enemy names, essence, loot, Ant Hero maps, or concrete menus.

## Engine Candidates

These components are candidates for the reusable engine, subject to dependency cleanup:

- `Entity`: position, movement, collision, and base lifecycle.
- Part of `LivingEntity`: health, damage, hitboxes, and invincibility.
- `Animation`, `AnimationSet`, `Frame`.
- `Group`, `GroupBox`, `GroupNumber`, `GroupPosition`, `GroupString`, and `GroupBuilder` for frame metadata.
- SDL rendering utilities in `drawing_functions`.
- SDL resource cleanup in `cleanup`.
- `TimeController`.
- `RandomNumber`.
- Resource path resolution in `res_path`.
- `SoundManager`, after game-specific sound names and paths are removed.
- The generic portion of `CameraController`.
- The base `Ui`, `Menu`, and possibly `PaginatedMenu` abstractions.
- Future `InputSystem`, `InputCommand`, `EntityRegistry`/`World`, and generic `MapLoader` abstractions.

## Completed Step 1

Loot and essence were removed from `Entity`.

- `LootDropSource` now owns pending item drops, drop tables, and essence-drop state.
- `EnemyEntity` and `RoundKing` implement the gameplay loot contract.
- `Game` consumes drops through `LootDropSource` instead of reading public fields from `Entity`.
- The existing enemy death flow and item spawning sequence were preserved.

The engine candidate `Entity` no longer contains `dropItem`, `checkIfDropsItem`, drop flags, drop tables, or essence. The new `LootDropSource` remains in the game-facing entities area because loot and essence are gameplay concepts.

### Important qualification

These classes are not currently clean engine code. Several use global state, SDL directly, fixed resource paths, or game rules. They should be extracted only after those dependencies are made explicit or moved to the game layer.

## Game-Specific Components

These components represent Ant Hero rules or content and should remain in the game layer:

### Application and composition

- `Game` in `inc/game.h` and `src/game.cpp`.
- `main.cpp` as the application composition root.

`Game` currently owns map loading, entity creation, spawning, map transitions, loot, checkpoints, bosses, save/load, HUD, menus, and the SDL game loop. It is the main coupling point and should eventually become a smaller composition layer.

### Actors and combat

- `Hero`.
- `Glob`.
- `Grob`.
- `TermiteMiner`.
- `SmallBrownSpider`.
- `RoundKing`.
- `Bullet`.
- `StoneProjectile`.
- `EnemyEntity`, because its current behavior is tied to targets and pheromone trails.

These classes use engine mechanisms such as movement, animation, collision, and timing, but their states, attacks, sounds, drops, animation names, and AI are game rules.

### Items and progression

- `Item` and the concrete item classes.
- `ItemsHub`.
- `Door`.
- `Checkpoint`.
- `Bloodstain`.
- `Map` in its current form.
- `SaveHandler` in its current form.

Although some of these concepts could eventually have generic infrastructure, their current schemas and behavior encode Ant Hero content, inventory, essence, doors, defeated bosses, map files, and save-game progression.

### NPCs and UI

- `FriendlyNpc`.
- The gameplay-facing parts of `NpcData` and `NpcFactory`.
- `HPBar`.
- `QuickItemUi`.
- `ItemPickMessageUi`.
- `EssenceCounterUi`.
- `ActionMessageUi`.
- `MainMenu`.
- `PauseMenu`.
- `SubMenu`.

These components directly know the hero, inventory, essence, dialogue, shops, quests, or game actions.

## Hybrid Components

These components need an explicit design decision or incremental split:

- `Entity`: reusable movement/collision core mixed with animation, globals, entity registry, loot, and essence.
- `LivingEntity`: reusable health/damage behavior mixed with pheromone trails, enemy IDs, and game save behavior.
- `CameraController`: reusable camera idea mixed with `Globals` and fixed `1024 x 1024` limits.
- `Wall`: potentially a generic solid obstacle, though currently an SDL/game entity.
- `Ui`, `Menu`, and `PaginatedMenu`: generic visual/navigation base with SDL and game-specific subclasses.
- `NpcData`: data-transfer structure that could belong to a generic content/data layer, while its current schema is game-specific.
- `NpcFactory`: content loading infrastructure currently creating a concrete gameplay NPC.
- `SaveHandler`: JSON persistence mechanism mixed with an entirely game-specific save schema.
- `Globals`: rendering and camera infrastructure mixed with pause/debug state.

## Critical Dependency Problems

The current dependency direction contains engine-to-game leakage:

- `Entity` contains loot, essence, and a global list of all entities.
- `KeyboardInput` and `JoystickInput` include and store `Hero*` and call hero actions directly.
- Concrete UI reads fields directly from `Hero`.
- `SaveHandler` includes `item/itemsHub.h`.
- `CameraController` writes to or depends on `Globals` and includes `game.h` unnecessarily in its implementation.
- `AnimationSet` uses fixed asset directories and global renderer state.
- `Game` manipulates Tileson directly, including layer names and map properties.
- Entities call the global `SoundManager` directly.
- Most systems depend on `Globals::renderer`, `Globals::camera`, and `TimeController::timeController`.

The main global-state obstacles are:

- `Globals::renderer`.
- `Globals::camera`.
- `Globals::pause`.
- `Entity::entities`.
- `TimeController::timeController`.
- `SoundManager::soundManager`.
- Static defeated-enemy/boss counters.

## Initial Target Structure

```text
engine/
  core/
    Entity
    Collision
    TimeController
    RandomNumber
  rendering/
    Frame
    Animation
    AnimationSet
    Renderer
  audio/
    SoundManager
  input/
    InputSystem
    InputCommand
  world/
    CameraController
    EntityRegistry
  data/
    ResourcePath
    JsonLoader
    FrameData

game/
  actors/
  items/
  npcs/
  world/
  maps/
  ui/
  persistence/
  rules/

app/
  main.cpp
  Game
  SDL initialization
```

This is a target direction, not a required immediate folder move.

## Recommended Extraction Order

1. Remove loot, essence, and game-specific drops from `Entity`.
2. Replace renderer, camera, and time singletons with explicit dependencies or a controlled context.
3. Extract `Entity::entities` into an `EntityRegistry` or `World` owned by the game/session.
4. Make keyboard and joystick input produce commands instead of referencing `Hero`.
5. Separate logical updates from rendering where practical.
6. Encapsulate Tileson and map-layer details behind a game-facing `TiledMapLoader`.
7. Make resource paths and audio identifiers configurable rather than hardcoded.
8. Separate generic JSON/file persistence from the game-specific save schema.
9. Reduce `Game` to orchestration of systems, scenes, and game states.
10. Create a separate engine library/project in the solution only after the dependency direction is stable.

## Baseline Constraints

- Preserve current gameplay behavior during extraction.
- Prefer small, compilable steps over a simultaneous rewrite.
- Do not move a class into the engine solely because its name sounds generic.
- Every engine header should be checked for direct dependencies on game headers, game identifiers, fixed game assets, and global game state.
- The project currently targets C++17 and uses SDL2, SDL_image, SDL_mixer, SDL_ttf, Tileson, and `nlohmann::json`.

## Definition of a Successful Boundary

The first meaningful milestone is not a new folder layout. It is a one-way dependency boundary in which:

```text
application/game -> engine
engine           -X-> game
```

The engine should be buildable and testable without including `hero.h`, concrete enemy headers, item headers, Ant Hero map content, or game-specific save data.
