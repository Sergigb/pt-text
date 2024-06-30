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

#ifndef TEXT2D_HPP
#define TEXT2D_HPP

#include <GLFW/glfw3.h>

#include <vector>

class FontAtlas;


#define STRING_MAX_LEN 256

// string placement
#define STRING_DRAW_ABSOLUTE_BL 1
#define STRING_DRAW_ABSOLUTE_TL 2
#define STRING_DRAW_ABSOLUTE_TR 3
#define STRING_DRAW_ABSOLUTE_BR 4
#define STRING_DRAW_RELATIVE 5

// string alignment
#define STRING_ALIGN_LEFT 1
#define STRING_ALIGN_CENTER_X 2
#define STRING_ALIGN_CENTER_Y 3
#define STRING_ALIGN_CENTER_XY 4
#define STRING_ALIGN_RIGHT 5 // this should be used when the text is drawn relative to the bottom left/top right


class Text2D{
    private:
        GLuint m_vao, m_vbo_vert, m_vbo_tex, m_vbo_col, m_vbo_ind;
        GLuint m_disp_location;
        GLuint m_num_vertices, m_num_indices;
        std::vector<struct string> m_strings;
        bool m_update_buffer, m_init;
        int m_fb_width, m_fb_height;
        GLuint m_shader;
        float m_disp[2];

        const FontAtlas* m_font_atlas;

        void updateBuffers();
        void initgl();
        void getPenXY(float& pen_x, float& pen_y, struct string* string_);
    public:
        Text2D();
        Text2D(int fb_width, int fb_height, const FontAtlas* font, GLuint shader);
        ~Text2D();

        void addString(const wchar_t* string, uint x, uint y, float scale, 
                       int placement, int alignment, float color[4]);
        void addString(const wchar_t* string, float relative_x, float 
                       relative_y, float scale, int alignment, float color[4]);
        void setDisplacement(float x, float y);
        void clearStrings();

        uint getFontHeigth() const;

        void onFramebufferSizeUpdate(int fb_width, int fb_height);
        void render();
};

struct string{
    int posx;
    int posy;
    uint strlen;  // string len without the \n
    short placement;
    short alignment;
    float relative_x;
    float relative_y;
    float scale;
    uint width;
    uint height;
    wchar_t textbuffer[STRING_MAX_LEN];
    float color[4];
};


#endif