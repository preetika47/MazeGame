# MazeGame
# Simple C++ Console Maze Game

A basic console-based maze navigation game written in C++. Players guide 'P' through a maze, collect items '*', avoid enemies 'X', and reach the exit 'E' to progress through levels.

This version uses **Windows-specific libraries** (`windows.h`, `conio.h`) for console manipulation (colors, screen clearing, direct character input without needing Enter). Therefore, it is **designed to be compiled and run on Windows systems.**

## Features

*   Text-based maze rendering in the console.
*   Player movement using WASD keys.
*   Collectibles ('*') that must be gathered to complete a level.
*   Randomly moving enemies ('X') that end the game on contact.
*   Multiple levels with increasing collectibles (and potentially more enemies on later levels).
*   Score and move tracking.
*   Game save and load functionality (to `maze_save.txt`).
*   Colored output using the Windows Console API for better visualization.

## Requirements

*   **Windows Operating System:** This code relies heavily on `windows.h` and `conio.h` and will *not* compile or run correctly on Linux or macOS without significant modification.
*   **C++ IDE** Like Code Blocks.

## How to Compile & Run in Code Blocks:

You need two files: `Maze.h` and `Maze.cpp`.
1.  Save the header code as `Maze.h`.
2.  Save the implementation code as `Maze.cpp`.
3.  Click on Build and Run. The console will pop up.


## How to Play
1.  Follow the on-screen instructions.
2.  **Movement:**
    *   `W`: Move Up
    *   `A`: Move Left
    *   `S`: Move Down
    *   `D`: Move Right
3.  **Objective:**
    *   Navigate the player `P` through the maze passages (represented by spaces).
    *   Avoid the walls (`#`).
    *   Collect all the collectibles (`*`) scattered throughout the maze. You gain 10 points per collectible.
    *   Avoid colliding with the enemies (`X`). If `P` and `X` occupy the same square, the game is over.
4.  **Winning a Level:**
    *   Once all collectibles (`*`) are gathered, move the player `P` to the exit (`E`).
    *   This will advance you to the next level, which will have a new layout and more collectibles.
5.  **Other Controls:**
    *   `V`: Save the current game state (level, score, moves, player/enemy positions, maze state) to `maze_save.txt`.
    *   `L`: Load the game state from `maze_save.txt`. If the file doesn't exist or is corrupt, an error will be shown.
    *   `Q`: Quit the game at any time.

## Save File

The game saves and loads progress using a plain text file named `maze_save.txt` located in the same directory as the executable.
