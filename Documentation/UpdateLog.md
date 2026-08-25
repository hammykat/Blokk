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
