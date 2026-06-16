#include <iostream>

#include "minefield.h"

#include <SDL3/SDL.h>
#include <ranges>
#include <string>
#include <fstream>
#include <sstream>

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>

constexpr int TileSize = 24;
constexpr float TileTexSize = 16.f;

constexpr auto TITLE_REGULAR = "Minesweeper";
constexpr auto TITLE_GAME_OVER = "Minesweeper - Game Over";
constexpr auto TITLE_GAME_WON = "Minesweeper - You Win!";

struct AppContext {
    Minefield *minefield{nullptr};
    SDL_Window *window{nullptr};
    SDL_Renderer *renderer{nullptr};
    SDL_Texture *texture{nullptr};
    SDL_Surface *iconSurface{nullptr};
    SDL_Point mouse{};

    int startingWidth = 20;
    int startingHeight = 20;
    int startingMines = 40;
    SDL_FRect *tileSrcRects{nullptr};
    SDL_FRect *tileDstRect{nullptr};
};

void trim(std::string &text) {
    const auto first = text.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
        text.clear();
        return;
    }

    const auto last = text.find_last_not_of(" \t\r\n");
    text = text.substr(first, last - first + 1);
}

void parseSettingsFile(AppContext &context) {
    std::ifstream file("settings.ini");
    if (!file) {
        std::cout << "No settings.ini found, using defaults." << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        trim(line);

        if (line.empty()) {
            continue;
        }

        if (line.starts_with('#') || line.starts_with(';')) {
            continue;
        }

        const auto equalsPos = line.find('=');

        if (equalsPos == std::string::npos) {
            continue;
        }

        std::string key = line.substr(0, equalsPos);
        std::string value = line.substr(equalsPos + 1);

        trim(key);
        trim(value);

        try {
            if (key == "width") {
                context.startingWidth = std::stoi(value);
            } else if (key == "height") {
                context.startingHeight = std::stoi(value);
            } else if (key == "mines") {
                context.startingMines = std::stoi(value);
            }
        } catch ([[maybe_unused]] const std::exception &exception) {
            std::cout << "Invalid value in settings.ini: " << line << std::endl;
        }
    }
}

SDL_FRect tileTexFRect(const Tile &tile, const bool explosionPoint, const bool isGameOver) {
    SDL_FRect tileRect{
        .x = 1,
        .y = 2,
        .w = TileTexSize,
        .h = TileTexSize
    };

    if (tile.isOpen) {
        if (tile.adjacentMines > 0) {
            const int numberIndex = tile.adjacentMines - 1;
            tileRect.x = static_cast<float>(numberIndex % 4);
            tileRect.y = SDL_floorf(static_cast<float>(numberIndex) / 4);
        } else if (tile.isMine) {
            tileRect.x = explosionPoint ? 3 : 2;
            tileRect.y = 3;
        } else if (tile.isQuestionMarked) {
            tileRect.x = 0;
            tileRect.y = 3;
        } else {
            tileRect.x = 0;
            tileRect.y = 2;
        }
    } else {
        if (tile.isFlagged) {
            tileRect.x = 2;
            tileRect.y = 2;

            if (isGameOver && !tile.isMine) {
                tileRect.x = 3;
            }
        } else if (tile.isQuestionMarked) {
            tileRect.x = 1;
            tileRect.y = 3;
        }
    }

    tileRect.x *= TileTexSize;
    tileRect.y *= TileTexSize;

    return tileRect;
}

void updateTileSrcRects(const AppContext *context) {
    const auto &explosionPos = context->minefield->explosionPos();
    const auto *minefield = context->minefield;

    for (int i = 0; i < minefield->width() * context->minefield->height(); ++i) {
        const int x = i % minefield->width();
        const int y = i / minefield->width();
        const auto &tile = minefield->at(x, y);

        context->tileSrcRects[i] = tileTexFRect(tile, explosionPos.x == x && explosionPos.y == y,
                                                context->minefield->isGameOver());
        context->tileDstRect[i] = {
            .x = static_cast<float>(x * TileSize), .y = static_cast<float>(y * TileSize),
            .w = static_cast<float>(TileSize), .h = static_cast<float>(TileSize)
        };
    }
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
    SDL_SetAppMetadata(TITLE_REGULAR, "1.0", nullptr);

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    auto *context = new AppContext();
    *appstate = context;
    parseSettingsFile(*context);

    if (!SDL_CreateWindowAndRenderer(TITLE_REGULAR, context->startingWidth * TileSize,
                                     context->startingHeight * TileSize, 0,
                                     &context->window,
                                     &context->renderer)) {
        SDL_Log("SDL_CreateWindowAndRenderer failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_SetRenderVSync(context->renderer, 1);

    context->iconSurface = SDL_LoadPNG("assets/icon.png");
    if (!context->iconSurface) {
        SDL_Log("SDL_LoadPNG failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_SetWindowIcon(context->window, context->iconSurface);

    SDL_Surface *surface = SDL_LoadPNG("assets/tiles.png");
    if (!surface) {
        SDL_Log("SDL_LoadPNG failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    context->texture = SDL_CreateTextureFromSurface(context->renderer, surface);
    SDL_DestroySurface(surface);
    if (!context->texture) {
        SDL_Log("SDL_CreateTextureFromSurface failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    context->minefield = new Minefield(context->startingWidth, context->startingHeight, context->startingMines);
    if (!context->minefield) {
        SDL_Log("Failed to create minefield");
        return SDL_APP_FAILURE;
    }
    context->tileSrcRects = new SDL_FRect[context->minefield->width() * context->minefield->height()];
    context->tileDstRect = new SDL_FRect[context->minefield->width() * context->minefield->height()];
    updateTileSrcRects(context);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    const auto *context = static_cast<AppContext *>(appstate);

    const auto *minefield = context->minefield;
    auto *renderer = context->renderer;
    auto *texture = context->texture;
    const auto width = minefield->width();
    const auto height = minefield->height();

    // Background
    SDL_SetRenderDrawColor(renderer, 100, 149, 237, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int i = 0; i < width * height; ++i) {
        SDL_RenderTexture(renderer, texture, &context->tileSrcRects[i], &context->tileDstRect[i]);
    }

    // Cursor
    const auto mouseRect = SDL_FRect{
        .x = static_cast<float>(context->mouse.x * TileSize), .y = static_cast<float>(context->mouse.y * TileSize), // NOLINT(*-integer-division)
        .w = static_cast<float>(TileSize), .h = static_cast<float>(TileSize)
    };

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 160);
    SDL_RenderRect(renderer, &mouseRect);

    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    auto *context = static_cast<AppContext *>(appstate);

    switch (event->type) {
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            if (event->button.button == SDL_BUTTON_LEFT) {
                context->minefield->openTile(context->mouse.x, context->mouse.y);
                updateTileSrcRects(context);
                if (context->minefield->isGameOver()) {
                    SDL_SetWindowTitle(context->window, TITLE_GAME_OVER);
                    return SDL_APP_CONTINUE;
                }
                if (context->minefield->isGameWon()) {
                    SDL_SetWindowTitle(context->window, TITLE_GAME_WON);
                    return SDL_APP_CONTINUE;
                }
            }
            if (event->button.button == SDL_BUTTON_RIGHT) {
                context->minefield->toggleFlag(context->mouse.x, context->mouse.y);
                updateTileSrcRects(context);
            }
            break;

        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
            return SDL_APP_SUCCESS;

        case SDL_EVENT_MOUSE_MOTION:
            context->mouse.x = static_cast<int>(SDL_floorf(event->motion.x / TileSize));
            context->mouse.y = static_cast<int>(SDL_floorf(event->motion.y / TileSize));
            break;

        case SDL_EVENT_KEY_DOWN:
            if (event->key.key == SDLK_N || event->key.key == SDLK_SPACE) {
                if (!context->minefield->isGameOver() && !context->minefield->isGameWon()) {
                    return SDL_APP_CONTINUE;
                }
                const auto width = context->minefield->width();
                const auto height = context->minefield->height();
                const auto mines = context->minefield->numMines();

                delete context->minefield;
                context->minefield = new Minefield(width, height, mines);
                updateTileSrcRects(context);
                SDL_SetWindowTitle(context->window, TITLE_REGULAR);
            }
            break;
        default: ;
    }

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    if (appstate) {
        const auto *context = static_cast<AppContext *>(appstate);
        delete[] context->tileDstRect;
        delete[] context->tileSrcRects;
        delete context->minefield;
        SDL_DestroySurface(context->iconSurface);
        SDL_DestroyTexture(context->texture);
        SDL_DestroyRenderer(context->renderer);
        SDL_DestroyWindow(context->window);
        delete context;
    }

    SDL_Quit();
}
