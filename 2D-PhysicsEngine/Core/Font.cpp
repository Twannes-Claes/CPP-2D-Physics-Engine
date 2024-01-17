#include "Font.h"

#include <iostream>

#include "PhysicsEngine.h"

uint32_t Font::m_Count = 0;

Font::Font(const char* fontName, const uint32_t fontSize, const SDL_Color color, const int x, const int y, SDL_Renderer* pRenderer):
 m_Color(color),
 m_X(x),
 m_Y(y),
 m_pText(" "),
 m_pRenderer(pRenderer)
{
    ++m_Count;

    //Initialize font only on the first instance of a font object
    if(m_Count == 1)
    {
        //Initialize Font library
        if (TTF_Init() == -1)
        {
            std::cout << "SDL_ttf initialization failed: " << TTF_GetError() << '\n';
        }
        else
        {
            std::cout << "SDL Font initialized" << TTF_GetError() << '\n';
        }
    }

    //Load font for FPS
    //m_pFont = TTF_OpenFont("ARCADECLASSIC.TTF", 24);
    m_pFont = TTF_OpenFont(fontName, static_cast<int>(fontSize));

    if (!m_pFont)
    {
        std::cout << "TTF_OpenFont error: " << TTF_GetError() << '\n';

        throw std::runtime_error("Font name not correct");
    }

    //Assign text/color and texture
    m_pFontSurface = TTF_RenderText_Solid(m_pFont, m_pText, m_Color);

    if (!m_pFontSurface)
    {
        throw std::runtime_error("Fontsurfacen not loaded correctly");
    }

    m_pFontTex = SDL_CreateTextureFromSurface(pRenderer, m_pFontSurface);
}

Font::~Font()
{
    //Delete font
    SDL_FreeSurface(m_pFontSurface);
    SDL_DestroyTexture(m_pFontTex);
    TTF_CloseFont(m_pFont);

    --m_Count;

    if (m_Count != 0) return;

    TTF_Quit();
}

void Font::SetText(const char* text)
{
    m_pText = text;

    m_pFontSurface = TTF_RenderText_Solid(m_pFont, m_pText, m_Color);
    m_pFontTex = SDL_CreateTextureFromSurface(m_pRenderer, m_pFontSurface);
}

void Font::SetColor(const SDL_Color color)
{
    m_Color = color;

    m_pFontSurface = TTF_RenderText_Solid(m_pFont, m_pText, m_Color);
    m_pFontTex = SDL_CreateTextureFromSurface(m_pRenderer, m_pFontSurface);
}

void Font::Draw() const
{
    //Draw Font
    const SDL_Rect textRect = { m_X, m_Y, m_pFontSurface->w, m_pFontSurface->h };
    SDL_RenderCopy(m_pRenderer, m_pFontTex, nullptr, &textRect);
}
