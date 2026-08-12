The blokk engine is the part of the engine that handles GameObjects for you every frame. 
It uses systems, which are currently:
- Movement with Velocity and Collisions
- Animations
- Sounds
- Visibility / Rendering

#Parallelism#

  The engine will open threads according to the workload.
For example, at the start of the program, it'll only open 1,
but if the object count reaches the thousands, it'll open more, 
and if it goes up to the 100 thousands, it'll open the  max amount
of threads it can open without exceeding the user's cpu core 
count. 

  It'll distribute the workload across all the threads at the 
start of the frame by assigning each thread their own queue of 
ranges of objects to complete a system on. These ranges are 
determined at the start of the frame by the main thread. But if 
the engine sees that no objects were added and no fields were 
updated in the last frame, it'll reuse the same queues from the 
previous frame.

#SIMD#

  The engine will use SIMD to carry out multiple operations
at one time. It matters on how big the user's computer's SIMD 
register width. For example, if it can carry 4 floats (128 bit), 
then the engine will carry out the operations on 4 objects on a 
time. The same happens with 256 and 512 bit registers (8 and 16 
floats).

#Data Layout#

  The user creates a new object, and the engine stores all
the object's fields in an SoA (Structure of Array) form. This 
offers cache locality and makes SIMD faster and easier. 

  The engine stores the objects that are static and dynamic
together, which makes the operations super fast with SIMD. The
engine stores the velocities and positions of the static and 
dynamic objects together, the animations of the objects together 
according to their visibility, and the collisions together.

  The engine won't store the velocities of static objects,
because they are all {0, 0}. It only stores the velocities of
dynamic objects.

