#include "font.h"
#include <stddef.h>


static Font font = {0};


inline void load_font(const char *path)
{
    if (font.texture.id == 0)
        font = LoadFontEx(path, 128, NULL, 95);
}

inline Font get_font()
{
    return font;
}
