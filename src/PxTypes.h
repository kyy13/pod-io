// pod-index
// Kyle J Burgess

#ifndef PODX_TYPES_H
#define PODX_TYPES_H

#include "pod_index.h"

#include <unordered_map>
#include <vector>
#include <string>

struct PxData
{
    std::vector<uint8_t> values;
    size_t count;
    PxType type;
};

using PxMap = std::unordered_map<std::string, PxData>;

struct PxContainer
{
    PxMap map;
};

#endif
