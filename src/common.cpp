#include <cstring>

#include "common.h"

void wstrcpy(wchar_t* dest, const wchar_t* source, unsigned int max){
    unsigned int i = 0;
    while(source[i] != '\0' && i < max)
        i++;
    std::memcpy(dest, source, (i + 1) * sizeof(wchar_t));
}

