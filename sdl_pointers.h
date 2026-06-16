//
// Created by blomq on 2026-06-16.
//

#ifndef MINESWEEPER_SDL_POINTERS_H
#define MINESWEEPER_SDL_POINTERS_H
#include <memory>

#include "SDL3/SDL.h"

struct WindowDeleter
{
    void operator()(SDL_Window *window) const
    {
        SDL_DestroyWindow(window);
    }
};

struct RenderDeleter
{
    void operator()(SDL_Renderer *renderer) const
    {
        SDL_DestroyRenderer(renderer);
    }
};

struct TextureDeleter
{
    void operator()(SDL_Texture *texture) const
    {
        SDL_DestroyTexture(texture);
    }
};

struct SurfaceDeleter
{
    void operator()(SDL_Surface *surface) const
    {
        SDL_DestroySurface(surface);
    }
};

using WindowPtr = std::unique_ptr<SDL_Window, WindowDeleter>;
using RenderPtr = std::unique_ptr<SDL_Renderer, RenderDeleter>;
using TexturePtr = std::unique_ptr<SDL_Texture, TextureDeleter>;
using SurfacePtr = std::unique_ptr<SDL_Surface, SurfaceDeleter>;

#endif //MINESWEEPER_SDL_POINTERS_H
