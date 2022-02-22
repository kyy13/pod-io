// PODserializer
// Kyle J Burgess

#include "PODserializer.h"
#include "PsBytes.h"
#include "PsTypes.h"
#include "PsDeflate.h"

#include <cstring>
#include <fstream>

template<bool reverse_bytes>
std::vector<uint8_t> writeBody(PsSerializer* serializer)
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
            next_multiple_of(9 + key.size(), 8) +
            next_multiple_of(block.data.size(), 8);
    }

    std::vector<uint8_t> data(size);

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

        set_bytes<uint32_t, reverse_bytes>(data, i, 4, static_cast<uint32_t>(block.type));
        i += 4;
        set_bytes<uint32_t, reverse_bytes>(data, i, 4, static_cast<uint32_t>(block.count));
        i += 4;
        set_bytes<uint8_t , reverse_bytes>(data, i, key.size(), reinterpret_cast<const uint8_t*>(key.data()));
        i += key.size();
        set_bytes<uint8_t , reverse_bytes>(data, i, 1, static_cast<uint8_t>(0));
        ++i;

        // Pad until next alignment
        size_t prev = i;
        i = next_multiple_of(i, 8);
        pad_bytes(data, prev, i - prev);

        // 8 byte-aligned data
        //    [?] data
        //    [.] 0 padding to reach next alignment

        switch(block.type)
        {
            case PS_CHAR8:
            case PS_UINT8:
            case PS_INT8:
                set_bytes<uint8_t , reverse_bytes>(data, i, block.data.size(), block.data.data());
                break;
            case PS_UINT16:
            case PS_INT16:
                set_bytes<uint16_t, reverse_bytes>(data, i, block.data.size(), reinterpret_cast<const uint16_t*>(block.data.data()));
                break;
            case PS_UINT32:
            case PS_INT32:
            case PS_FLOAT32:
                set_bytes<uint32_t, reverse_bytes>(data, i, block.data.size(), reinterpret_cast<const uint32_t*>(block.data.data()));
                break;
            case PS_UINT64:
            case PS_INT64:
            case PS_FLOAT64:
                set_bytes<uint64_t, reverse_bytes>(data, i, block.data.size(), reinterpret_cast<const uint64_t*>(block.data.data()));
                break;
        }

        i += block.data.size();

        // Pad until next alignment
        prev = i;
        i = next_multiple_of(i, 8);
        pad_bytes(data, prev, i - prev);
    }

    return data;
}

template<bool reverse_bytes>
std::vector<uint8_t> write(PsSerializer* serializer, PsEndian endian, PsChecksum checksum)
{
    std::vector<uint8_t> header(16);
    size_t i = 0;

    // "PODS"

    set_bytes<uint8_t, reverse_bytes>(header, i, 1, 0x50u);
    ++i;
    set_bytes<uint8_t, reverse_bytes>(header, i, 1, 0x4Fu);
    ++i;
    set_bytes<uint8_t, reverse_bytes>(header, i, 1, 0x44u);
    ++i;
    set_bytes<uint8_t, reverse_bytes>(header, i, 1, 0x53u);
    ++i;

    // Checksum

    if (checksum == PS_CRC)
    {
        set_bytes<uint8_t, reverse_bytes>(header, i, 1, 0x43u);
        ++i;
        set_bytes<uint8_t, reverse_bytes>(header, i, 1, 0x52u);
        ++i;
    }
    else if (checksum == PS_NO_CHECKSUM)
    {
        set_bytes<uint8_t, reverse_bytes>(header, i, 1, 0x4Eu);
        ++i;
        set_bytes<uint8_t, reverse_bytes>(header, i, 1, 0x4Fu);
        ++i;
    }

    // Endian

    if (endian == PS_NATIVE_ENDIAN)
    {
        endian = is_big_endian() ? PS_BIG_ENDIAN : PS_LITTLE_ENDIAN;
    }

    if (endian == PS_BIG_ENDIAN)
    {
        set_bytes<uint8_t, reverse_bytes>(header, i, 1, 0x42u);
        ++i;
        set_bytes<uint8_t, reverse_bytes>(header, i, 1, 0x45u);
        ++i;
    }
    else if (endian == PS_LITTLE_ENDIAN)
    {
        set_bytes<uint8_t, reverse_bytes>(header, i, 1, 0x4Cu);
        ++i;
        set_bytes<uint8_t, reverse_bytes>(header, i, 1, 0x45u);
        ++i;
    }

    std::vector<uint8_t> compressed;

    {
        // Write data blocks
        auto data = writeBody<reverse_bytes>(serializer);

        // Compress data blocks
        psDeflate(data.data(), data.size(), compressed);

        // Write data size
        set_bytes<uint32_t, reverse_bytes>(header, i, 4, data.size());
        i += 4;
    }

    // Write compressed size
    set_bytes<uint32_t, reverse_bytes>(header, i, 4, compressed.size());
    i += 4;

    // Write compressed data
    size_t compressedSize = next_multiple_of(compressed.size(), 8);

    header.resize(i + compressedSize);

    set_bytes<uint8_t, reverse_bytes>(header, i, compressed.size(), compressed.data());
    i += compressed.size();

    pad_bytes(header, i, header.size() - i);

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

    std::vector<uint8_t> data;

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
