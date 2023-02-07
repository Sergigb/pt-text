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

#include <iostream>

#ifdef SAVE_STB
    #ifdef __GNUC__ // make gcc shut up about missing field initializers on stb
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wmissing-field-initializers"
    #else
        // clang??
    #endif

    #define STB_IMAGE_WRITE_IMPLEMENTATION
    #include <stb_image_write.h>
#endif

#include "src/FontAtlas.h"
#include "src/common.h"


int main(int argc, char* argv[]){
    /*
     * Usage:
     * ./main PATH_TO_FONT
     */
    const char* path;

    if(argc < 2){
        std::cerr << "Missing argument path to font" << std::endl;
        return EXIT_FAILURE;
    }
    else if(argc > 2){
        std::cerr << "Too many arguments" << std::endl;
        return EXIT_FAILURE;
    }
    else
        path = argv[1];

    FontAtlas atlas(512); // create the atlas

    if(atlas.loadFont(path, 32)){
        std::cerr << "Failed to load font" << std::endl;
        return EXIT_FAILURE;
    }

    uint failed_chars = 0;
    failed_chars += atlas.loadCharacterRange(32, 255); // ascii
    failed_chars += atlas.loadCharacterRange(913, 1023); // greek and coptic
    std::cout << "Failed to load " << failed_chars << " characters" << std::endl;

    atlas.createAtlas(true);

    return EXIT_SUCCESS;
}