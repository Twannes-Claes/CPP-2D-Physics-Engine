#pragma once
#include <cstdint>

#include <SDL_pixels.h>
#include <SDL_ttf.h>

struct SDL_Surface;
struct SDL_Texture;

class Font
{
public:

	Font(const char* fontName, const uint32_t fontSize, const SDL_Color color, const int x, const int y, SDL_Renderer* pRenderer);
	~Font();

	Font(const Font& other) = delete;
	Font(Font&& other) = delete;
	Font& operator=(const Font& other) = delete;
	Font& operator=(Font&& other) = delete;

	void SetText(const char* text);
	void SetColor(const SDL_Color color);

	void Draw() const;

private:

	SDL_Color m_Color;
	const int m_X;
	const int m_Y;

	const char* m_pText;

	SDL_Surface* m_pFontSurface = nullptr;
	SDL_Texture* m_pFontTex = nullptr;
	TTF_Font* m_pFont = nullptr;

	SDL_Renderer* m_pRenderer;

	static uint32_t m_Count;
};

