# Using the engine

You can use the engine by creating a new instance of the `ObjectManager` class. 
The constructor accepts a `ManagerCreation` struct as the argument:
```cpp
struct ManagerCreation 
{
    BlokkCulling CullingType; // The system the engine will use to cull to get the visible objects
    Vector2 ScreenDimensions; // The screen width and height
    uint32_t FPS = 30;  // The FPS
    bool Debug = false; // If Debug / Diagnostics is enabled
};
```

Then, use a basic raylib loop, and inside, use the `EngineProcess();` member function of `ObjectManager` to let the engine handle all the objects.

For the CullingType, [check here](RenderCullingSystems.md) for more info.
For the Debug, [check here](EngineDiagnostics.md) for more info.
