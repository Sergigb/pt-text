#ifndef COMMON_H
#define COMMON_H

#define UNUSED(expr) do { (void)(expr); } while (0)

void wstrcpy(wchar_t* dest, const wchar_t* source, unsigned int max);

#endif
