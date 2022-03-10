// PODstore
// Kyle J Burgess

#ifndef PS_TYPES_H
#define PS_TYPES_H

#include "PODstore.h"

#include <unordered_map>
#include <vector>
#include <string>

struct PsData
{
    std::vector<uint8_t> values;
    size_t count;
    PsType type;
};

using PsMap = std::unordered_map<std::string, PsData>;

struct PsContainer
{
    PsMap map;
};

#endif
