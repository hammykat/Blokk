# Game Object function list

## Constructor

| Function                                                      | Use                                                         |
| ------------------------------------------------------------- | ----------------------------------------------------------- |
| `GameObject(GameObjectCreationParams CP = {{0, 0}, {0, 0}});` | Creates a GameObject with the specified creation parameters |

## Helpers

| Function                                                                                                                                                   | Use                                                             |
| ---------------------------------------------------------------------------------------------------------------------------------------------------------- | --------------------------------------------------------------- |
| `void UpdateEngineData(CommandTypes Type, vector<float> *Values, size_t Idx, float OtherVal);`                                                             | Queues an update to a single engine data value                  |
| `void UpdateEngineData_Double(CommandTypes Type, vector<float> *XVals, vector<float> *YVals, const float XUpdateVal, const float YUpdateVal, size_t Idx);` | Queues an update to two related engine data values              |
| `void SetToStatic();`                                                                                                                                      | Changes the object from dynamic to static                       |
| `void SetToDynamic(Vector2 Vel);`                                                                                                                          | Changes the object from static to dynamic and sets its velocity |
| `void SetToVisible();`                                                                                                                                     | Makes the object visible                                        |
| `void SetToInvisible();`                                                                                                                                   | Makes the object invisible                                      |

## Velocity

| Function                                               | Use                                                           |
| ------------------------------------------------------ | ------------------------------------------------------------- |
| `void SetVelocity(float XVel, float YVel);`            | Sets the velocity to the input X and Y                        |
| `void SetVelocity(Vector2 Velocity);`                  | Sets the velocity to the input Vector2                        |
| `void SetVelocityX(float XVel);`                       | Sets the X velocity to the input                              |
| `void SetVelocityY(float YVel);`                       | Sets the Y velocity to the input                              |
| `Vector2 GetVelocity();`                               | Returns the velocity as a Vector2                             |
| `float GetVelocityX();`                                | Returns the X velocity as a float                             |
| `float GetVelocityY();`                                | Returns the Y velocity as a float                             |
| `float GetSpeed();`                                    | Returns the speed in pixels per second as a float             |
| `float GetSpeedSquared();`                             | Returns the speed squared without calculating the square root |
| `bool IsMoving();`                                     | Returns a bool representing whether the object is moving      |
| `void StopVelocity();`                                 | Stops the object by setting its velocity to 0                 |
| `void ReverseVelocity();`                              | Reverses both the X and Y velocity                            |
| `void ReverseVelocityX();`                             | Reverses the X velocity                                       |
| `void ReverseVelocityY();`                             | Reverses the Y velocity                                       |
| ----------------------------                           | --------------------------------                              |
| **Multiply**                                           |                                                               |
| `void MultiplyVelocity(float FactorX, float FactorY);` | Multiplies the X and Y velocity by specific factors           |
| `void MultiplyVelocity(Vector2 Factor);`               | Multiplies the X and Y velocity by the values in a Vector2    |
| `void MultiplyVelocity(float Factor);`                 | Multiplies both the X and Y velocity by the same factor       |
| `void MultiplyXVelocity(float Factor);`                | Multiplies the X velocity by a factor                         |
| `void MultiplyYVelocity(float Factor);`                | Multiplies the Y velocity by a factor                         |
| ----------------------------                           | --------------------------------                              |
| **Add**                                                |                                                               |
| `void AddVelocity(float XVel, float YVel);`            | Adds the input X and Y values to the velocity                 |
| `void AddVelocity(Vector2 Vel);`                       | Adds the X and Y values of a Vector2 to the velocity          |
| ----------------------------                           | --------------------------------                              |
| **Subtract**                                           |                                                               |
| `void SubtractVelocity(float XVel, float YVel);`       | Subtracts the input X and Y values from the velocity          |
| `void SubtractVelocity(Vector2 Vel);`                  | Subtracts the X and Y values of a Vector2 from the velocity   |
| ----------------------------                           | --------------------------------                              |
| **Divide**                                             |                                                               |
| `void DivideVelocity(float XVel, float YVel);`         | Divides the X and Y velocity by specific values               |
| `void DivideVelocity(Vector2 Vel);`                    | Divides the X and Y velocity by the values in a Vector2       |
| `void DivideVelocityX(float Vel);`                     | Divides the X velocity by the input value                     |

## Position

| Function                                             | Use                                                     |
| ---------------------------------------------------- | ------------------------------------------------------- |
| `void SetPosition(float TargetX, float TargetY);`    | Sets the position to the input X and Y coordinates      |
| `void SetPosition(Vector2 TargetPos);`               | Sets the position to the input Vector2                  |
| `void SetXPosition(float Target);`                   | Sets the X position to the input value                  |
| `void SetYPosition(float Target);`                   | Sets the Y position to the input value                  |
| `Vector2 GetPosition();`                             | Returns the object's position as a Vector2              |
| ----------------------------                         | --------------------------------                        |
| **Change**                                           |                                                         |
| `void ChangePosition(Vector2 Change);`               | Changes the X and Y position by the values in a Vector2 |
| `void ChangePosition(float ChangeX, float ChangeY);` | Changes the X and Y position by the input values        |
| `void ChangeXPosition(float X);`                     | Changes the X position by the input value               |
| `void ChangeYPosition(float Y);`                     | Changes the Y position by the input value               |

## Visibility

| Function                 | Use                        |
| ------------------------ | -------------------------- |
| `void SetToVisible();`   | Makes the object visible   |
| `void SetToInvisible();` | Makes the object invisible |
