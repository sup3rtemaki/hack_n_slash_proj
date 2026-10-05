# Architecture Baseline

**Date:** 2026-08-27
**Status:** Baseline updated after completion of ownership cleanup and final dependency audit.

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

## Progress On Step 2

Entity updates now receive the current frame delta through `Entity::deltaTime`, assigned by `Game` before each entity update. The entity hierarchy no longer reads `TimeController::timeController` directly.

This is an incremental step. `Game` now owns its `TimeController`; there is no global time-controller instance.

`CameraController` is now also supplied its frame delta by `Game` and no longer includes or reads `TimeController` directly. The timed message UIs now receive their frame delta through `Ui`. The application-level clock is owned by `Game`.

### Important qualification

These classes are not currently clean engine code. Several use global state, SDL directly, fixed resource paths, or game rules. They should be extracted only after those dependencies are made explicit or moved to the game layer.

## Completed Step 3: Move the global entity registry to the game/session ownership

The next refactoring target is the global registry inside `Entity`:

- `Entity::entities` is a static list shared across the whole program.
- Many gameplay systems depend on it for collision checks, spawning, removal, and map cleanup.
- This makes the engine candidate depend on a global runtime state that belongs to the application/game session.

### Goal

Make the entity collection a game-owned runtime resource, while preserving today’s behavior and keeping the refactor incremental.

### Proposed boundary

- The engine owns reusable entity behavior: position, movement, collision primitives, animation data, and lifecycle hooks.
- The game/session owns the actual registry of active entities for the current map or current scene.
- The registry should be passed or injected where needed instead of being accessed through a process-wide static global.

### Safe migration plan

1. Introduce an `EntityRegistry` or `World` container owned by `Game`.
2. Keep a compatibility layer during the transition so existing code still compiles while the registry moves.
3. Update collision and spawn flows to read from the registry owned by the active session, not the static global.
4. Remove direct static access from engine code when each subsystem is migrated.
5. Keep all map changes, entity activation, and cleanup logic in the same game layer that already owns the current map.

### Design rules for this step

- `Entity` should not directly own the current active-world list.
- Game or session code should decide which entities are active in the current map.
- Collision broad-phase checks should query the active registry of the current world context.
- The engine should expose reusable utilities, but the runtime list should live in the game/application boundary.

This is the next dependency-reduction milestone after time and camera injection, and it should be done before attempting a larger engine/library split.

### Completed result

Step 3 is complete at the current incremental boundary:

- `Game` owns the runtime entity list through `Game::entities`.
- `Entity::getEntities()` exposes the active world list to gameplay systems instead of requiring direct iteration over a static process-wide registry.
- Entity creation and removal use the active-world API through `Entity::addEntity()` and `Entity::removeEntity()` where objects can be created outside `Game`.
- Targeting, damage, update, draw, spawn, cleanup, and the base collision broad-phase now all use `Entity::getEntities()` as their active source.
- The thrown stone projectile was verified in gameplay after being moved to the active registry path.
- The legacy `Entity::entities` mirror has been removed from the active boundary: the current world is represented by `Game::entities` and `Entity::activeWorld` points to it while the session is active; a minimal fallback registry remains as a safe default for creation paths that occur before or after a session is detached.

### Validation

The project was rebuilt manually in Visual Studio 2019 and the game launched successfully. Enemy spawning and the throwing stone projectile were tested in the running game.

The last direct engine-side read of the static `Entity::entities` mirror, inside the collision broad-phase in `Entity::updateCollisions()`, was replaced with `Entity::getEntities()`. This closes the remaining boundary gap for this step; the static list still exists only as the compatibility mirror synchronized by `Game`, and can be removed entirely in a future cleanup once nothing outside `Entity` needs the legacy static symbol.

## Completed Step 4: Make keyboard and joystick input produce commands

This is the next official extraction step after Step 3. `KeyboardInput` and `JoystickInput` currently include `Hero`, store a `Hero*`, and call hero actions directly. That makes input depend on a concrete game character instead of producing reusable input intent.

### Goal

Make keyboard and joystick adapters translate SDL events and device state into generic commands. `Game` or a gameplay controller should interpret those commands and decide how `Hero` responds.

### Safe migration plan

1. Define a small `InputCommand` representation for movement, attack, dash, item use, and interaction.
2. Make `KeyboardInput` emit commands without including or referencing `Hero`.
3. Make `JoystickInput` emit the same command representation.
4. Move command interpretation to `Game` or a game-facing input controller.
5. Preserve the current key bindings and action behavior while migrating one action at a time.

The existing direct `Hero*` fields in `KeyboardInput` and `JoystickInput` are the primary boundary to remove in this step.

### Progress on Step 4

The first input-command slice is now implemented:

- `InputCommand` represents movement, stop, attack, dash, item use, interaction, and quick-item selection.
- `KeyboardInput` and `JoystickInput` no longer include `Hero` or store a `Hero*`.
- Both adapters emit the same command representation while preserving the existing key/button bindings and movement angles.
- `Game::handleInputCommand()` is responsible for interpreting commands and invoking the game character's actions.
- The active movement source priority between keyboard and joystick remains preserved.

### Validation and known follow-up

The game was rebuilt and tested successfully after the input migration. Keyboard and gameplay input commands are working.

The joystick bindings still need a separate follow-up because the controller was changed during development. The current command migration covers the gameplay path, but joystick command mapping is not yet standardized across all menus and UI states. This is a known limitation for a future input-system pass, not a blocker for completing this extraction step.

Future input work should:

- make joystick button mappings configurable or device-independent;
- define menu navigation commands separately from hero gameplay commands;
- route menu and UI input through the same command boundary;
- validate controller behavior with the currently used device.
### Future singleton audit: move process-wide registries and factories into the session boundary

This is the next explicit-ownership slice after the entity world cleanup.

The remaining global/runtime state is not only renderer and camera data; there are also process-wide registries and factories with hidden lifetime management. `NpcFactory` is currently a singleton data cache for NPC definitions, but it behaves like a game-session asset registry rather than a reusable engine singleton.

### Safe migration plan

1. Make the NPC catalog a normal object owned by `Game` or a bootstrap subsystem.
2. Pass the factory instance to the code that loads or creates NPCs at runtime.
3. Keep the NPC content and spawn flow unchanged while removing the static singleton.
4. Re-audit any remaining static registry or service objects before beginning broader engine extraction.

### Progress on the singleton audit

- `NpcFactory` no longer uses a static singleton accessor.
- The factory is now owned by `Game` as a normal member object and is used through the current session instance.
- The gameplay behavior remains the same, but the NPC database lifetime is tied to the game session rather than the process.

## Final ownership cleanup sweep

This cleanup pass confirmed that the project-owned runtime state has been moved behind explicit session ownership boundaries.

### What was resolved

- Resource-path resolution is no longer cached in process-wide static state. The base path is recalculated when needed in `getResourcePath()`.
- UI resource roots are owned by each UI instance instead of a shared static path.
- Enemy kill counters are session-scoped through `GameKillStats` in the Game layer and are owned by `Game`.
- Entity world ownership remains attached to the active game session via `Game::entities` and `Entity::activeWorld`.
- Any remaining direct singleton-like runtime ownership in the project code has been reduced to explicit dependencies passed through constructors and game-session members.

### Remaining non-project state

The only non-const static members left in the codebase are in the third-party Tileson header at `inc/tileson/tileson.hpp`. Those are library internals, not game-owned runtime state, so they are outside the current architecture cleanup boundary.

### Verification status

The refactor was validated by:

- checking the remaining `static`/global runtime candidates in the project-owned code;
- confirming the resource-path, UI, and enemy-stat ownership changes compile cleanly in editor diagnostics;
- verifying that the remaining project runtime state is now expressed as session-owned dependencies rather than process-wide globals.

This marks the end of the ownership cleanup sweep for the Ant Hero game layer. The next stage, if desired, should focus on a broader engine/game separation rather than more session-lifetime cleanup.
## Future rendering slice: Replace renderer, camera, and time globals with explicit dependencies

This rendering slice should be addressed after the input-command migration, unless a small prerequisite is required by a concrete change.

### Progress on future rendering slice

The first rendering slice is complete:

- `RenderContext` now carries the SDL renderer, camera rectangle, and debug-rendering flag.
- `Entity::draw()` and `LivingEntity::draw()` receive the context explicitly.
- `Frame::Draw()` receives the context instead of reading `Globals::renderer` directly.
- `Game` builds the context from its current application state before drawing the entity world.
- `AnimationSet::loadAnimationSet()` now takes an explicit `SDL_Renderer*` parameter instead of reading `Globals::renderer` internally; every caller (all gameplay entities, items, NPCs, and projectiles that load an animation set) passes `Globals::renderer` explicitly at the call site.
- `CameraController::update()` now takes the camera rect and world bounds as explicit parameters instead of reading/writing `Globals::camera` or hardcoding map size.
- `Game` now owns the paused-state flag, and the pause flow no longer reads or writes `Globals::pause`.
- `ActionMessageUi`, `HPBar`, and `ItemPickMessageUi` now use their injected renderer directly instead of falling back to `Globals::renderer`.
- `QuickItemUi` and `EssenceCounterUi` now load their textures from `setRenderer()` after construction and use the injected renderer directly, avoiding null-renderer bootstrap loads.
- `PauseMenu` and `SubMenu` now receive the renderer explicitly; pause-menu textures are loaded after injection and all menu drawing uses that renderer directly.
- UI rendering no longer has a `Globals::renderer` fallback. Remaining renderer-global uses are in entity/resource construction and application bootstrap, which require a separate constructor-injection slice.
- `Hero` and `Bloodstain` now receive the renderer during construction and pass it to animation loading.
- Inventory items (`HoneydewPotion`, `GreenBerry`, `Stone`, and `Key`) now receive the renderer during construction; both save-game loading and map item spawning pass it from `Game`.
- `MapPopulationSystem` now owns an explicit renderer dependency and passes it to doors, checkpoints, `TermiteMiner`, and `SmallBrownSpider` during map population.
- `NpcFactory` and `FriendlyNpc` now receive the renderer explicitly when the bootstrap creates the blacksmith NPC.
- `StoneProjectile` and `Bullet` now receive the renderer explicitly. `Stone` stores the injected renderer for projectile creation, and `RoundKing` passes its renderer when firing bullets.
- `Glob` and `Grob` now receive the renderer explicitly for animation loading.
- The application bootstrap no longer stores the SDL renderer in `Globals`: `main.cpp` owns the renderer lifetime and passes it to `Game`, which initializes `RenderContext` from that explicit dependency.
- `TimeController` is owned by `Game`; the audit found no remaining time singleton access.
- `SoundManager` is now owned by `Game` and passed through entity/system dependencies; the static `SoundManager::soundManager` singleton was removed.
- Fixed display dimensions and scale now live in `DisplayConfig`; `Globals` no longer owns screen dimensions or the debug flag. `RenderContext` keeps the explicit debug value used by rendering.

This keeps the change incremental: entity, frame, animation-loading, and camera rendering now have an explicit dependency boundary, while UI rendering remains compatible with the existing global state for now.

The next boundary target is the remaining global runtime context that is still being reached directly from engine-side code:

- time-controller singleton access, if any remains
- sound-manager singleton access
- application-wide screen and debug configuration
- `TimeController::timeController`-style patterns (already reduced, but remaining uses should be audited)
- implicit access to application-wide SDL state from `Entity`, `AnimationSet`, and rendering helpers

### Why this is the next step

Even after the registry move, the engine still leans on global SDL state for drawing and camera placement. That means the reusable logic is not yet independently testable or portable, and it still depends on a process-wide application context.

### Target direction

- `Game` or a `GameContext` should own SDL renderer and camera state.
- The engine should receive renderer/camera/time via constructor injection or a lightweight context object.
- Draw helpers should not require `Globals::renderer` directly when their callers are already in the game scene.
- `Entity` and `AnimationSet` should prefer explicit draw parameters or a renderer abstraction instead of reading hidden global state.

### Safe migration plan

1. Identify every engine class that reads `Globals::renderer` or `Globals::camera` directly.
2. Create a minimal runtime context object used by rendering and camera logic.
3. Inject the context at construction or update time from `Game`.
4. Keep a compatibility layer temporarily to avoid broad churn while the migration is in progress.
5. Remove the remaining global access only after each subsystem is numerically verified to still render and update correctly.

### Next refactoring sequence

1. Complete renderer bootstrap cleanup and verify the full build and runtime flow.
2. Audit and remove remaining time-controller singleton access.
3. Replace the global sound-manager singleton with an explicit sound service dependency.
4. Decide which screen/debug values belong in the application context instead of `Globals`.
5. Remove the transitional static entity-registry mirror after confirming no consumers remain.
6. Move remaining Tileson layer/property interpretation behind a game-facing map service.
7. Narrow and extract the interaction checks after reducing their `Game` and UI dependencies (Completed via `InteractionSystem`).
8. Organize files and directory boundaries for Engine vs Game modules in preparation for project separation.
9. Create a separate engine project only after the dependency direction is stable.

This is the point where the project becomes much closer to a true engine/game split: the engine will still handle drawing and motion primitives, but no longer depend on hidden global application state to do so.

## Completed Step 5: Separate logical updates from rendering

The orchestration and map-phase separation of Step 5 are complete at the current boundary:

- `Game::updateEntities()` now owns the logical update pass and gameplay interaction checks.
- `Game::drawEntities()` now owns entity sorting and the rendering pass.
- `Game::updateMaps()` now handles map transition and fade state without rendering tiles.
- `Game::drawMap()` now owns tile and fade rendering.
- `runMainGame()` coordinates the two passes without mixing their internal responsibilities.
- The existing entity update order and post-update gameplay checks were preserved.

This step is intentionally incremental. Entity classes still contain both `update()` and `draw()` methods, but the game loop no longer embeds both responsibilities in one block. Further decomposition of scene and system responsibilities remains optional future work.

### Validation

The project was rebuilt in Visual Studio 2019 and the game was tested successfully after the map-phase extraction. The game loop continues to update and render the map, entities, UI, and transitions correctly.

### Scope clarification

The `RenderContext` work was started early as part of the incorrectly selected rendering slice. It supports this step by making entity drawing more explicit, but it is tracked separately from the official extraction order.

## Completed Step 6: Encapsulate Tileson and map-layer details

The first loader slice is implemented:

- `TiledMapLoader` now owns the Tileson parser instance and the parse operation.
- `Game::loadTiledMap()` delegates map loading to `TiledMapLoader`.
- The global `tson::Tileson` parser instance was removed from `game.cpp`.
- The `tson::Map` remains owned by the current game session while existing layer consumers are migrated incrementally.
- `Game` still interprets Ant Hero-specific layer names, properties, spawning, and rendering, as expected for the game-facing portion of this step.

### Validation

The project was rebuilt in Visual Studio 2019 and the game was tested successfully after introducing `TiledMapLoader`.

Repeated layer and property access can be moved behind game-facing map services in a future refinement, without hiding Ant Hero rules inside a generic engine loader.

## Completed Step 9: Extract map state and persistence logic from the game session

The current slice is complete at the session boundary:

- `MapStateSystem` owns the map item-pick persistence, current-map item deactivation, enemy cleanup, and checkpoint activation state updates.
- `Game` delegates those responsibilities to the system instead of mixing map-file JSON mutation and cleanup rules directly into the gameplay loop.
- The entity registry sync remains connected through a callback so the system can maintain the same active-world behavior without reintroducing global coupling.
- The behavior remains unchanged: map transition persists picked items before changing maps, checkpoint activation still marks objects as active in the map JSON, and enemy cleanup still clears the active world and dead-enemy tracking.

This is a small, safe extraction that reduces the `Game` controller without broadening the refactor into unrelated gameplay logic.

## Completed Step 10: Extract map transition flow from the game session

The map-change lifecycle is now separated from the gameplay coordinator:

- `MapFlowSystem` owns the actual map-transition mechanics: changing the current map file, reloading the tiled map, resetting the active world, and repopulating the new map.
- `Game` still decides when the transition should happen, but it no longer owns the concrete map-teardown and repopulation details.
- The move preserves the original behavior for respawn and waypoint transitions while keeping the state reset sequence explicit and testable.

This is another narrow extraction in the same direction: lower the orchestration weight of `Game` while preserving gameplay semantics and callback ordering.

## Completed Step 7: Make resource paths and audio identifiers configurable

The resource-configuration slice is complete at the current boundary:

- `ResourcePaths` centralizes the main map, texture, HUD texture, font, sound, and animation folders.
- `SoundIds` centralizes the audio identifiers used by gameplay actors.
- `Game` uses the centralized map, texture, HUD, and sound paths for its resource setup.
- `AnimationSet` receives the white damage palette asset path from the Game layer when needed.
- Every gameplay entity, item, projectile, NPC, checkpoint, door, and bloodstain that loads an `.fdset` animation file now uses `ResourcePaths::ANIMATIONS` instead of repeating the folder literal.
- `Ui::HUD_TEXTURES_PATH` and `Ui::FONTS_PATH` were removed; all UI consumers use `ResourcePaths::HUD_TEXTURES` and `ResourcePaths::FONTS`.
- Gameplay sound calls use `SoundIds` instead of repeating string literals.
- `gameResourceConfig.h` is registered in the game project and filters; resource paths and sound IDs are Game-owned.

Individual asset filenames (e.g. `antHero.fdset`, `bloodstain.fdset`) remain literal in their owning game-specific classes; externalizing them into a data-driven asset registry is not required by this step and would be unnecessary abstraction, since these classes already belong to the game layer, not the engine. The sound-name-to-file mapping in `Game::Game()` also remains in the game layer for the same reason.

### Validation

The project was rebuilt in Visual Studio 2019 and the game was tested successfully after the animation path centralization.

## Completed Step 8: Separate generic JSON/file persistence from the game-specific save schema

`SaveHandler` included `item/itemsHub.h` without using any type from it; the include was dead weight pulling an unrelated game-content header into the persistence class. It has been removed, with no behavior change.

A generic `JsonFileStore` helper (`jsonFileStore.h`/`.cpp`) now owns the raw file-open/parse/write mechanics shared by every JSON persistence call site:

- `JsonFileStore::readJsonFile()` returns `JsonFileResult::FileNotFound`, `ParseError`, or `Success`, using a non-throwing parse instead of letting a malformed file crash the caller.
- `JsonFileStore::writeJsonFile()` opens, writes, and reports success/failure without the caller managing the stream directly.
- `SaveHandler::save()`/`load()` now call the helper and keep their existing, distinct error messages for "file not found" versus "parse failed".
- `Game::updateMaps()`'s item-flag persistence and `Game::saveCheckpointActivatedState()` now call the helper too. Both previously parsed the map file with no open/parse check at all (a latent crash on a missing or malformed map file); they now skip the mutation instead of crashing if the read fails. This is a deliberate, visible behavior improvement, not a side effect of deduplication.
- `SaveHandler` still owns the Ant Hero-specific save schema (hero stats, inventory ids, doors, bosses, bloodstain) — only the generic file mechanics moved out, per this step's goal.
- `jsonFileStore.h`/`.cpp` are registered in the Visual Studio project and filters.

### Validation

The project was rebuilt in Visual Studio 2019 and the game was tested successfully, covering save/load, item pickup persistence across a map reload, and checkpoint activation persistence.

A follow-up build broke on `saveHandler.h`: removing the unused `item/itemsHub.h` include also silently removed the only transitive path bringing `<string>`/`using namespace std;` into that header. `saveHandler.h` now includes `<string>` and declares `using namespace std;` itself instead of depending on an unrelated header for that.

## Progress On Step 9: Reduce Game to orchestration of systems, scenes, and game states

`Game` is currently a single ~1600-line class with roughly 40 methods covering menu handling, the main gameplay loop, map population/spawning, save/load, per-entity interaction checks, and rendering. A full split into separate system classes is a large, systemic change with real regression risk if attempted as a single pass, and it is not being done in one shot.

The first safe slice is done: `runMainGame()` and `runPausedGameMenu()` (the two active-scene methods) each ended with an identical, duplicated four-line sequence — update map state, draw the frame, then update the camera. That sequence is now a single `Game::renderFrame()` method called by both scenes, so the two scene methods no longer repeat the same end-of-frame orchestration.

The map-transition slice is also complete at the current boundary:

- `MapTransitionSystem` owns fade alpha, transition phases, and the one-shot map-change callback.
- `Game` remains responsible for map loading, entity cleanup, item persistence, and map population through that callback.
- Picked-item JSON persistence now lives in `Game::persistPickedMapItems()` instead of inside the fade loop.
- Starting a fade while already transitioning is ignored, so remaining inside a waypoint cannot restart the transition every frame.
- `Game::renderFrame()` updates and synchronizes the camera before drawing the frame.
- Tile rendering and the tileset texture cache now live in `TileRenderer`; `Game::renderTiles()` only delegates the current Tileson map to it.

Remaining candidates for later slices, each requiring its own deliberate design rather than a mechanical move:

- Map population/spawning (`buildDoors`, `buildWalls`, `buildWaypoints`, `spawnEnemies`, `spawnBoss`, `spawnCheckpoints`, `spawnItemsFromCurrentMap`) is a cohesive responsibility, but it touches most of `Game`'s state (`currentMap`, the entity registry, `walls`, `fogWalls`, `currentMapEnemies`, `hero`, `currentBoss`, `gui`, id lists) and would need a real ownership design before extraction, not just a copy-paste into a new class.
- The five `checkAndHandleNear*`/`checkAndHandleEnemyLoot` interaction checks are a coherent "system," but they read and write `hero`, `actionMessageUi`, and several `Game` flags, and call back into `spawnItem()`/`saveCheckpointActivatedState()`; extracting them now would just relocate the coupling rather than reduce it, unless those dependencies are deliberately narrowed first.
- Save/load orchestration (`saveGame`, `loadGame`, `loadInventoryItems`) already delegates persistence mechanics to `SaveHandler`/`JsonFileStore`; `Game` mostly just marshals data between them and its own state, which is closer to legitimate orchestration than a leftover responsibility.

### Validation

The project was compiled manually in Visual Studio 2019 and launched successfully after the map-transition extraction. Map rendering, map traversal, fade behavior, and the refactored orchestration path were confirmed working in gameplay.

The project was compiled manually in Visual Studio 2019 and launched successfully after the `TileRenderer` extraction. Tile rendering, entity rendering, and map traversal continued to work.

The next candidate is a focused map-population slice. It should extract only the repeated map setup orchestration around `handleMapChange()` while keeping `Game` responsible for the session-owned entity lists and progression state.

## Completed Step 11: Extract interaction checks into InteractionSystem

The five interaction check methods (`checkAndHandleEnemyLoot`, `checkAndHandleNearItem`, `checkAndHandleNearDoor`, `checkAndHandleNearCheckpoint`, `checkAndHandleNearBloodstain`) were extracted into `InteractionSystem`:

- `InteractionSystem` owns per-entity interaction detection and message triggering.
- Game callbacks (`spawnItemCallback`, `saveCheckpointCallback`) decoupled `Game` mutation from the interaction system.
- The extraction narrowed `Game` responsibilities while preserving gameplay interaction behavior.

## Completed Step 8: Physical File Separation for Engine vs Game Modules

The directory structure and Visual Studio project entries were migrated into clean module paths:

- **Engine modules**:
  - `inc/engine/core/` and `src/engine/core/` (`entity`, `livingEntity`, `timeController`, `randomNumber`)
  - `inc/engine/rendering/` and `src/engine/rendering/` (`animation`, `animationSet`, `frame`, `drawing_functions`, `renderContext`, `displayConfig`)
  - `inc/engine/audio/` and `src/engine/audio/` (`soundManager`)
  - `inc/engine/input/` and `src/engine/input/` (`inputCommand`, `keyboardInput`, `joystickInput`)
  - `inc/engine/world/` and `src/engine/world/` (`cameraController`)
  - `inc/engine/data/` and `src/engine/data/` (`res_path`, `jsonFileStore`, `tiledMapLoader`, `cleanup`, `groupBuilder`, group data types)

- **Game modules**:
  - `inc/game/actors/` and `src/game/actors/` (`hero`, `enemyEntity`, `glob`, `grob`, `termiteMiner`, `roundKing`, `bullet`, `lootDropSource`)
  - `inc/game/items/` and `src/game/items/` (`item`, `honeydewPotion`, `greenBerry`, `key`, `stone`, `stoneProjectile`, `itemsHub`)
  - `inc/game/npcs/` and `src/game/npcs/` (`door`, `npcData`, `npcFactory`, `friendlyNpc`, `smallBrownSpider`)
  - `inc/game/world/` and `src/game/world/` (`map`, `wall`, `checkpoint`, `bloodstain`)
  - `inc/game/ui/` and `src/game/ui/` (`ui`, `hpBar`, `quickItemUi`, `essenceCounterUi`, `actionMessageUi`, `itemPickMessageUi`, menus)
  - `inc/game/persistence/` and `src/game/persistence/` (`saveHandler`, `gameSaveManager`)
  - `inc/game/systems/` and `src/game/systems/` (`mapStateSystem`, `mapFlowSystem`, `mapPopulationSystem`, `interactionSystem`, `tileRenderer`, `mapTransitionSystem`, menus)
  - `inc/game/game.h` and `src/game/game.cpp`

The Visual Studio project configuration (`aula_sdl2.vcxproj` and `.filters`) was updated with relative include directories and paths. Diagnostics confirmed 0 errors across the workspace.

## Completed Step 12: Eliminate Legacy Globals Class and Introduce MathUtils/StringUtils

The legacy `Globals` class (`globals.h`/`globals.cpp`) was completely removed from the project:

- Created `MathUtils` (`inc/engine/core/mathUtils.h`) containing mathematical constants (`MathUtils::PI`).
- Created `StringUtils` (`inc/engine/data/stringUtils.h` and `src/engine/data/stringUtils.cpp`) containing data parsing utilities (`StringUtils::clipOffDataHeader`).
- Refactored `Entity`, `GroupBuilder`, `Animation`, `AnimationSet`, and `Frame` to use the new Engine utility namespaces.
- Removed legacy `#include "globals.h"` from all Game layer classes and composition roots.
- Removed `globals.h`/`globals.cpp` from the solution and project build filters.

## Completed Step 13: Separate Engine Static Library Project

The reusable engine code was formally extracted into a dedicated Visual Studio project (`engine.vcxproj` / Static Library):

- **Created `engine.vcxproj` and `engine.vcxproj.filters`**:
  - Compiles as a C++ Static Library (`StaticLibrary`).
  - Contains all source files in `src/engine/` (`core`, `rendering`, `audio`, `input`, `world`, `data`).
  - Exposes public headers in `inc/engine/`.
- **Refactored `aula_sdl2.vcxproj`**:
  - Removed all `src/engine/` and `inc/engine/` items from the game project build list.
  - Added a `<ProjectReference Include="engine.vcxproj">` link so `aula_sdl2` automatically builds and links the `engine` static library.
- **Updated `aula_sdl2.sln`**:
  - Added `engine.vcxproj` to the Visual Studio solution file with build configurations for `Debug|Win32`, `Release|Win32`, `Debug|x64`, `Release|x64`.

Diagnostics confirmed 0 errors across both projects in the workspace.

## Completed Step 14: Remove Game Concepts from Engine Headers

The remaining Ant Hero concepts were moved out of Engine:

- `SessionStats` became `GameKillStats` in `inc/game/gameKillStats.h`; only game actors hold or update it.
- The pheromone trail moved from `LivingEntity` to `Hero`, exposed to game AI through `PheromoneTrailSource`.
- `ResourcePaths` and `SoundIds` moved to `inc/game/gameResourceConfig.h`.
- `AnimationSet` now accepts the white-palette asset path from its caller instead of importing a game resource configuration header.
- The game project owns game configuration headers; Engine no longer lists or includes them.

Focused editor diagnostics report no errors in the changed headers, actors, map population code, and project files. Full build/runtime validation remains dependent on the local SDL include/library configuration.

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

- `Entity`: reusable movement/collision core mixed with animation, globals, and entity registry.
- `LivingEntity`: reusable health/damage behavior mixed with pheromone trails, enemy IDs, and game save behavior.
- `CameraController`: now receives the camera rect and world bounds as explicit parameters to `update()` instead of reading/writing `Globals::camera` or hardcoding Ant Hero's `1024 x 1024` map size; `Game` owns the `WORLD_WIDTH`/`WORLD_HEIGHT` constants and supplies `Globals::camera` at the call site.
- `Wall`: potentially a generic solid obstacle, though currently an SDL/game entity.
- `Ui`, `Menu`, and `PaginatedMenu`: generic visual/navigation base with SDL and game-specific subclasses.
- `NpcData`: data-transfer structure that could belong to a generic content/data layer, while its current schema is game-specific.
- `NpcFactory`: content loading infrastructure currently creating a concrete gameplay NPC.
- `SaveHandler`: owns the Ant Hero-specific save schema; the generic file-open/parse/write mechanics now live in `JsonFileStore`.
- `Globals`: rendering and camera infrastructure mixed with pause/debug state.

## Critical Dependency Problems

The current dependency direction still contains some engine-to-game leakage. Resolved items (the static entity registry, `KeyboardInput`/`JoystickInput` referencing `Hero`, `CameraController` depending on `Globals`, and `AnimationSet` reading `Globals::renderer` internally) have been removed from this list as their fixes landed; see the completed steps above for details.

Remaining leakage:

- `Game` manipulates Tileson directly, including layer names and map properties (acknowledged remaining work from Step 6).
- Gameplay entities call the global `SoundManager::soundManager` singleton directly, rather than receiving a sound service dependency.
- UI classes still read `Globals::renderer` directly for drawing; this is expected, since concrete UI classes are Game-Specific Components, not engine candidates.

The main global-state obstacles are:

- `Globals::renderer` (still read directly by UI classes).
- `Globals::camera` (still the storage owned by `Game`, but no longer read/written directly by `Entity`, `LivingEntity`, `Frame`, `AnimationSet`, or `CameraController`).
- `Globals::pause`.
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
