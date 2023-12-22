#include "Circle.h"

#pragma warning(push)
#pragma warning(disable: 4201) // Disable warning C4201
#include <iostream>

#include "glm/gtx/norm.hpp"
#pragma warning(pop)


Circle::Circle(const float radius) :m_Radius(radius) { m_AngleBetweenVertices = 360.f / static_cast<float>(m_AmountVertices); }

std::unique_ptr<Shape> Circle::Clone() const
{
	return std::make_unique<Circle>(m_Radius);
}

void Circle::DrawShape(SDL_Renderer* pRenderer, const glm::vec2& pos)
{
    SDL_SetRenderDrawColor(pRenderer, 255, 255, 255, 255);

    for (int i{}; i < m_AmountVertices; ++i)
    {
        angles.x = glm::radians(static_cast<float>(i) * m_AngleBetweenVertices);

        xy1.x = pos.x + m_Radius * glm::cos(angles.x);
        xy1.y = pos.y + m_Radius * glm::sin(angles.x);

        angles.y = glm::radians(static_cast<float>(i + 1) * m_AngleBetweenVertices);

        xy2.x = pos.x + m_Radius * glm::cos(angles.y);
        xy2.y = pos.y + m_Radius * glm::sin(angles.y);

        SDL_RenderDrawLineF(pRenderer, xy1.x, xy1.y, xy2.x, xy2.y);
    }
}



//int Circle::DrawLine(SDL_Renderer* renderer, const float x1, const float x2, const float y) const
//{
//	return SDL_RenderDrawLineF(renderer, x1, y, x2, y);;
//}
//
//#pragma warning(push)
//#pragma warning(disable: 4244) // Disable warning C4201
//int Circle::DrawCircle(SDL_Renderer* renderer, float x, float y, int rx, int ry) const
//{
//	int result;
//	int ix, iy;
//	float h, i, j, k;
//	float oh, oi, oj, ok;
//	float xmh, xph;
//	float xmi, xpi;
//	float xmj, xpj;
//	float xmk, xpk;
//
//	/*
//	* Sanity check radii
//	*/
//	if ((rx < 0) || (ry < 0)) {
//		return (-1);
//	}
//
//	/*
//	* Special case for rx=0 - draw a vline
//	*/
//	if (rx == 0) {
//		return (DrawLine(renderer, x, y - ry, y + ry));
//	}
//	/*
//	* Special case for ry=0 - draw a hline
//	*/
//	if (ry == 0) {
//		return (DrawLine(renderer, x - rx, x + rx, y));
//	}
//
//	/*
//	* Set color
//	*/
//	result = 0;
//	result |= SDL_SetRenderDrawBlendMode(renderer, (255 == 255) ? SDL_BLENDMODE_NONE : SDL_BLENDMODE_BLEND);
//	result |= SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
//
//	/*
//	* Init vars
//	*/
//	oh = oi = oj = ok = 0xFFFF;
//
//	/*
//	* Draw
//	*/
//	if (rx > ry) {
//		ix = 0;
//		iy = rx * 64;
//
//		do {
//			h = (ix + 32) >> 6;
//			i = (iy + 32) >> 6;
//			j = (h * ry) / rx;
//			k = (i * ry) / rx;
//
//			if ((ok != k) && (oj != k)) {
//				xph = x + h;
//				xmh = x - h;
//				if (k > 0) {
//					result |= DrawLine(renderer, xmh, xph, y + k);
//					result |= DrawLine(renderer, xmh, xph, y - k);
//				}
//				else {
//					result |= DrawLine(renderer, xmh, xph, y);
//				}
//				ok = k;
//			}
//			if ((oj != j) && (ok != j) && (k != j)) {
//				xmi = x - i;
//				xpi = x + i;
//				if (j > 0) {
//					result |= DrawLine(renderer, xmi, xpi, y + j);
//					result |= DrawLine(renderer, xmi, xpi, y - j);
//				}
//				else {
//					result |= DrawLine(renderer, xmi, xpi, y);
//				}
//				oj = j;
//			}
//
//			ix = ix + iy / rx;
//			iy = iy - ix / rx;
//
//		} while (i > h);
//	}
//	else {
//		ix = 0;
//		iy = ry * 64;
//
//		do {
//			h = (ix + 32) >> 6;
//			i = (iy + 32) >> 6;
//			j = (h * rx) / ry;
//			k = (i * rx) / ry;
//
//			if ((oi != i) && (oh != i)) {
//				xmj = x - j;
//				xpj = x + j;
//				if (i > 0) {
//					result |= DrawLine(renderer, xmj, xpj, y + i);
//					result |= DrawLine(renderer, xmj, xpj, y - i);
//				}
//				else {
//					result |= DrawLine(renderer, xmj, xpj, y);
//				}
//				oi = i;
//			}
//			if ((oh != h) && (oi != h) && (i != h)) {
//				xmk = x - k;
//				xpk = x + k;
//				if (h > 0) {
//					result |= DrawLine(renderer, xmk, xpk, y + h);
//					result |= DrawLine(renderer, xmk, xpk, y - h);
//				}
//				else {
//					result |= DrawLine(renderer, xmk, xpk, y);
//				}
//				oh = h;
//			}
//
//			ix = ix + iy / ry;
//			iy = iy - ix / ry;
//
//		} while (i > h);
//	}
//
//	return (result);
//}
//#pragma warning(pop)
