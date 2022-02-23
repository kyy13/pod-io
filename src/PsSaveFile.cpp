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
    //    [4] key size
    //    [4] type
    //    [4] value count
    //    [?] key
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
            next_multiple_of(12 + key.size(), 8) +
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
        //    [4] key size
        //    [4] type
        //    [4] value count
        //    [?] key
        //    [.] 0 padding to reach next alignment

        set_bytes<uint32_t, reverse_bytes>(data, i, 4, static_cast<uint32_t>(key.size()));
        i += 4;
        set_bytes<uint32_t, reverse_bytes>(data, i, 4, static_cast<uint32_t>(block.type));
        i += 4;
        set_bytes<uint32_t, reverse_bytes>(data, i, 4, static_cast<uint32_t>(block.count));
        i += 4;
        set_bytes<uint8_t , reverse_bytes>(data, i, key.size(), reinterpret_cast<const uint8_t*>(key.data()));
        i += key.size();

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
PsResult writeBytes(PsSerializer* serializer, PsEndian endian, PsChecksum checksum, std::vector<uint8_t>& out)
{
    out.resize(16);
    size_t i = 0;

    // "PS"

    set_bytes<uint8_t, reverse_bytes>(out, i, 1, 0x50u);
    ++i;
    set_bytes<uint8_t, reverse_bytes>(out, i, 1, 0x53u);
    ++i;

    // Endian

    switch(endian)
    {
        case PS_ENDIAN_LITTLE:
            set_bytes<uint8_t, reverse_bytes>(out, i, 1, 0x4Cu);
            ++i;
            set_bytes<uint8_t, reverse_bytes>(out, i, 1, 0x45u);
            ++i;
            break;
        case PS_ENDIAN_BIG:
            set_bytes<uint8_t, reverse_bytes>(out, i, 1, 0x42u);
            ++i;
            set_bytes<uint8_t, reverse_bytes>(out, i, 1, 0x45u);
            ++i;
            break;
        case PS_ENDIAN_NATIVE:
            if (is_little_endian())
            {
                set_bytes<uint8_t, reverse_bytes>(out, i, 1, 0x4Cu);
                ++i;
                set_bytes<uint8_t, reverse_bytes>(out, i, 1, 0x45u);
                ++i;
            }
            else if (is_big_endian())
            {
                set_bytes<uint8_t, reverse_bytes>(out, i, 1, 0x42u);
                ++i;
                set_bytes<uint8_t, reverse_bytes>(out, i, 1, 0x45u);
                ++i;
            }
            else
            {
                return PS_UNSUPPORTED_ENDIANNESS;
            }
            break;
        default:
            return PS_UNSUPPORTED_ENDIANNESS;
    }

    // Checksum

    switch(checksum)
    {
        case PS_CHECKSUM_NONE:
            set_bytes<uint8_t, reverse_bytes>(out, i, 1, 0x4Eu);
            ++i;
            set_bytes<uint8_t, reverse_bytes>(out, i, 1, 0x4Fu);
            ++i;
            set_bytes<uint8_t, reverse_bytes>(out, i, 1, 0x4Eu);
            ++i;
            set_bytes<uint8_t, reverse_bytes>(out, i, 1, 0x45u);
            ++i;
            break;
        case PS_CHECKSUM_ADLER32:
            set_bytes<uint8_t, reverse_bytes>(out, i, 1, 0x41u);
            ++i;
            set_bytes<uint8_t, reverse_bytes>(out, i, 1, 0x44u);
            ++i;
            set_bytes<uint8_t, reverse_bytes>(out, i, 1, 0x33u);
            ++i;
            set_bytes<uint8_t, reverse_bytes>(out, i, 1, 0x32u);
            ++i;
            break;
        case PS_CHECKSUM_CRC32:
            set_bytes<uint8_t, reverse_bytes>(out, i, 1, 0x43u);
            ++i;
            set_bytes<uint8_t, reverse_bytes>(out, i, 1, 0x52u);
            ++i;
            set_bytes<uint8_t, reverse_bytes>(out, i, 1, 0x33u);
            ++i;
            set_bytes<uint8_t, reverse_bytes>(out, i, 1, 0x32u);
            ++i;
            break;
        default:
            return PS_UNSUPPORTED_ENDIANNESS;
    }

    std::vector<uint8_t> compressed;

    {
        // Write data blocks
        auto data = writeBody<reverse_bytes>(serializer);

        // Compress data blocks
        psDeflate(data.data(), data.size(), compressed);

        // Write data size
        set_bytes<uint32_t, reverse_bytes>(out, i, 4, data.size());
        i += 4;
    }

    // Write compressed size
    set_bytes<uint32_t, reverse_bytes>(out, i, 4, compressed.size());
    i += 4;

    // Write compressed data
    size_t compressedSize = next_multiple_of(compressed.size(), 8);

    out.resize(i + compressedSize);

    set_bytes<uint8_t, reverse_bytes>(out, i, compressed.size(), compressed.data());
    i += compressed.size();

    pad_bytes(out, i, out.size() - i);

    return PS_SUCCESS;
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
        (endian == PS_ENDIAN_LITTLE && is_big_endian()) ||
        (endian == PS_ENDIAN_BIG && is_little_endian());

    std::vector<uint8_t> data;
    PsResult result;

    if (requiresByteSwap)
    {
        result = writeBytes<true>(serializer, endian, checksum, data);
    }
    else
    {
        result = writeBytes<false>(serializer, endian, checksum, data);
    }

    if (result != PS_SUCCESS)
    {
        return result;
    }

    file.write(
        reinterpret_cast<const char*>(data.data()),
        static_cast<std::streamsize>(data.size()));

    return PS_SUCCESS;
}
