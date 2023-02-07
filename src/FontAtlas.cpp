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

#include <algorithm>
#include <cstring>
#include <stdexcept>
#include <iostream>

#ifdef DEBUG
    #include <cassert>
#endif

#ifdef SAVE_STB
    #include <stb_image_write.h>
#endif

#include "FontAtlas.h"
#include "common.h"


FontAtlas::FontAtlas(){
    if (FT_Init_FreeType(&m_ft)){
        std::cerr << "FontAtlas::FontAtlas: Freetype error - could not initialize the FreeType" 
                  << std::endl;
    }
    m_atlas_size = 512;
    m_atlas.reset(nullptr);
    m_font_height = 0;
}


FontAtlas::FontAtlas(uint atlas_size){
#ifdef DEBUG
    assert(atlas_size);
    assert(!(atlas_size & (atlas_size - 1))); // atlas size must be a power of 2
#endif // DEBUG
    if (FT_Init_FreeType(&m_ft)){
        std::cerr << "FontAtlas::FontAtlas:Freetype error - could not initialize FreeType" 
                  << std::endl;
    }
    m_atlas_size = atlas_size;
    m_atlas.reset(nullptr);
    m_font_height = 0;
}


FontAtlas::~FontAtlas(){
    if(m_ft)
        FT_Done_FreeType(m_ft);
}


uint FontAtlas::loadFont(const char* path, int size){
#ifdef DEBUG
    assert(path);
    assert(size > 0);
#endif // DEBUG
    if (FT_New_Face(m_ft, path, 0, &m_face)){
#ifdef DEBUG
        std::cout << "FontAtlas::loadFont: Freetype error: failed to load font " << path
                  << std::endl;
#endif // DEBUG
        return EXIT_FAILURE;
    }
    FT_Set_Pixel_Sizes(m_face, 0, size);
    loadCharacter(0); // load default

    return EXIT_SUCCESS;
}


bool comparator(const character& a, const character& b){
    return a.height > b.height;
}


uint FontAtlas::loadCharacterRange(uint start, uint end){
    uint glyph_index, index = 0;
    uint failed = 0;

    if(!m_characters_vec.empty())
        index = m_characters_vec.size();

    for(uint i=start; i<=end; i++){
        glyph_index = FT_Get_Char_Index(m_face, i);

        if(!FT_Load_Glyph(m_face, glyph_index, FT_LOAD_RENDER) && glyph_index){
            m_characters_vec.push_back(character());
            m_characters_vec.at(index).code = i;
            m_characters_vec.at(index).glyph_index = glyph_index;
            m_characters_vec.at(index).width = m_face->glyph->bitmap.width;
            m_characters_vec.at(index).height = m_face->glyph->bitmap.rows;
            m_characters_vec.at(index).bearing_x = m_face->glyph->bitmap_left;
            m_characters_vec.at(index).bearing_y = m_face->glyph->bitmap_top;
            m_characters_vec.at(index).advance_x = m_face->glyph->advance.x;
            m_characters_vec.at(index).advance_y = m_face->glyph->advance.y;
            index++;
        }
        else{
            failed++;
        }
    }
    return failed;
}


uint FontAtlas::loadCharacter(uint code){
    uint glyph_index, res, index = 0;

    if(!m_characters_vec.empty())
        index = m_characters_vec.size();

    glyph_index = FT_Get_Char_Index(m_face, code);
    res = FT_Load_Glyph(m_face, glyph_index, FT_LOAD_RENDER);

    if((!res)){
        m_characters_vec.push_back(character());
        m_characters_vec.at(index).code = code;
        m_characters_vec.at(index).glyph_index = glyph_index;
        m_characters_vec.at(index).width = m_face->glyph->bitmap.width;
        m_characters_vec.at(index).height = m_face->glyph->bitmap.rows;
        m_characters_vec.at(index).bearing_x = m_face->glyph->bitmap_left;
        m_characters_vec.at(index).bearing_y = m_face->glyph->bitmap_top;
        m_characters_vec.at(index).advance_x = m_face->glyph->advance.x;
        m_characters_vec.at(index).advance_y = m_face->glyph->advance.y;

        return 0;
    }
    else{
        return 1;
    }
}


bool FontAtlas::createAtlas(bool save_png){ // check error codes
    UNUSED(save_png);
    uint glyph_index, pen_x = 1, pen_y = 1, max_heigth_row;
    bool failed = false;
    m_atlas.reset(new unsigned char[m_atlas_size*m_atlas_size]);

    m_font_height = m_face->size->metrics.ascender - m_face->size->metrics.descender;

    std::sort(m_characters_vec.begin(), m_characters_vec.end(), comparator);

    //let's create the atlas
    std::memset(m_atlas.get(), 0, m_atlas_size*m_atlas_size);

    max_heigth_row = m_characters_vec[0].height;

    for(uint i=0; i<m_characters_vec.size(); i++){
        if(pen_x + m_characters_vec[i].width + 2 > m_atlas_size){
            pen_y += max_heigth_row + 2; // new "row"
            max_heigth_row = m_characters_vec[i].height;
            pen_x = 0; // should this be one?
            if(pen_y + max_heigth_row + 2 > m_atlas_size){ // out of space
                failed = true;
                break;
            } // if there's no space left for the next row we stop
        }

        m_characters_vec[i].tex_x_min = (float)pen_x / m_atlas_size;
        m_characters_vec[i].tex_x_max = (float)(pen_x + m_characters_vec[i].width) / m_atlas_size;
        m_characters_vec[i].tex_y_min = (float)pen_y / m_atlas_size;
        m_characters_vec[i].tex_y_max = (float)(pen_y  + m_characters_vec[i].height) / m_atlas_size;

        glyph_index = FT_Get_Char_Index(m_face, m_characters_vec[i].code);
        FT_Load_Glyph(m_face, glyph_index, FT_LOAD_RENDER);

        for(int j=0; j<m_characters_vec[i].height; j++){
            std::copy(&m_face->glyph->bitmap.buffer[m_characters_vec[i].width * j],
                      &m_face->glyph->bitmap.buffer[m_characters_vec[i].width * (j+1)],
                      m_atlas.get() + pen_y * m_atlas_size + pen_x + m_atlas_size * j);
        }
        pen_x += m_characters_vec[i].width + 2;
    }

    for(uint i=0; i<m_characters_vec.size(); i++)
        m_characters[m_characters_vec[i].code] = m_characters_vec[i];

#ifdef SAVE_STB
    if(save_png)
        if(!stbi_write_png("data/atlas.png", m_atlas_size,
            m_atlas_size, 1, m_atlas.get(), m_atlas_size))
            std::cerr << "FontAtlas::createAtlas: failed to save atlas" << std::endl;
#else // SAVE_STB
    if(save_png)
        std::cerr << "FontAtlas::createAtlas: can't save atlas because I was not built with "
                  << "stb :( (do \"export SAVE_STB=1\" before compiling and make sure "
                  << "stb_image_write.h is under include/)" << std::endl;

#endif // SAVE_STB
    return failed;
}


int FontAtlas::getCharacter(uint code, const character** the_character) const{
#ifdef DEBUG
    assert(the_character);
#endif // DEBUG
    try{
        *the_character = &m_characters.at(code);
    }
    catch(const std::out_of_range& e){
        // if no character matches it returns the null character.
        *the_character = &m_characters.at(0);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}


int FontAtlas::getKerning(uint code1, uint code2) const{
    // TODO
    UNUSED(code1);
    UNUSED(code2);
    return 0;
}


const unsigned char* FontAtlas::getAtlas() const{
    return m_atlas.get();
}


uint FontAtlas::getAtlasSize() const{
    return m_atlas_size;
}


int FontAtlas::getHeight() const{
    return m_font_height;
}

