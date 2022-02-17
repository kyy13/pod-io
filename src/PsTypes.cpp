// PODserializer
// Kyle J Burgess

#include "PODserializer.h"

const uint32_t TypeSizeLookup[11] =
    {
        1,
        1,
        2,
        4,
        8,
        1,
        2,
        4,
        8,
        4,
        8,
    };

uint32_t psGetTypeSize(PsType type)
{
    return TypeSizeLookup[type];
}
