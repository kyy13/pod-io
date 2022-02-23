// PODserializer
// Kyle J Burgess

#include "PODserializer.h"
#include "PsBytes.h"
#include "PsTypes.h"
#include "PsDeflate.h"

#include <cstring>
#include <fstream>
#include <iostream>

template<bool reverse_bytes>
PsResult readBytes(PsSerializer* serializer, FILE* file)
{
    int r;
    auto& map = serializer->map;

    std::vector<uint8_t> buffer(8);

    if (fread(buffer.data(), 1, 8, file) != 8)
    {
        return PS_FILE_CORRUPT;
    }

    // Read size and compressed size
    uint32_t size, compressedSize;
    uint32_t processedSize = 0;

    get_bytes<uint32_t, reverse_bytes>(buffer, 0, 4, size);
    get_bytes<uint32_t, reverse_bytes>(buffer, 4, 4, compressedSize);

    // Start inflating

    inflate_stream is;
    if (!inflate_init(is, file, compressedSize))
    {
        return PS_ZLIB_ERROR;
    }

    // Get Value Groups
    while (size != 0u)
    {
        // Inflate sizes

        buffer.resize(12);
        r = inflate_next(is, buffer.data(), 12);
        processedSize += 12;

        if (r == inflate_stream_end)
        {
            break;
        }

        if (r != inflate_ok)
        {
            inflate_end(is);
            return PS_FILE_CORRUPT;
        }

        // Set sizes

        uint32_t strSize, rawType, valueCount;

        get_bytes<uint32_t, reverse_bytes>(buffer, 0, 4, strSize);
        get_bytes<uint32_t, reverse_bytes>(buffer, 4, 4, rawType);
        get_bytes<uint32_t, reverse_bytes>(buffer, 8, 4, valueCount);

        // Inflate key

        buffer.resize(next_multiple_of(12 + strSize, 8) - 12);
        r = inflate_next(is, buffer.data(), buffer.size());
        processedSize += buffer.size();

        std::cout << "bs=" << buffer.size() << "\n";
        std::cout << "r=" << r << "\n";

        if (r == inflate_stream_end)
        {
            break;
        }

        if (r != inflate_ok)
        {
            inflate_end(is);
            return PS_FILE_CORRUPT;
        }

        // Set key

        std::string key;
        key.assign(reinterpret_cast<char*>(buffer.data()), strSize);

        std::cout << key << "\n";

        // Setup block

        auto& block = map[key];
        block.count = valueCount;

        switch (rawType)
        {
            case PS_CHAR8:
                block.type = PS_CHAR8;
                break;
            case PS_UINT8:
                block.type = PS_UINT8;
                break;
            case PS_UINT16:
                block.type = PS_UINT16;
                break;
            case PS_UINT32:
                block.type = PS_UINT32;
                break;
            case PS_UINT64:
                block.type = PS_UINT64;
                break;
            case PS_INT8:
                block.type = PS_INT8;
                break;
            case PS_INT16:
                block.type = PS_INT16;
                break;
            case PS_INT32:
                block.type = PS_INT32;
                break;
            case PS_INT64:
                block.type = PS_INT64;
                break;
            case PS_FLOAT32:
                block.type = PS_FLOAT32;
                break;
            case PS_FLOAT64:
                block.type = PS_FLOAT64;
                break;
            default:
                return PS_FILE_CORRUPT;
        }

        // Inflate values

        block.data.resize(next_multiple_of(block.count * size_of_type(block.type), 8));
        r = inflate_next(is, block.data.data(), block.data.size());
        processedSize += block.data.size();

        if (r == inflate_stream_end)
        {
            break;
        }

        if (r != inflate_ok)
        {
            inflate_end(is);
            return PS_FILE_CORRUPT;
        }
    }

    inflate_end(is);

    if (r != inflate_stream_end)
    {
        return PS_FILE_CORRUPT;
    }

    if (processedSize != size)
    {
        return PS_FILE_CORRUPT;
    }

    return PS_SUCCESS;
}

PsResult psLoadFile(PsSerializer* serializer, const char* fileName)
{
    FILE* file = fopen(fileName, "rb");

    if (file == nullptr)
    {
        fclose(file);
        return PS_FILE_NOT_FOUND;
    }

    // Read header

    uint8_t header[8];

    if (fread(header, 1, 8, file) != 8)
    {
        fclose(file);
        return PS_FILE_CORRUPT;
    }

    // PS

    if (header[0] != 0x50u || header[1] != 0x53)
    {
        fclose(file);
        return PS_FILE_CORRUPT;
    }

    // Endianness

    PsEndian endian;

    if (header[2] == 0x4Cu && header[3] == 0x45u)
    {
        endian = PS_ENDIAN_LITTLE;
    }
    else if (header[2] == 0x42u && header[3] == 0x45u)
    {
        endian = PS_ENDIAN_BIG;
    }
    else
    {
        fclose(file);
        return PS_FILE_CORRUPT;
    }

    // Checksum

    PsChecksum checksum;

    if (header[4] == 0x43u && header[5] == 0x52u && header[6] == 0x33u && header[7] == 0x32u)
    {
        checksum = PS_CHECKSUM_CRC32;
    }
    else if (header[4] == 0x41u && header[5] == 0x44u && header[6] == 0x33u && header[7] == 0x32u)
    {
        checksum = PS_CHECKSUM_ADLER32;
    }
    else if (header[4] == 0x4Eu && header[5] == 0x4Fu && header[6] == 0x4Eu && header[7] == 0x45u)
    {
        checksum = PS_CHECKSUM_NONE;
    }
    else
    {
        fclose(file);
        return PS_FILE_CORRUPT;
    }

    // Read bytes

    bool requiresByteSwap =
        (endian == PS_ENDIAN_LITTLE && is_big_endian()) ||
        (endian == PS_ENDIAN_BIG && is_little_endian());

    PsResult result;

    if (requiresByteSwap)
    {
        result = readBytes<true>(serializer, file);
    }
    else
    {
        result = readBytes<false>(serializer, file);
    }

    fclose(file);
    return result;
}
