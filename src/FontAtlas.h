/*
 * Copyright (C) 2023 Sergi Garcia Bordils
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software Foundation, either version 3 of the 
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program. If not,
 * see <https://www.gnu.org/licenses/>. 
 *
 */


#ifndef FONT_ATLAS_H
#define FONT_ATLAS_H

#include <ft2build.h>
#include FT_FREETYPE_H

#include <vector>
#include <unordered_map>
#include <memory>


struct character{
    uint code; // unicode value
    uint glyph_index; // freetype index

    int width;        //size x
    int height;       //size y 
    int bearing_x;
    int bearing_y;
    int advance_x;
    int advance_y;

    // normalized texture coordinates
    float tex_x_min;
    float tex_x_max;
    float tex_y_min;
    float tex_y_max;
};


class FontAtlas{
    private:
        FT_Library m_ft;
        FT_Face m_face;

        uint m_atlas_size;
        int m_font_height;
        std::unique_ptr<unsigned char[]> m_atlas;
        std::unordered_map<int, struct character> m_characters;
        std::vector<struct character> m_characters_vec;
    public:
        FontAtlas();
        FontAtlas(uint atlas_size);
        ~FontAtlas();
        uint loadFont(const char* path, int size);

        uint loadCharacterRange(uint start, uint end);
        uint loadCharacter(uint code);
        bool createAtlas(bool save_png);

        int getCharacter(uint code, const character** the_character) const;
        int getKerning(uint code1, uint code2) const;
        uint getAtlasSize() const;
        int getHeight() const;
        const unsigned char* getAtlas() const;

        // new function to calculate the width of a string?
};


#endif
