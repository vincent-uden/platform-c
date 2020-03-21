# Platform C

This is a small 2D platforming prototype written in C99/ANSI-C based on SDL2. It includes a playable environment and a map editor in the same binary.

![](https://thumbs.gfycat.com/ApprehensiveAffectionateFieldmouse-size_restricted.gif)


## Building
The included Makefile should cover everything. "make" to compile and "make run" to run the binary. SDL2 with all of it's library's (such as SDL_Image) are needed to compile the project.

## Controls
- Left/Right = A/D
- Jump = Space
- Open/Close Map Editor = F
  - Rectangle tool = R
  - Select tool = S
  - Save map = N
  - Load map = M
  - Toggle Grid = G
  - Grid Size + = K (Ctrl can be used for fine control)
  - Grid Size - = J (Ctrl can be used for fine control)
- Toggle frame-by-frame mode = P
  - Advance one frame = N

There are additional controls shown in the map editor through the interface.
