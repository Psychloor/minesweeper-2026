//
// Created by blomq on 2026-06-16.
//

#include "minefield.h"

#include <array>
#include <unordered_set>

#include "rng.h"

constexpr std::array ADJACENT_CELLS{
    std::make_pair(-1, -1),
    std::make_pair(-1, 0),
    std::make_pair(-1, 1),
    std::make_pair(0, -1),
    std::make_pair(0, 1),
    std::make_pair(1, -1),
    std::make_pair(1, 0),
    std::make_pair(1, 1)
};

int minMax(const int current, const int min, const int max) {
    return std::min(std::max(current, min), max);
}

Minefield::Minefield(const int width, const int height, const int numMines) : width_(width), height_(height) {
    numMines_ = minMax(numMines, 1, (width * height) >> 1);
    tiles_.reserve(width_ * height_);
    std::generate_n(std::back_inserter(tiles_), width_ * height_, [] { return Tile{}; });
}

void Minefield::openTile(const int xPos, const int yPos) {
    if (isGameOver_) return;
    if (isGameWon_) return;

    if (firstOpen_) {
        placeMines(xPos, yPos);
        countAdjacentMines();
        firstOpen_ = false;
    }

    auto &tile = tiles_[yPos * width_ + xPos];
    if (tile.isFlagged || tile.isOpen) return;

    if (tile.isMine) {
        isGameOver_ = true;
        tile.isOpen = true;
        tile.isQuestionMarked = false;
        explosionPos_ = {static_cast<int>(xPos), static_cast<int>(yPos)};
        openAllMines();
    } else {
        openNearbyTiles(xPos, yPos);
        checkWinCondition();
    }
}

void Minefield::toggleFlag(const int xPos, const int yPos) {
    if (isGameOver_) return;
    if (isGameWon_) return;

    auto &tile = tiles_[yPos * width_ + xPos];

    if (tile.isOpen) {
        return;
    }

    if (!tile.isFlagged && !tile.isQuestionMarked) {
        tile.isFlagged = true;
    } else if (tile.isFlagged) {
        tile.isFlagged = false;
        tile.isQuestionMarked = true;
    } else {
        tile.isQuestionMarked = false;
    }
}

const std::vector<Tile> &Minefield::tiles() const {
    return tiles_;
}

int Minefield::width() const {
    return width_;
}

int Minefield::height() const {
    return height_;
}

int Minefield::numMines() const {
    return numMines_;
}

bool Minefield::isGameOver() const {
    return isGameOver_;
}

bool Minefield::isGameWon() const {
    return isGameWon_;
}

const SDL_Point &Minefield::explosionPos() const {
    return explosionPos_;
}

const Tile &Minefield::at(const int xPos, const int yPos) const {
    return tiles_[yPos * width_ + xPos];
}

void Minefield::placeMines(const int firstX, const int firstY) {
    RNG rng;
    std::uniform_int_distribution mineDistribution(0, width_ * height_ - 1);
    size_t minesToPlace = numMines_;
    while (minesToPlace > 0) {
        const auto tilePos = mineDistribution(rng.generator());
        const auto x = tilePos % width_;
        const auto y = tilePos / width_;
        if (!tiles_[y * width_ + x].isMine && !(x == firstX && y == firstY)) {
            tiles_[y * width_ + x].isMine = true;
            --minesToPlace;
        }
    }
}

void Minefield::countAdjacentMines() {
    for (int i = 0; i < width_ * height_; ++i) {
        const auto x = i % width_;
        const auto y = i / width_;
        if (tiles_[i].isMine) {
            continue;
        }

        tiles_[i].adjacentMines = minesNearTile(x, y);
    }
}

void Minefield::openNearbyTiles(const int xPos, const int yPos) {
    std::vector<std::pair<int, int> > tilesToOpen;
    std::unordered_set<int> visitedTiles{};

    tilesToOpen.emplace_back(xPos, yPos);
    visitedTiles.emplace(yPos * width_ + xPos);

    while (!tilesToOpen.empty()) {
        const auto [tileX, tileY] = tilesToOpen.back();
        tilesToOpen.pop_back();
        auto &currentTile = tiles_[tileY * width_ + tileX];

        if (currentTile.isOpen) {
            continue;
        }
        if (currentTile.isFlagged) {
            continue;
        }
        if (currentTile.isMine) {
            continue;
        }
        currentTile.isOpen = true;
        currentTile.isQuestionMarked = false;
        if (currentTile.adjacentMines > 0) {
            continue;
        }

        for (auto [x, y]: ADJACENT_CELLS) {
            const int neighborX = tileX + x;
            const int neighborY = tileY + y;
            const int neighborIndex = neighborY * width_ + neighborX;

            if (neighborX >= 0 && neighborX < width_ &&
                neighborY >= 0 && neighborY < height_) {
                auto &tile = at(neighborX, neighborY);
                if (tile.isOpen || tile.isFlagged) {
                    continue;
                }

                if (visitedTiles.emplace(neighborIndex).second) {
                    tilesToOpen.emplace_back(neighborX, neighborY);
                }
            }
        }
    }
}

void Minefield::checkWinCondition() {
    for (const auto &tile: tiles_) {
        if (!tile.isMine && !tile.isOpen) {
            return;
        }
    }

    openAllMines();
    isGameWon_ = true;
}

void Minefield::openAllMines() {
    for (auto &tile: tiles_) {
        if (tile.isMine && !tile.isFlagged) {
            tile.isOpen = true;
            tile.isQuestionMarked = false;
        }
    }
}

Tile &Minefield::at(const int xPos, const int yPos) {
    return tiles_[yPos * width_ + xPos];
}

uint8_t Minefield::minesNearTile(const int xPos, const int yPos) const {
    uint8_t minesNear = 0;
    for (auto [x, y]: ADJACENT_CELLS) {
        if (x + xPos >= 0 && x + xPos < width_ && y + yPos >= 0 && y + yPos < height_) {
            if (tiles_[(y + yPos) * width_ + (x + xPos)].isMine) {
                ++minesNear;
            }
        }
    }

    return minesNear;
}
