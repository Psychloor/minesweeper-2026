//
// Created by blomq on 2026-06-16.
//

#ifndef MINESWEEPER_MINEFIELD_H
#define MINESWEEPER_MINEFIELD_H
#include <memory>

#include "SDL3/SDL_rect.h"

struct Tile {
    bool isOpen: 1 = false;
    bool isMine: 1 = false;
    bool isFlagged: 1 = false;
    bool isQuestionMarked: 1 = false;
    uint8_t adjacentMines: 4 = 0;

    explicit Tile() = default;

    Tile(const Tile &other) = delete;

    Tile(Tile &&other) noexcept = delete;

    Tile & operator=(const Tile &other) = delete;

    Tile & operator=(Tile &&other) noexcept = delete;
};

class Minefield {
public:
    explicit Minefield(int width, int height, int numMines);

    ~Minefield() = default;

    Minefield(const Minefield &other) = delete;

    Minefield(Minefield &&other) noexcept = delete;

    Minefield & operator=(const Minefield &other) = delete;

    Minefield & operator=(Minefield &&other) noexcept = delete;

    void openTile(int xPos, int yPos);

    void toggleFlag(int xPos, int yPos) const;

    [[nodiscard]] int width() const;

    [[nodiscard]] int height() const;

    [[nodiscard]] int numMines() const;

    [[nodiscard]] bool isGameOver() const;

    [[nodiscard]] bool isGameWon() const;

    [[nodiscard]] const SDL_Point &explosionPos() const;

    [[nodiscard]] const Tile &at(int xPos, int yPos) const;

private:
    void placeMines(int firstX, int firstY) const;

    void countAdjacentMines() const;

    void openNearbyTiles(int xPos, int yPos);

    void checkWinCondition();

    void openAllMines() const;

    Tile &at(int xPos, int yPos);

    [[nodiscard]] uint8_t minesNearTile(int xPos, int yPos) const;

    int width_;
    int height_;
    int numMines_;
    bool firstOpen_ = true;
    bool isGameOver_ = false;
    bool isGameWon_ = false;
    std::unique_ptr<Tile[]> tiles_ = nullptr;
    SDL_Point explosionPos_{-1, -1};
};

#endif //MINESWEEPER_MINEFIELD_H
