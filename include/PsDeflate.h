// PODserializer
// Kyle J Burgess

#ifndef PS_DEFLATE_H
#define PS_DEFLATE_H

#include <vector>
#include <cstdint>

std::vector<uint8_t> deflate(uint8_t* in, size_t size);

std::vector<uint8_t> inflate(uint8_t* in, size_t size);

#endif
