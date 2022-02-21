// PODserializer
// Kyle J Burgess

#include "PODserializer.h"
#include "PsBytes.h"
#include "PsTypes.h"

#include <cstring>
#include <fstream>

PsResult psLoadFile(PsSerializer* serializer, const char* fileName)
{
//    std::ifstream file(fileName, std::ios::binary);
//
//    if (!file.is_open())
//    {
//        return PS_FILE_NOT_FOUND;
//    }
//
//    // Read header stamp and endianness
//
//    uint8_t header[16] = {};
//
//    file.read(reinterpret_cast<char*>(header), 16);
//
//    if (header[0] != 0x50 || header[1] != 0x4F || header[2] != 0x44 || header[3] != 0x53)
//    {
//        return PS_FILE_CORRUPT;
//    }
//
//    PsEndian endian;
//
//    if (header[4] == 0x42 && header[5] == 0x45)
//    {
//        endian = PS_BIG_ENDIAN;
//    }
//    else if (header[4] == 0x4C && header[5] == 0x45)
//    {
//        endian = PS_LITTLE_ENDIAN;
//    }
//    else
//    {
//        return PS_FILE_CORRUPT;
//    }
//
//    // Determine if bytes need to be swapped
//
//    bool requiresByteSwap = false;
//
//    if (endian == PS_LITTLE_ENDIAN && isBigEndian())
//    {
//        requiresByteSwap = true;
//    }
//
//    if (endian == PS_BIG_ENDIAN && !isBigEndian())
//    {
//        requiresByteSwap = true;
//    }
//
//    // Read header checksum and data sizes
//
//    PsChecksum checksum;
//
//    if (header[6] == 0x43 && header[7] == 0x52)
//    {
//        checksum = PS_CRC;
//    }
//    else if (header[6] == 0x4E && header[7] == 0x4F)
//    {
//        checksum = PS_NO_CHECKSUM;
//    }
//    else
//    {
//        return PS_FILE_CORRUPT;
//    }
//
//    uint32_t bodySize = *reinterpret_cast<uint32_t*>(&header[8]);
//
//    if (requiresByteSwap) { bodySize = byteSwap(bodySize); }
//
//    uint32_t compressedBodySize = *reinterpret_cast<uint32_t*>(&header[12]);
//
//    if (requiresByteSwap) { compressedBodySize = byteSwap(compressedBodySize); }
//
//    // Read Body
//
//    std::vector<uint8_t> compressedBody(compressedBodySize);
//
//    file.read(reinterpret_cast<char*>(compressedBody.data()), compressedBodySize);
//
//    // Check crc if necessary
//
//    if (checksum == PS_CRC)
//    {
//        uint32_t crc;
//
//        file.read(reinterpret_cast<char*>(&crc), 4);
//
//
//
//
//    }

    return PS_SUCCESS;
}
