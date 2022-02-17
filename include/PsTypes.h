// PODserializer
// Kyle J Burgess

#ifndef PS_TYPES_H
#define PS_TYPES_H

#include <unordered_map>
#include <vector>
#include <string>

struct PsBlock
{
    std::vector<uint64_t> values;
    uint32_t count;
    PsType type;
};

using PsMap = std::unordered_map<std::string, PsBlock>;

struct PsSerializer
{
    PsMap map;
};

uint32_t psGetTypeSize(PsType type);

#endif
