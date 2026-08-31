# Blokk v0.1.6
> 8/31/2026

## Update Notes

### Major Changes

* **Removed the internal field update command queues**

  * Object data is now updated directly instead of being passed through update queues.
  * Removed the associated queue variables, functions, and processing implementations.
  * Simplifies the engine's internal update system and reduces unnecessary intermediate work.

* **Reworked object destruction**

  * Replaced the previous `DestroyObject` implementation with separate:

    * `DestroyDynamicObject`
    * `DestroyStaticObject`
  * Improved handling of objects stored in static and dynamic data.
  * Fixed object data and index handling when objects are moved between storage types.

* **Improved static/dynamic object management**

  * Fixed logic for switching objects between static and dynamic storage.
  * Added visibility handling to object swapping and creation operations.
  * Fixed a bug where creating a dynamic object when no static objects existed could leave its internal index incorrect.

### Visibility

* Added a new visibility implementation with direct engine data manipulation.
* Added visibility support to object creation parameters.
* Added a `Visible` field to `ObjectCreationParams`.
* Updated visibility-related object operations to work with the new direct data system.

### Engine Internals

* Renamed the internal `EngineObjects` variable to `Engine`.
* Changed its type from `size_t` to `uint32_t`.
* Added a destructor to improve cleanup when the engine/project is destroyed.
* Added the `IndexRange` structure to the `Blokk` namespace.
* Made `IndexRange::GetSize()` `const`.

### Threading

* Adjusted thread initialization so the worker's static `ObjectManager` reference is set **before** opening the worker thread.
* This fixes initialization ordering and makes the worker setup more reliable.

### Rendering

* Fixed a rendering offset issue where positions were being offset in the wrong direction.

## Cleanup

* Removed obsolete update queue implementations.
* Removed old field update command processing.
* Consolidated duplicate `ProcessAddCommand` implementations.
* Removed internal functions that were only required by the previous queue-based update system.

Overall, **v0.1.6 simplifies Blokk's internal data flow, improves object management, and lays the groundwork for more direct and efficient engine updates.**


# v0.1.5
> 8/30/2026

## New Features
- Added configurable thread control with `Blokk_Thread_Control`.
- Added `Blokk_Thread_AdaptiveTiming` for automatically adjusting the number of worker threads based on frame execution time.
- Added `Blokk_Thread_FixedCount` for manually controlling the number of worker threads.
- Added `SetThreadCount()` to change the active worker thread count.
- Added safety checks for invalid thread counts.
- Improved SIMD visibility culling organization with internal helper functions.
- Added improved namespace organization for internal engine functionality.

## Visibility Culling
- Refactored Basic and Axis visibility culling.
- Organized SSE2, AVX2, and AVX-512 visibility functions under `InternalHelpers`.
- Fixed visibility range indexing when processing worker ranges.

## Threading
- The engine can now choose between adaptive and fixed thread-count modes.
- Adaptive threading can increase or decrease the number of active worker threads based on performance.
- Fixed threading allows the user to specify exactly how many worker threads the engine should use.

## V0.1.4
> 8/28/2026

* Cleaned up several parts of the engine code and removed unused update queues.

### Added:

* Camera support with X and Y movement.
* Camera-aware visibility culling across SSE2, AVX2, and AVX-512.

### Fixed:

* Object creation and destruction.
* Queued update commands not being removed after processing.

### Improved:

* Handling of static and dynamic objects when creating, destroying, and swapping them.
* Some diagnostics getters and general code consistency.

### Summary
This update mainly focuses on making object management more reliable while laying the groundwork for camera support.

> **PLEASE NOTE:**
> This update's new camera **does not work with basic culling**; you'll have to enable visibility culling to use the camera

## V0.1.3
> 8/24/2026

## Diagnostics & Architecture Update

### Added
- Added engine diagnostics through `PrintDiagnostics()`.
- Added getters for:
  - Total object count
  - Static object count
  - Dynamic object count
  - Opened thread count
  - Total thread count
  - Previous frame execution time
  - Target execution time
  - SIMD register size
- Added the ability to set the target engine execution time.
- Added a basic example project template.

### Changed
- Separated engine updating and rendering into separate operations.
- `EngineProcess()` is being replaced with separate update and render stages.
- Reorganized example projects and documentation into dedicated directories.
- Improved the public engine API for easier integration into projects.

### Performance
- Continued optimization of hot engine loops.
- Reduced unnecessary runtime branching in engine systems.
- Improved diagnostics for identifying engine performance bottlenecks.

### Documentation
- Added and reorganized documentation for the engine.
- Added a basic example project showing the minimum setup required to use Blokk.

## V 0.1.1:
> 8/22/2026


### Added

- New rendering culling system: Axis Culling
- Added `AnimFrameCounts` to track animation frame counts
- Added documentation for:
  - Requirements
  - Visibility culling systems

### Changed

- Updated SIMD level detection and function selection
- Split visibility culling into Basic Culling and Axis Culling
- Changed `IndexRange` and related size/index values from `size_t` to `uint32_t`
- Updated CMake to include the new visibility culling files
- Renamed visibility-related files to be more specific

### Performance

- Eliminated switch statements from hot loops in visibility and velocity processing
- Added SIMD-based dispatch for the selected SIMD level
- Improved animation frame handling

### Removed

- Removed AVX support
- Removed the incomplete Blokk-C transpiler
- Removed NEON support for now
