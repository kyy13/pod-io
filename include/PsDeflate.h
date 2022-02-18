// PODserializer
// Kyle J Burgess

#ifndef PS_DEFLATE_H
#define PS_DEFLATE_H

#include <cstdint>
#include <vector>

std::vector<uint64_t> deflate8(std::vector<uint64_t>& in, uint32_t& byteCount);

#endif
