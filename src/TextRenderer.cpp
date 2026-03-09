//
// Project Morpheus
// Copyright (C) 2026 Alessio Saltarin
//
// This software is licensed under MIT license.
// See LICENSE.
//

#include "TextRenderer.h"
#include <vector>
#include <cctype>

namespace {
    // Simple 5x7 bitmap font definitions using line segments
    struct CharData {
        std::vector<std::pair<SDL_FPoint, SDL_FPoint>> lines;
        float width;
    };

    CharData getCharData(char c) {
        CharData data;
        c = std::toupper(c);

        switch (c) {
            case 'A':
                data.lines = {
                    {{1, 5}, {0, 2}}, {{0, 2}, {1, 0}}, {{1, 0}, {2, 2}}, {{2, 2}, {3, 5}},
                    {{0.5f, 3}, {2.5f, 3}}
                };
                data.width = 4;
                break;
            case 'B':
                data.lines = {
                    {{0, 0}, {0, 5}}, {{0, 0}, {2, 0}}, {{2, 0}, {2.5f, 0.5f}},
                    {{2.5f, 0.5f}, {2.5f, 2}}, {{2.5f, 2}, {2, 2.5f}}, {{2, 2.5f}, {0, 2.5f}},
                    {{2, 2.5f}, {2.5f, 3}}, {{2.5f, 3}, {2.5f, 4.5f}}, {{2.5f, 4.5f}, {2, 5}},
                    {{2, 5}, {0, 5}}
                };
                data.width = 4;
                break;
            case 'C':
                data.lines = {
                    {{3, 1}, {2, 0}}, {{2, 0}, {1, 0}}, {{1, 0}, {0, 1}},
                    {{0, 1}, {0, 4}}, {{0, 4}, {1, 5}}, {{1, 5}, {2, 5}}, {{2, 5}, {3, 4}}
                };
                data.width = 4;
                break;
            case 'D':
                data.lines = {
                    {{0, 0}, {0, 5}}, {{0, 0}, {2, 0}}, {{2, 0}, {3, 1}},
                    {{3, 1}, {3, 4}}, {{3, 4}, {2, 5}}, {{2, 5}, {0, 5}}
                };
                data.width = 4;
                break;
            case 'E':
                data.lines = {
                    {{0, 0}, {0, 5}}, {{0, 0}, {3, 0}}, {{0, 2.5f}, {2.5f, 2.5f}}, {{0, 5}, {3, 5}}
                };
                data.width = 4;
                break;
            case 'F':
                data.lines = {
                    {{0, 0}, {0, 5}}, {{0, 0}, {3, 0}}, {{0, 2.5f}, {2.5f, 2.5f}}
                };
                data.width = 4;
                break;
            case 'G':
                data.lines = {
                    {{3, 1}, {2, 0}}, {{2, 0}, {1, 0}}, {{1, 0}, {0, 1}},
                    {{0, 1}, {0, 4}}, {{0, 4}, {1, 5}}, {{1, 5}, {2, 5}},
                    {{2, 5}, {3, 4}}, {{3, 4}, {3, 2.5f}}, {{3, 2.5f}, {2, 2.5f}}
                };
                data.width = 4;
                break;
            case 'H':
                data.lines = {
                    {{0, 0}, {0, 5}}, {{0, 2.5f}, {3, 2.5f}}, {{3, 0}, {3, 5}}
                };
                data.width = 4;
                break;
            case 'I':
                data.lines = {
                    {{0, 0}, {2, 0}}, {{1, 0}, {1, 5}}, {{0, 5}, {2, 5}}
                };
                data.width = 3;
                break;
            case 'J':
                data.lines = {
                    {{0, 0}, {3, 0}}, {{2, 0}, {2, 4}}, {{2, 4}, {1, 5}}, {{1, 5}, {0, 4}}
                };
                data.width = 4;
                break;
            case 'K':
                data.lines = {
                    {{0, 0}, {0, 5}}, {{3, 0}, {0, 2.5f}}, {{0.5f, 2.5f}, {3, 5}}
                };
                data.width = 4;
                break;
            case 'L':
                data.lines = {
                    {{0, 0}, {0, 5}}, {{0, 5}, {3, 5}}
                };
                data.width = 4;
                break;
            case 'M':
                data.lines = {
                    {{0, 5}, {0, 0}}, {{0, 0}, {2, 2}}, {{2, 2}, {4, 0}}, {{4, 0}, {4, 5}}
                };
                data.width = 5;
                break;
            case 'N':
                data.lines = {
                    {{0, 5}, {0, 0}}, {{0, 0}, {3, 5}}, {{3, 5}, {3, 0}}
                };
                data.width = 4;
                break;
            case 'O':
                data.lines = {
                    {{1, 0}, {0, 1}}, {{0, 1}, {0, 4}}, {{0, 4}, {1, 5}},
                    {{1, 5}, {2, 5}}, {{2, 5}, {3, 4}}, {{3, 4}, {3, 1}},
                    {{3, 1}, {2, 0}}, {{2, 0}, {1, 0}}
                };
                data.width = 4;
                break;
            case 'P':
                data.lines = {
                    {{0, 5}, {0, 0}}, {{0, 0}, {2, 0}}, {{2, 0}, {3, 1}},
                    {{3, 1}, {3, 2}}, {{3, 2}, {2, 3}}, {{2, 3}, {0, 3}}
                };
                data.width = 4;
                break;
            case 'Q':
                data.lines = {
                    {{1, 0}, {0, 1}}, {{0, 1}, {0, 4}}, {{0, 4}, {1, 5}},
                    {{1, 5}, {2, 5}}, {{2, 5}, {3, 4}}, {{3, 4}, {3, 1}},
                    {{3, 1}, {2, 0}}, {{2, 0}, {1, 0}}, {{2, 3.5f}, {3.5f, 5.5f}}
                };
                data.width = 4.5f;
                break;
            case 'R':
                data.lines = {
                    {{0, 5}, {0, 0}}, {{0, 0}, {2, 0}}, {{2, 0}, {3, 1}},
                    {{3, 1}, {3, 2}}, {{3, 2}, {2, 3}}, {{2, 3}, {0, 3}}, {{2, 3}, {3, 5}}
                };
                data.width = 4;
                break;
            case 'S':
                data.lines = {
                    {{3, 1}, {2, 0}}, {{2, 0}, {1, 0}}, {{1, 0}, {0, 1}},
                    {{0, 1}, {0, 2}}, {{0, 2}, {1, 2.5f}}, {{1, 2.5f}, {2, 2.5f}},
                    {{2, 2.5f}, {3, 3.5f}}, {{3, 3.5f}, {3, 4}}, {{3, 4}, {2, 5}},
                    {{2, 5}, {1, 5}}, {{1, 5}, {0, 4}}
                };
                data.width = 4;
                break;
            case 'T':
                data.lines = {
                    {{0, 0}, {4, 0}}, {{2, 0}, {2, 5}}
                };
                data.width = 5;
                break;
            case 'U':
                data.lines = {
                    {{0, 0}, {0, 4}}, {{0, 4}, {1, 5}}, {{1, 5}, {2, 5}},
                    {{2, 5}, {3, 4}}, {{3, 4}, {3, 0}}
                };
                data.width = 4;
                break;
            case 'V':
                data.lines = {
                    {{0, 0}, {1.5f, 5}}, {{1.5f, 5}, {3, 0}}
                };
                data.width = 4;
                break;
            case 'W':
                data.lines = {
                    {{0, 0}, {1, 5}}, {{1, 5}, {2, 3}}, {{2, 3}, {3, 5}}, {{3, 5}, {4, 0}}
                };
                data.width = 5;
                break;
            case 'X':
                data.lines = {
                    {{0, 0}, {3, 5}}, {{3, 0}, {0, 5}}
                };
                data.width = 4;
                break;
            case 'Y':
                data.lines = {
                    {{0, 0}, {1.5f, 2.5f}}, {{3, 0}, {1.5f, 2.5f}}, {{1.5f, 2.5f}, {1.5f, 5}}
                };
                data.width = 4;
                break;
            case 'Z':
                data.lines = {
                    {{0, 0}, {3, 0}}, {{3, 0}, {0, 5}}, {{0, 5}, {3, 5}}
                };
                data.width = 4;
                break;
            case '0':
                data.lines = {
                    {{1, 0}, {0, 1}}, {{0, 1}, {0, 4}}, {{0, 4}, {1, 5}},
                    {{1, 5}, {2, 5}}, {{2, 5}, {3, 4}}, {{3, 4}, {3, 1}},
                    {{3, 1}, {2, 0}}, {{2, 0}, {1, 0}}, {{0.5f, 1}, {2.5f, 4}}
                };
                data.width = 4;
                break;
            case '1':
                data.lines = {
                    {{1, 1}, {1.5f, 0}}, {{1.5f, 0}, {1.5f, 5}}, {{0, 5}, {3, 5}}
                };
                data.width = 4;
                break;
            case '2':
                data.lines = {
                    {{0, 1}, {1, 0}}, {{1, 0}, {2, 0}}, {{2, 0}, {3, 1}},
                    {{3, 1}, {3, 2}}, {{3, 2}, {0, 5}}, {{0, 5}, {3, 5}}
                };
                data.width = 4;
                break;
            case '3':
                data.lines = {
                    {{0, 0}, {3, 0}}, {{3, 0}, {3, 2}},
                    {{3, 2}, {2, 2.5f}}, {{2, 2.5f}, {3, 3}}, {{3, 3}, {3, 5}}, {{3, 5}, {0, 5}}
                };
                data.width = 4;
                break;
            case '4':
                data.lines = {
                    {{2.5f, 0}, {2.5f, 5}}, {{2.5f, 0}, {0, 3}}, {{0, 3}, {3, 3}}
                };
                data.width = 4;
                break;
            case '5':
                data.lines = {
                    {{3, 0}, {0, 0}}, {{0, 0}, {0, 2.5f}}, {{0, 2.5f}, {2, 2.5f}},
                    {{2, 2.5f}, {3, 3.5f}}, {{3, 3.5f}, {3, 4.5f}}, {{3, 4.5f}, {2, 5}},
                    {{2, 5}, {0, 5}}
                };
                data.width = 4;
                break;
            case '6':
                data.lines = {
                    {{2, 0}, {1, 0}}, {{1, 0}, {0, 1}}, {{0, 1}, {0, 4}},
                    {{0, 4}, {1, 5}}, {{1, 5}, {2, 5}}, {{2, 5}, {3, 4}},
                    {{3, 4}, {3, 3}}, {{3, 3}, {2, 2.5f}}, {{2, 2.5f}, {0, 2.5f}}
                };
                data.width = 4;
                break;
            case '7':
                data.lines = {
                    {{0, 0}, {3, 0}}, {{3, 0}, {1.5f, 5}}
                };
                data.width = 4;
                break;
            case '8':
                data.lines = {
                    {{1, 0}, {2, 0}}, {{2, 0}, {3, 1}},
                    {{3, 1}, {3, 2}}, {{3, 2}, {2, 2.5f}}, {{2, 2.5f}, {3, 3}},
                    {{3, 3}, {3, 4}}, {{3, 4}, {2, 5}}, {{2, 5}, {1, 5}},
                    {{1, 5}, {0, 4}}, {{0, 4}, {0, 3}}, {{0, 3}, {1, 2.5f}},
                    {{1, 2.5f}, {0, 2}}, {{0, 2}, {0, 1}}, {{0, 1}, {1, 0}}
                };
                data.width = 4;
                break;
            case '9':
                data.lines = {
                    {{3, 2.5f}, {0, 2.5f}}, {{0, 2.5f}, {0, 1}}, {{0, 1}, {1, 0}},
                    {{1, 0}, {2, 0}}, {{2, 0}, {3, 1}}, {{3, 1}, {3, 4}},
                    {{3, 4}, {2, 5}}, {{2, 5}, {1, 5}}
                };
                data.width = 4;
                break;
            case ':':
                data.lines = {
                    {{0.5f, 1.5f}, {0.5f, 2}}, {{0.5f, 3.5f}, {0.5f, 4}}
                };
                data.width = 2;
                break;
            case ' ':
                data.width = 3;
                break;
            default:
                data.width = 4;
                break;
        }

        return data;
    }
}

void TextRenderer::renderChar(SDL_Renderer* renderer, char c, float x, float y, float scale) {
    CharData data = getCharData(c);

    for (const auto& line : data.lines) {
        SDL_RenderLine(renderer,
                      x + line.first.x * scale,
                      y + line.first.y * scale,
                      x + line.second.x * scale,
                      y + line.second.y * scale);
    }
}

float TextRenderer::getCharWidth(char c) {
    return getCharData(c).width;
}

float TextRenderer::getTextWidth(const std::string& text, float scale) {
    float width = 0;
    for (size_t i = 0; i < text.length(); ++i) {
        width += getCharWidth(text[i]) * scale;
        if (i < text.length() - 1) {
            width += scale; // spacing between characters
        }
    }
    return width;
}

void TextRenderer::renderText(SDL_Renderer* renderer,
                              const std::string& text,
                              float x,
                              float y,
                              float scale,
                              const Colors::Color& color,
                              Alignment alignment) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    float startX = x;

    if (alignment == Alignment::CENTER) {
        startX = x - getTextWidth(text, scale) / 2.0f;
    } else if (alignment == Alignment::RIGHT) {
        startX = x - getTextWidth(text, scale);
    }

    float currentX = startX;

    for (char c : text) {
        renderChar(renderer, c, currentX, y, scale);
        currentX += getCharWidth(c) * scale + scale; // character width + spacing
    }
}



