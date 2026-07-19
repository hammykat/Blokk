# AI Agent Instructions for C Raylib Project

## Project Overview
This is a C graphics/game development project using **raylib**, a lightweight, cross-platform graphics library. The project compiles to a Windows executable using GCC.

## Build & Compilation

### Build Command
```bash
gcc main.c -o game.exe -I include -L lib -lraylib -lopengl32 -lgdi32 -lwinmm
```

### Key Build Details
- **Compiler**: GCC
- **Include path**: `-I include` (contains raylib.h, raymath.h, rlgl.h headers)
- **Library path**: `-L lib` (contains libraylib.a, libraylibdll.a, raylib.dll)
- **Linked libraries**: raylib, opengl32, gdi32, winmm
- **Output**: game.exe

### Running
Execute `game.exe` directly or `./game.exe` in terminal.

## Project Structure

```
.
├── main.c              # Main application code
├── include/            # Raylib headers (raylib.h, raymath.h, rlgl.h)
├── lib/                # Raylib libraries (libraylib.a, libraylibdll.a, raylib.dll)
└── game.exe            # Compiled executable
```

## Key Conventions

### Raylib API Pattern
- **Window**: `InitWindow()` → game loop → `CloseWindow()`
- **Drawing**: `BeginDrawing()` → draw calls → `EndDrawing()`
- **Color system**: Use predefined colors (BLACK, RED, etc.) or create custom colors with CLITERAL(Color)

### Struct Naming
- Use PascalCase for custom types (e.g., `RayLibRect`)
- Always include curly braces for struct initialization

## Common Tasks

### Add New Drawing Objects
- Define a struct in main.c
- Initialize in the setup phase (before game loop)
- Add drawing calls inside the game loop, between `BeginDrawing()` and `EndDrawing()`

### Update Game Logic
- Place update logic inside the main game loop before drawing calls
- Use `GetFrameTime()` for delta-time calculations if needed

## Tips for AI Agents
- Always respect the include path (`-I include`) and library path (`-L lib`) in compilation
- Raylib functions require proper window initialization before use
- Drawing operations must occur between `BeginDrawing()` and `EndDrawing()`
- For graphics operations, refer to raylib documentation for the specific function signatures
