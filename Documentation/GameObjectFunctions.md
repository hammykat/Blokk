# Game Object function list

## Velocity
| Function | Use
|----------|--------
| `void SetVelocity(float XVel, float YVel);` | Sets the velocity to the input X and Y
| `void SetVelocity(Vector2 Velocity);` | Sets the velocity to the input Vector2
| `void SetVelocityX(float XVel);` | Sets the X velocity to the input
| `void SetVelocityY(float YVel);` | Sets the Y velocity to the input
| `Vector2 GetVelocity();` | Returns the velocity as a Vector2
| `float GetVelocityX();` | Returns the X velocity as a float
| `float GetVelocityY();` | Returns the Y velocity as a float
| `float GetSpeed();` | Returns the speed in pixels per second as a float
| `float GetSpeedSquared();` | Returns the speed squared in pixels per second as a float
| `bool IsMoving();` | Returns a bool representing if the object is moving (velocity == 0)
| `void StopVelocity();` | Sets the object's velocity to 0
| `void ReverseVelocity();` | Reverse the object's x and y velocity
