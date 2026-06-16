# Minesweeper

A simple Minesweeper game written in C++ using SDL3.

## Features

- Classic Minesweeper-style gameplay
- Configurable board width, height, and mine count
- The first click is safe
- Flood-fill opening for empty cells
- Flag and question-mark cell states
- Game over and win detection
- Sprite-based rendering with SDL3

## Controls

| Input       | Action                                           |
|-------------|--------------------------------------------------|
| Left click  | Open a cell                                      |
| Right click | Cycle cell mark: flag → question mark → unmarked |
| `N`         | Start a new field/game after game over or win    |
| `Space`     | Start a new field/game after game over or win    |

## Configuration

The game reads settings from `settings.ini`.

Example:
```ini 
width=20
height=20
mines=40
```


### Settings

| Key      | Description           |
|----------|-----------------------|
| `width`  | Board width in cells  |
| `height` | Board height in cells |
| `mines`  | Number of mines       |

The mine count is clamped automatically:

- Minimum: `1`
- Maximum: half of the total cells

For example, with a `20x20` board, there are `400` cells, so the maximum mine count is `200`.

## How to Play

The goal is to open every non-mine cell without clicking on a mine.

- Left-click a cell to open it.
- If the cell contains a mine, the game is over.
- If the cell has no nearby mines, nearby empty cells are opened automatically.
- Right-click a closed cell to mark it.
- Right-click cycles through:
    - Flag
    - Question mark
    - Unmarked

You win when all non-mine cells are opened.

## Notes

- Pressing `N` or `Space` only starts a new game after the current game has ended.
- The first opened cell will not contain a mine.
- The window size is based on the configured board size.
