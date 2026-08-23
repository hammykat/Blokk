# Render culling systems

There are currently 2 culling systems that you can choose the engine to use for rendering:

## 1. Basic Culling

Here, the engine compares every object's x and y to check if they are in the screen.

### Good situations:
* A few hundred objects
* Objects are spread fairly evenly
* Objects don't have wildly different sizes
* You want the simplest culling implementation
* You care more about low overhead than squeezing out every last operation

## 2. Axis Culling

In this, the engine will check each object's y position, filter out all the objects that can't be inside the screen based off of the results, then check the remaining x positions. This can let it perform almost 2x less work than basic culling.

**Best for:** large scenes, large worlds, and situations where many objects can be quickly rejected by their X or Y position.

### Good situations
- Thousands or more objects
- Large worlds with a relatively small viewport
- Objects spread across a large area
- Many objects are completely outside the screen
- Objects have predictable or relatively simple bounding dimensions
- Visibility checks are a major part of frame time
- You want to take advantage of SIMD processing
- You need high-throughput culling for large object counts

### Example

A large world might contain:

100,000 objects  
↓  
X-axis culling  
↓  
20,000 candidates  
↓  
Y-axis culling  
↓  
4,000 visible objects  
↓  
Render

AxisCulling is especially useful when most objects are far outside the viewport, because objects can be rejected without performing the complete visibility check.
