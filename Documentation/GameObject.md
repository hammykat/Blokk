# Game Objects

Creating a game object is only creating an instance of the class. While creating the instance, you can specify certain starting values, like the position and velocity, in the form of a struct called `ObjectCreationParams` (`Vector2 Velocity; Vector2 Position`). The default starting position and velocity are `{0, 0}`. 

With the instance created, you can then call it's functions normally like any class. But you can't directly change or access it's variables, and instead you have to use getting/setting functions. This is because the engine handles all of the object data in the form an SoA, and the engine needs to access the data, change it's state, etc. 

For a list of the functions, check the [function list](GameObjectFunctions.md)
To take a deeper dive on how the engine handles objects, check the [engine documentation](EngineArchitecture.md)

If you do this:

```cpp

GameObject MyObject = new();

MyObject.SetVelocity(Vector2{5, 6});

std::cout << MyObject.GetVelocityX();

```

The output will be 5 because the object directly reads and mutates the engine's data.
