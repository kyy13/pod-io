// pod-io
// Kyle J Burgess

#ifndef POD_TYPES_H
#define POD_TYPES_H

#include "pod_io.h"

#include <unordered_map>
#include <vector>
#include <string>

struct PodData
{
    std::vector<uint8_t> values;
    size_t count;
    pod_type_t type;
};

using PodMap = std::unordered_map<std::string, PodData>;

struct pod_container_t
{
    PodMap map;
};

#endif
