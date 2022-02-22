// PODserializer
// Kyle J Burgess

#ifndef PS_DEFLATE_H
#define PS_DEFLATE_H

#include "PODserializer.h"

#include <vector>
#include <cstdint>

PsResult psDeflate(uint8_t* in, size_t in_size, std::vector<uint8_t>& out);

PsResult psInflate(uint8_t* in, size_t in_size, std::vector<uint8_t>& out);

#endif
