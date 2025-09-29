# Sudoku Game (C)

This repository contains a multi-player Sudoku game written in C.  
The program manages several players, builds a balanced tree of active players, and iteratively advances each player’s board using constraint-based steps and user choices. Winners and their final boards are written to a file.

## Features
- 9×9 Sudoku with standard row/column/box rules.
- Computes possible digits per cell (constraint propagation).
- Iterative single-candidate filling; when stuck, asks the user to pick among the minimum-options cell.
- Validates the board at each step; invalid selections eliminate the player.
- Manages multiple players:
  - Players are stored in a list and also in a binary tree built from a sorted array.
  - The traversal processes players in order.
- Outputs winners and their final boards to a file named `winner_file`.

## Build

## Visual Studio (Windows)
Create a new Console C project.
Add main.c, sudoku_core.c, and the header include/sudoku.h.
Ensure the compiler’s include path contains the include/ folder.
Build the project to produce the executable.

---

##Program flow:

Enter the number of players.
For each player, input the name. A random, partially filled board is created.
The engine performs constraint-based steps.
If a cell has only one possible digit, it is filled automatically.
If the board gets stuck, the program asks the user to choose a value for the cell with the fewest options.
An invalid choice (causing a duplicate) eliminates the player.
Completed boards are added to the winners list.
At the end, the file winner_file contains each winner’s name and final board.

---

## Notes
- Board size is defined by BOARD_SIZE in sudoku.h and is set to 9.
- Random boards: createRandomBoard uses randomness to pick initial filled cells and values that keep the board valid.

File output: winners are appended to winner_file in a simple ASCII layout.
