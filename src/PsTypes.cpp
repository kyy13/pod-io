// PODserializer
// Kyle J Burgess

#include "PODserializer.h"

uint32_t psGetTypeSize(PsType type)
{
    return type & 0xffffu;
}
