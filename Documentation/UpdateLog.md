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
