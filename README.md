Program to render glsl shaders
---

Inspired by shadertoy, this will allow you play with shaders on the desktop.

### Info
- Shaders are written in standard glsl, without requiring the #version preprocessor directive.
- Added C style #include preprocessor directive
- 00 is in the lower left corner
- Output vec4 color must be put into DiffuseColor
- Number of inputs are provided by the program:

    | Input | Type | Description |
    | :--- | :--- | :--- |
    | MousePosition | vec4 | Cursor position in x & y, lb and rb in z & w respectively |
    | Resolution | vec2 | Window pixel resolution |
    | DeltaTime | float | Time spent in the last frame |
    | TotalTime | float | Time since program start |
    | FragCoord | vec4 | Current fragment position |

### Usage

Start program from the command line to load a specific shader. Shader file name must be the first argument. Window size can be set with -w and -h, or --width and --height. Default window size is 1000x1000. If no arguments are given, program will look for a file called "shader1.shader".

Example start command:

```sh
shaderthing.exe test.shader -w 500 -h 500
```
