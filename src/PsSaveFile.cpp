// PODserializer
// Kyle J Burgess

#include "PODserializer.h"
#include "PsBytes.h"
#include "PsTypes.h"
#include "PsDeflate.h"

#include <cstring>
#include "PsFile.h"

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

    size_t size = 0;

    for (const auto& pair : map)
    {
        const auto& key = pair.first;
        const auto& block = pair.second;

        size +=
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

        set_bytes<uint32_t, reverse_bytes>(data, static_cast<uint32_t>(key.size()), i, 4);
        i += 4;
        set_bytes<uint32_t, reverse_bytes>(data, static_cast<uint32_t>(block.type), i, 4);
        i += 4;
        set_bytes<uint32_t, reverse_bytes>(data, static_cast<uint32_t>(block.count), i, 4);
        i += 4;
        set_bytes<uint8_t , reverse_bytes>(data, key.data(), i, key.size());
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
                set_bytes<uint8_t , reverse_bytes>(data, block.data.data(), i, block.data.size());
                break;
            case PS_UINT16:
            case PS_INT16:
                set_bytes<uint16_t, reverse_bytes>(data, block.data.data(), i, block.data.size());
                break;
            case PS_UINT32:
            case PS_INT32:
            case PS_FLOAT32:
                set_bytes<uint32_t, reverse_bytes>(data, block.data.data(), i, block.data.size());
                break;
            case PS_UINT64:
            case PS_INT64:
            case PS_FLOAT64:
                set_bytes<uint64_t, reverse_bytes>(data, block.data.data(), i, block.data.size());
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

    // "PS"

    out[0] = 0x50u;
    out[1] = 0x53u;

    // Endian

    switch(endian)
    {
        case PS_ENDIAN_LITTLE:
            out[2] = 0x4Cu;
            out[3] = 0x45u;
            break;
        case PS_ENDIAN_BIG:
            out[2] = 0x42u;
            out[3] = 0x45u;
            break;
        case PS_ENDIAN_NATIVE:
            if (is_little_endian())
            {
                out[2] = 0x4Cu;
                out[3] = 0x45u;
            }
            else if (is_big_endian())
            {
                out[2] = 0x42u;
                out[3] = 0x45u;
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
            out[4] = 0x4Eu;
            out[5] = 0x4Fu;
            out[6] = 0x4Eu;
            out[7] = 0x45u;
            break;
        case PS_CHECKSUM_ADLER32:
            out[4] = 0x41u;
            out[5] = 0x44u;
            out[6] = 0x33u;
            out[7] = 0x32u;
            break;
        case PS_CHECKSUM_CRC32:
            out[4] = 0x43u;
            out[5] = 0x52u;
            out[6] = 0x33u;
            out[7] = 0x32u;
            break;
        default:
            return PS_UNSUPPORTED_ENDIANNESS;
    }

    size_t i = 8;

    std::vector<uint8_t> compressed;

    {
        // Write data blocks
        auto data = writeBody<reverse_bytes>(serializer);

        // Compress data blocks
        if (psDeflate(data.data(), data.size(), compressed, checksum) != PS_SUCCESS)
        {
            return PS_ZLIB_ERROR;
        }

        // Write data size
        set_bytes<uint32_t, reverse_bytes>(out, data.size(), i, 4);
        i += 4;
    }

    // Write compressed size
    set_bytes<uint32_t, reverse_bytes>(out, compressed.size(), i, 4);
    i += 4;

    // Write compressed data
    size_t compressedSize = next_multiple_of(compressed.size(), 8);

    out.resize(i + compressedSize);

    set_bytes<uint8_t, reverse_bytes>(out, compressed.data(), i, compressed.size());
    i += compressed.size();

    pad_bytes(out, i, out.size() - i);

    return PS_SUCCESS;
}

PsResult psSaveFile(PsSerializer* serializer, const char* fileName, PsChecksum checksum, PsEndian endian)
{
    // Open File

    File file(fileName, FM_WRITE);

    if (!file.is_open())
    {
        return PS_FILE_NOT_FOUND;
    }

//    compress_stream cs {};
//    deflate_init(cs, &file, checksum);

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

    file.write(data.data(), data.size());

    return PS_SUCCESS;
}
