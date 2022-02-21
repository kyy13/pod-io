// PODserializer
// Kyle J Burgess

#ifndef PS_SYSTEM_H
#define PS_SYSTEM_H

#include <cstdint>

inline bool is_big_endian()
{
    union
    {
        uint32_t i;
        uint8_t c[4];
    };

    i = 0x01020304u;

    return c[0] == 1;
}

inline bool is_little_endian()
{
    union
    {
        uint32_t i;
        uint8_t c[4];
    };

    i = 0x01020304u;

    return c[0] == 4;
}

#endif
