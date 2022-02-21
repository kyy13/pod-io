// PODserializer
// Kyle J Burgess

#include "PODserializer.h"
#include "PsBytes.h"
#include "PsTypes.h"
#include "PsDeflate.h"
#include "PsSystem.h"

#include <cstring>
#include <fstream>

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
                break;
            case PS_UINT32:
            case PS_INT32:
            case PS_FLOAT32:
                data.set<uint32_t, swap_bytes>(i, block.bytes(), reinterpret_cast<const uint32_t*>(block.data()));
                break;
            case PS_UINT64:
            case PS_INT64:
            case PS_FLOAT64:
                data.set<uint64_t, swap_bytes>(i, block.bytes(), reinterpret_cast<const uint64_t*>(block.data()));
                break;
        }

        i += block.bytes();

        // Pad until next alignment
        prev = i;
        i = PsBytes::nextMultipleOf(i, 8);
        data.pad(prev, i - prev);
    }

    return data;
}

template<bool swap_bytes>
PsBytes write(PsSerializer* serializer, PsEndian endian, PsChecksum checksum)
{
    PsBytes header(16);
    size_t i = 0;

    // "PODS"

    header.set<uint8_t, swap_bytes>(i, 1, 0x50u);
    ++i;
    header.set<uint8_t, swap_bytes>(i, 1, 0x4Fu);
    ++i;
    header.set<uint8_t, swap_bytes>(i, 1, 0x44u);
    ++i;
    header.set<uint8_t, swap_bytes>(i, 1, 0x53u);
    ++i;

    // Endian

    if (endian == PS_NATIVE_ENDIAN)
    {
        endian = is_big_endian() ? PS_BIG_ENDIAN : PS_LITTLE_ENDIAN;
    }

    if (endian == PS_BIG_ENDIAN)
    {
        header.set<uint8_t, swap_bytes>(i, 1, 0x42u);
        ++i;
        header.set<uint8_t, swap_bytes>(i, 1, 0x45u);
        ++i;
    }
    else if (endian == PS_LITTLE_ENDIAN)
    {
        header.set<uint8_t, swap_bytes>(i, 1, 0x4Cu);
        ++i;
        header.set<uint8_t, swap_bytes>(i, 1, 0x45u);
        ++i;
    }

    // Checksum

    if (checksum == PS_CRC)
    {
        header.set<uint8_t, swap_bytes>(i, 1, 0x43u);
        ++i;
        header.set<uint8_t, swap_bytes>(i, 1, 0x52u);
        ++i;
    }
    else if (checksum == PS_NO_CHECKSUM)
    {
        header.set<uint8_t, swap_bytes>(i, 1, 0x4Eu);
        ++i;
        header.set<uint8_t, swap_bytes>(i, 1, 0x4Fu);
        ++i;
    }


    std::vector<uint8_t> compressed;

    {
        // Write data blocks
        auto data = writeBody<swap_bytes>(serializer);

        // Compress data blocks
        compressed = deflate(data.data(), data.size());

        // Write data size
        header.set<uint32_t, swap_bytes>(i, 4, data.size());
        i += 4;
    }

    // Write compressed size
    header.set<uint32_t, swap_bytes>(i, 4, compressed.size());
    i += 4;

    // Write compressed data
    size_t compressedSize = PsBytes::nextMultipleOf(compressed.size(), 8);

    header.resize(i + compressedSize);

    header.set<uint8_t, swap_bytes>(i, compressed.size(), compressed.data());
    i += compressed.size();

    header.pad(i, header.size() - i);

    return header;
}

PsResult psSaveFile(PsSerializer* serializer, const char* fileName, PsChecksum checksum, PsEndian endian)
{

    // Open File

    std::ofstream file(fileName, std::ios::binary);

    if (!file.is_open())
    {
        return PS_FILE_NOT_FOUND;
    }

    bool requiresByteSwap =
        (endian == PS_LITTLE_ENDIAN && is_big_endian()) ||
        (endian == PS_BIG_ENDIAN && is_little_endian());

    PsBytes data;

    if (requiresByteSwap)
    {
        data = write<true>(serializer, endian, checksum);
    }
    else
    {
        data = write<false>(serializer, endian, checksum);
    }

    file.write(
        reinterpret_cast<const char*>(data.data()),
        static_cast<std::streamsize>(data.size()));

    return PS_SUCCESS;
}
