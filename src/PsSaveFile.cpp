// PODserializer
// Kyle J Burgess

#include "PODserializer.h"
#include "PsBytes.h"
#include "PsTypes.h"
#include "PsDeflate.h"

#include <cstring>
#include <fstream>
#include <iostream>

template<bool swap_bytes>
PsBytes writeBody(PsSerializer* serializer)
{
    // 8 byte-aligned header
    //    [4] type
    //    [4] value count
    //    [?] null terminated name string
    //    [.] 0 padding to reach next alignment

    // 8 byte-aligned data
    //    [?] data
    //    [.] 0 padding to reach next alignment

    // Pre-compute Size

    const auto& map = serializer->map;

    size_t size;

    for (const auto& pair : map)
    {
        const auto& key = pair.first;
        const auto& block = pair.second;

        size =
            PsBytes::nextMultipleOf(9 + key.size(), 8) +
            PsBytes::nextMultipleOf(block.bytes(), 8);
    }

    PsBytes data(size);

    // Write pairs

    size_t i = 0;
    for (const auto& pair : map)
    {
        const auto& key = pair.first;
        const auto& block = pair.second;

        size_t headerSize = PsBytes::nextMultipleOf(9 + key.size(), 8);
        size_t dataSize = PsBytes::nextMultipleOf(block.bytes(), 8);

        // 8 byte-aligned header
        //    [4] type
        //    [4] value count
        //    [?] null terminated name string
        //    [.] 0 padding to reach next alignment

        data.set<uint32_t, swap_bytes>(i, 4, static_cast<uint32_t>(block.type()));
        i += 4;
        data.set<uint32_t, swap_bytes>(i, 4, static_cast<uint32_t>(block.count()));
        i += 4;
        data.set<uint8_t , swap_bytes>(i, key.size(), reinterpret_cast<const uint8_t*>(key.data()));
        i += key.size();
        data.set<uint8_t , swap_bytes>(i, 1, static_cast<uint8_t>(0));
        ++i;

        // Pad until next alignment
        size_t prev = i;
        i = PsBytes::nextMultipleOf(i, 8);
        data.pad(prev, i - prev);

        // 8 byte-aligned data
        //    [?] data
        //    [.] 0 padding to reach next alignment

        switch(block.type())
        {
            case PS_CHAR8:
            case PS_UINT8:
            case PS_INT8:
                data.set<uint8_t , swap_bytes>(i, block.bytes(), block.data());
                break;
            case PS_UINT16:
            case PS_INT16:
                data.set<uint16_t, swap_bytes>(i, block.bytes(), reinterpret_cast<const uint16_t*>(block.data()));
            case PS_UINT32:
            case PS_INT32:
            case PS_FLOAT32:
                data.set<uint32_t, swap_bytes>(i, block.bytes(), reinterpret_cast<const uint32_t*>(block.data()));
            case PS_UINT64:
            case PS_INT64:
            case PS_FLOAT64:
                data.set<uint64_t, swap_bytes>(i, block.bytes(), reinterpret_cast<const uint64_t*>(block.data()));
        }

        i += block.bytes();

        // Pad until next alignment
        prev = i;
        i = PsBytes::nextMultipleOf(i, 8);
        data.pad(prev, i - prev);

        return data;
    }
}

PsResult psSaveFile(PsSerializer* serializer, const char* fileName, PsChecksum checksum, PsEndian endian)
{
    // Determine if bytes need to be swapped

    bool requiresByteSwap =
        (endian == PS_LITTLE_ENDIAN && isBigEndian()) ||
        (endian == PS_BIG_ENDIAN && !isBigEndian());

    // Open File

    std::ofstream file(fileName, std::ios::binary);

    if (!file.is_open())
    {
        return PS_FILE_NOT_FOUND;
    }

    uint64_t header[2] = {};
    uint8_t* header8 = reinterpret_cast<uint8_t*>(header);

    // Set label

    header8[0] = 0x50;
    header8[1] = 0x4F;
    header8[2] = 0x44;
    header8[3] = 0x53;

    // Set endianness

    if (endian == PS_NATIVE_ENDIAN)
    {
        endian = isBigEndian()
            ? PS_BIG_ENDIAN
            : PS_LITTLE_ENDIAN;
    }

    if (endian == PS_BIG_ENDIAN)
    {
        header8[4] = 0x42;
        header8[5] = 0x45;
    }
    else if (endian == PS_LITTLE_ENDIAN)
    {
        header8[4] = 0x4C;
        header8[5] = 0x45;
    }

    // Set checksum

    if (checksum == PS_CRC)
    {
        header8[6] = 0x43;
        header8[7] = 0x52;
    }
    else if (checksum == PS_NO_CHECKSUM)
    {
        header8[6] = 0x4E;
        header8[7] = 0x4F;
    }

    std::vector<uint64_t> body;
    std::vector<uint64_t> compressed;
    uint32_t* header32 = reinterpret_cast<uint32_t*>(&header[1]);

    if (requiresByteSwap)
    {
        // Write body
        body = writeBody<true>(serializer);

        // Write uncompressed size
        header32[0] = byteSwap<uint32_t>(body.size() * 8);

        // Compress body
        uint32_t byteCount;
        compressed = deflate8(body, byteCount);

        // Write compressed size
        header32[1] = byteSwap<uint32_t>(byteCount);
    }
    else
    {
        // Write body
        body = writeBody<false>(serializer);

        // Write uncompressed size
        header32[0] = body.size() * 8;

        // Compress / write compressed size
        compressed = deflate8(body, header32[1]);
    }

    // Don't need body anymore

    body.clear();
    body.shrink_to_fit();

    // Write Header

    file.write(
        reinterpret_cast<const char*>(header),
        static_cast<std::streamsize>(16));

    // Write Compressed Body

    file.write(
        reinterpret_cast<const char*>(compressed.data()),
        static_cast<std::streamsize>(compressed.size() * sizeof(uint64_t)));

    return PS_SUCCESS;
}
