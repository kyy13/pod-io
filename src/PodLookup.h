// pod-io
// Kyle J Burgess

#ifndef POD_LOOKUP_H
#define POD_LOOKUP_H

#include <limits>

constexpr uint8_t cPODX[4] =
    { 0x50u, 0x4Fu, 0x44u, 0x58u };

constexpr uint8_t cLITE[4] =
    { 0x4Cu, 0x49u, 0x54u, 0x45u };

constexpr uint8_t cBIGE[4] =
    { 0x42u, 0x49u, 0x47u, 0x45u };

constexpr uint8_t cNONE[4] =
    { 0x4Eu, 0x4Fu, 0x4Eu, 0x45u };

constexpr uint8_t cAD32[4] =
    { 0x41u, 0x44u, 0x33u, 0x32u };

constexpr uint8_t cCR32[4] =
    { 0x43u, 0x52u, 0x33u, 0x32u };

constexpr uint32_t MaxCountLookup[] =
    {
        0u,
        std::numeric_limits<uint32_t>::max() / 1u, // 1 byte
        std::numeric_limits<uint32_t>::max() / 2u, // 2 bytes
        0u,
        std::numeric_limits<uint32_t>::max() / 4u, // 4 bytes
        0u,
        0u,
        0u,
        std::numeric_limits<uint32_t>::max() / 8u, // 8 bytes
    };

#endif
