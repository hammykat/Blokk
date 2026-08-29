# Camera

The camera is an optional feature that lets you move the view around the game world. When enabled, objects are rendered relative to the camera's position, so moving the camera changes which part of the world is shown on screen.

To enable the camera, add:

```cpp
#define Blokk_CamEnabled
```

You can set or change the camera's position using these functions:

| Function                                  | Use                                         |
| ----------------------------------------- | ------------------------------------------- |
| `void SetPosition(int32_t X, int32_t Y);` | Set the camera's position                   |
| `void SetPosition(Vector2 Pos);`          | Set the camera's position using a `Vector2` |
| `void SetXPosition(int32_t X);`           | Set the camera's X position                 |
| `void SetYPosition(int32_t Y);`           | Set the camera's Y position                 |
| `void ChangeXPosition(int32_t X);`        | Move the camera along the X axis            |
| `void ChangeYPosition(int32_t Y);`        | Move the camera along the Y axis            |

The camera position represents the point the view is offset from in the world. For example, increasing the camera's X position moves the view toward the right side of the world.
