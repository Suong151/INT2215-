A game made for INT2215

A Codeblocks version with MinGW SDL2 Development Library with MinGW SDL_image Development Library with MinGW Setting up IDE Go to Settings -> Compiler -> Global compiler settings -> Linker settings and fill the Other linker options box with -lmingw32 -lSDL2main -lSDL2 -lSDL2_image

Go to Settings -> Compiler -> Global compiler settings -> Search directories and add:

i686-w64-mingw32\include\SDL2 from the SDL2 and SDL_image library you've just downloaded to the Compiler tab.

Go to Settings -> Compiler -> Global compiler settings -> Search directories and add:

i686-w64-mingw32\include i686-w64-mingw32\lib from the SDL2 and SDL_image library you've just downloaded to the Linker tab.

Compiling Just open Codeblocks and drag the .cbp file into Codeblocks. Finally press Build and Run.

The images used in the game were drawn by me. Sounds used in the game are taken from https://pixabay.com/vi/ The functions used are referenced from the SDL library.
