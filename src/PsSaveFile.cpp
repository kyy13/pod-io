// PODserializer
// Kyle J Burgess

#include "PODserializer.h"
#include "Bytes.h"
#include "PsTypes.h"

#include <cstring>
#include <fstream>

std::vector<uint64_t> writeBody(PsSerializer* serializer, bool requiresByteSwap)
{
    // 8 byte-aligned header
    //    [4] type
    //    [4] value count
    //    [?] null terminated name string
    //    [.] 0 padding to reach next alignment

    // 8 byte-aligned data
    //    [?] data
    //    [.] 0 padding to reach next alignment

    const auto& map = serializer->map;

    uint32_t blockHeaderSize64 = 0; // block header in 64s
    uint32_t blockDataSize64 = 0; // block data in 64s

    // Compute total body size
    for (const auto& pair : map)
    {
        const auto& key = pair.first;
        const auto& block = pair.second;

        // cumulative block header size (64s)
        // 4 type, 4 count, 1 null terminator for key aligned to 64
        blockHeaderSize64 += pad64(9 + key.size());

        // cumulative block data size aligned to 64 (64s)
        blockDataSize64 += block.values.size();
    }

    // Create body container
    std::vector<uint64_t> body(blockHeaderSize64 + blockDataSize64, 0);

    uint32_t blockStart64 = 0; // block start in 64s

    // Write pairs to body
    for (const auto& pair : map)
    {
        const auto& key = pair.first;
        const auto& block = pair.second;

        uint32_t type = static_cast<uint32_t>(block.type);
        uint32_t count = block.count;
        const auto& values = block.values;

        // block header size (64s)
        // 4 type, 4 count, 1 null terminator for key
        blockHeaderSize64 = pad64(9 + key.size());

        // block data size (64s)
        blockDataSize64 = values.size();

        // set block type and size
        uint64_t* p64 = &body[blockStart64];
        uint32_t* p32 = reinterpret_cast<uint32_t*>(p64);

        if (requiresByteSwap)
        {
            p32[0] = byteSwap<uint32_t>(type);
            p32[1] = byteSwap<uint32_t>(count);
        }
        else
        {
            p32[0] = type;
            p32[1] = count;
        }

        // set block name
        memcpy(&p32[2], key.data(), key.size());

        // set block data
        memcpy(&p64[blockHeaderSize64], values.data(), blockDataSize64 * 8);

        // next block
        blockStart64 += blockHeaderSize64 + blockDataSize64;
    }

    return body;
}

PsResult psSaveFile(PsSerializer* serializer, const char* fileName, PsChecksum checksum, PsEndian endian)
{
    // Determine if bytes need to be swapped

    bool requiresByteSwap = false;

    if (endian == PS_LITTLE_ENDIAN && isBigEndian())
    {
        requiresByteSwap = true;
    }

    if (endian == PS_BIG_ENDIAN && !isBigEndian())
    {
        requiresByteSwap = true;
    }



    // Open File

    std::ofstream file(fileName, std::ios::binary);

    if (!file.is_open())
    {
        return PS_FILE_NOT_FOUND;
    }

//    uint8_t header[16] = {};
//
//    // Set label
//
//    header[0] = 0x50;
//    header[1] = 0x4F;
//    header[2] = 0x44;
//    header[3] = 0x53;
//
//    // Set endianness
//
//    if (endian == PS_NATIVE_ENDIAN)
//    {
//        endian = isBigEndian()
//            ? PS_BIG_ENDIAN
//            : PS_LITTLE_ENDIAN;
//    }
//
//    if (endian == PS_BIG_ENDIAN)
//    {
//        header[4] = 0x42;
//        header[5] = 0x45;
//    }
//    else if (endian == PS_LITTLE_ENDIAN)
//    {
//        header[4] = 0x4C;
//        header[5] = 0x45;
//    }
//
//    // Set checksum
//
//    if (checksum == PS_CRC)
//    {
//        header[6] = 0x43;
//        header[7] = 0x52;
//    }
//    else if (checksum == PS_NO_CHECKSUM)
//    {
//        header[6] = 0x4E;
//        header[7] = 0x4F;
//    }

    return PS_SUCCESS;
}
