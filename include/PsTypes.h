// PODserializer
// Kyle J Burgess

#ifndef PS_TYPES_H
#define PS_TYPES_H

#include "PODserializer.h"

#include <unordered_map>
#include <vector>
#include <string>

struct PsBlock
{
    std::vector<uint8_t> data;
    size_t count;
    PsType type;
};

using PsMap = std::unordered_map<std::string, PsBlock>;

struct PsSerializer
{
    PsMap map;
};

#endif
