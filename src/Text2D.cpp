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


#include <string>
#include <algorithm>
#include <iostream>
#include <cstring>
#include <cmath>
#ifdef DEBUG
    #include <cassert>
#endif // DEBUG

#include <GL/glew.h>

#include "Text2D.h"
#include "FontAtlas.h"
#include "common.h"


Text2D::Text2D(){
    m_init = false;
}


Text2D::Text2D(int fb_width, int fb_height, const FontAtlas* font, GLuint shader){
#ifdef DEBUG
    assert(font);
#endif // DEBUG
    m_font_atlas = font;
    m_num_vertices = 0;
    m_num_indices = 0;
    m_fb_width = fb_width;
    m_fb_height = fb_height;
    m_update_buffer = true;
    m_init = true;
    m_shader = shader;
    m_disp[0] = 0.0f;
    m_disp[1] = 0.0f;
//    m_disp = math::vec2(0.0, 0.0);

    initgl();
}


void Text2D::initgl(){
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vbo_vert);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vert);
    glVertexAttribPointer(0, 2,  GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &m_vbo_tex);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_tex);
    glVertexAttribPointer(1, 2,  GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &m_vbo_col);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_col);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(2);

    glGenBuffers(1, &m_vbo_ind);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo_ind);

    m_disp_location = glGetUniformLocation(m_shader, "disp");
}


Text2D::~Text2D(){
    if(m_init){
        glDeleteBuffers(1, &m_vbo_vert);
        glDeleteBuffers(1, &m_vbo_tex);
        glDeleteBuffers(1, &m_vbo_ind);
        glDeleteBuffers(1, &m_vbo_col);
        glDeleteVertexArrays(1, &m_vao);
    }
}


void Text2D::getPenXY(float& pen_x, float& pen_y, struct string* string_){
#ifdef DEBUG
    assert(string_);
#endif // DEBUG
    if(string_->placement == STRING_DRAW_RELATIVE){
        pen_x = std::floor(string_->relative_x * m_fb_width);
        pen_y = std::floor(string_->relative_y * m_fb_height);
    }
    else{
        if(string_->placement == STRING_DRAW_ABSOLUTE_BL || 
           string_->placement == STRING_DRAW_ABSOLUTE_TL){
            pen_x = string_->posx;
        }
        else{
            pen_x = m_fb_width - string_->posx;
        }
        if(string_->placement == STRING_DRAW_ABSOLUTE_BL || 
           string_->placement == STRING_DRAW_ABSOLUTE_BR){
            pen_y = string_->posy;
        }
        else{
            pen_y = m_fb_height - string_->posy;
        }
    }

    if(string_->alignment == STRING_ALIGN_CENTER_X ||
       string_->alignment == STRING_ALIGN_CENTER_XY){
        pen_x -= string_->width/2;
    }
    if(string_->alignment == STRING_ALIGN_CENTER_Y ||
       string_->alignment == STRING_ALIGN_CENTER_XY){
        pen_y += (string_->height/2) - (getFontHeigth() * string_->scale);
    }

    if(string_->alignment == STRING_ALIGN_LEFT){
        pen_x -= string_->width;
    }
}


void Text2D::updateBuffers(){
    uint total_num_characters = 0, acc = 0;
    std::unique_ptr<GLfloat[]> vertex_buffer;
    std::unique_ptr<GLfloat[]> tex_coords_buffer;
    std::unique_ptr<GLfloat[]> color_buffer;
    std::unique_ptr<GLuint[]> index_buffer;
    for(uint i=0; i < m_strings.size(); i++)
        total_num_characters += m_strings.at(i).strlen;

    m_num_vertices = total_num_characters * 4;
    m_num_indices = total_num_characters * 6;

    vertex_buffer.reset(new GLfloat[2 * m_num_vertices]);
    tex_coords_buffer.reset(new GLfloat[2 * m_num_vertices]);
    color_buffer.reset(new GLfloat[4 * m_num_vertices]);
    index_buffer.reset(new GLuint[6 * total_num_characters]);

    for(uint i=0; i < m_strings.size(); i++){
        struct string* current_string = &m_strings.at(i);
        float pen_x = current_string->posx, pen_y = current_string->posy, w, h, xpos, ypos;
        uint index, index_color, disp;
        const character* ch;
        getPenXY(pen_x, pen_y, current_string);
        uint j = 0, k = 0; // k is used to skip the possible line breaks
        while(current_string->textbuffer[j] != '\0'){
            if(current_string->textbuffer[j] == '\n'){
                j++;
                getPenXY(pen_x, pen_y, current_string);
                pen_y -= (getFontHeigth() * current_string->scale) * (j - k);
                continue;
            }

            m_font_atlas->getCharacter(current_string->textbuffer[j], &ch);

            xpos = pen_x + (float)ch->bearing_x * current_string->scale;
            ypos = pen_y - (float)(ch->height - ch->bearing_y) * current_string->scale;
            w = (float)ch->width * current_string->scale;
            h = (float)ch->height * current_string->scale;

            index = (k + acc) * 8;
            vertex_buffer[index] = xpos;
            vertex_buffer[index + 1] = ypos;
            vertex_buffer[index + 2] = xpos;
            vertex_buffer[index + 3] = ypos + h;
            vertex_buffer[index + 4] = xpos + w;
            vertex_buffer[index + 5] = ypos + h;
            vertex_buffer[index + 6] = xpos + w;
            vertex_buffer[index + 7] = ypos;

            tex_coords_buffer[index] = ch->tex_x_min;
            tex_coords_buffer[index + 1] = ch->tex_y_max;
            tex_coords_buffer[index + 2] = ch->tex_x_min;
            tex_coords_buffer[index + 3] = ch->tex_y_min;
            tex_coords_buffer[index + 4] = ch->tex_x_max;
            tex_coords_buffer[index + 5] = ch->tex_y_min;
            tex_coords_buffer[index + 6] = ch->tex_x_max;
            tex_coords_buffer[index + 7] = ch->tex_y_max;

            index_color = (k + acc) * 16;
            std::memcpy(&color_buffer[index_color], current_string->color, sizeof(GLfloat) * 4);
            std::memcpy(&color_buffer[index_color + 4], current_string->color, sizeof(GLfloat) * 4);
            std::memcpy(&color_buffer[index_color + 8], current_string->color, sizeof(GLfloat) * 4);
            std::memcpy(&color_buffer[index_color + 12], current_string->color, sizeof(GLfloat) * 4);

            disp = (k + acc) * 4;
            index = (k + acc) * 6;
            index_buffer[index] = disp;
            index_buffer[index + 1] = disp + 2;
            index_buffer[index + 2] = disp + 1;
            index_buffer[index + 3] = disp;
            index_buffer[index + 4] = disp + 3;
            index_buffer[index + 5] = disp + 2;

            pen_x += (float)(ch->advance_x >> 6) * current_string->scale;

            j++;
            k++;
        }
        acc += current_string->strlen;
    }

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vert);
    glBufferData(GL_ARRAY_BUFFER, 2 * m_num_vertices * sizeof(GLfloat), vertex_buffer.get(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_tex);
    glBufferData(GL_ARRAY_BUFFER, 2 * m_num_vertices * sizeof(GLfloat), tex_coords_buffer.get(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_col);
    glBufferData(GL_ARRAY_BUFFER, 4 * m_num_vertices * sizeof(GLfloat), color_buffer.get(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo_ind);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * total_num_characters * sizeof(GLuint), index_buffer.get(), GL_STATIC_DRAW);
}


void Text2D::render(){
    if(m_update_buffer){
        updateBuffers();
        m_update_buffer = false;
    }
    glUseProgram(m_shader);
    glBindVertexArray(m_vao);

    glUniform2f(m_disp_location, m_disp[0], m_disp[1]);

    m_font_atlas->bindTexture();
    glDrawElements(GL_TRIANGLES, m_num_indices, GL_UNSIGNED_INT, NULL);
}


void Text2D::addString(const wchar_t* string, float relative_x, 
                       float relative_y, float scale, int alignment, float color[3]){
#ifdef DEBUG
    assert(color);
    assert(string);
    assert(relative_x >= 0.0);
    assert(relative_x <= 1.0);
    assert(relative_y >= 0.0);
    assert(relative_y <= 1.0);
    assert(alignment > 0);
    assert(alignment < 6);
#endif // DEBUG
    uint i;

    addString(string, 0, 0, scale, STRING_DRAW_RELATIVE, alignment, color);

    // dirty dirty...
    i = m_strings.size();
    struct string& str = m_strings.at(i-1);
    str.relative_x = relative_x;
    str.relative_y = relative_y;
}


void Text2D::addString(const wchar_t* text, uint x, uint y, 
                       float scale, int placement, int alignment, float color[4]){
#ifdef DEBUG
    assert(color);
    assert(text);
    assert(alignment > 0);
    assert(alignment < 6);
#endif
    const character* ch;
    uint i = m_strings.size(), j = 0, w = 0;

    m_strings.push_back(string());
    struct string& str = m_strings.at(i);

    str.posx = x;
    str.posy = y;
    str.scale = scale;
    str.placement = placement;
    str.alignment = alignment;
    str.width = 0;
    str.height = getFontHeigth() * scale;
    std::memcpy(str.color, color, sizeof(float) * 4);
    wstrcpy(str.textbuffer, text, STRING_MAX_LEN);

    str.strlen = 0;
    while(str.textbuffer[j] != '\0' && j <= STRING_MAX_LEN){
        j++;

        m_font_atlas->getCharacter(str.textbuffer[j], &ch);
        w += (float)(ch->advance_x >> 6) * scale;

        if(str.textbuffer[j] != '\n'){
            str.strlen++;
        }
        else{
            str.height += getFontHeigth() * scale;
            if(str.width < w){
                str.width = w;
            }
            w = 0;
        }
    }
    if(str.width < w){
        str.width = w;
    }

    m_update_buffer = true;
}


void Text2D::clearStrings(){
    m_strings.clear();
    m_update_buffer = true;
}


void Text2D::onFramebufferSizeUpdate(int fb_width, int fb_height){
    m_fb_width = fb_width;
    m_fb_height = fb_height;
    m_update_buffer = true;
}


void Text2D::setDisplacement(float x, float y){
    m_disp[0] = x;
    m_disp[1] = y;
}


uint Text2D::getFontHeigth() const{
    return m_font_atlas->getHeight() >> 6;
}

