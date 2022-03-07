// PODserializer
// Kyle J Burgess

#include "PODserializer.h"
#include "PsBytes.h"
#include "PsTypes.h"
#include "PsDeflate.h"
#include "PsLookup.h"

#include <cstring>
#include <fstream>
#include <iostream>

template<bool reverse_bytes>
PsResult readBytes(PsSerializer* serializer, File& file, PsChecksum checksum, uint32_t check32)
{
    int r;
    auto& map = serializer->map;

    // Start inflating

    compress_stream is {};
    if (inflate_init(is, &file, checksum, check32) != COMPRESS_SUCCESS)
    {
        return PS_ZLIB_ERROR;
    }

    std::vector<uint8_t> buffer;

    // Get Value Groups
    while (true)
    {
        // Inflate sizes

        buffer.resize(12);
        r = inflate_next(is, buffer.data(), 12);

        if (r == COMPRESS_STREAM_END)
        {
            break;
        }

        if (r != COMPRESS_SUCCESS)
        {
            inflate_end(is);
            return PS_FILE_CORRUPT;
        }

        // Set sizes

        uint32_t strSize, rawType, valueCount;

        get_bytes<uint32_t, reverse_bytes>(strSize, buffer, 0, 4);
        get_bytes<uint32_t, reverse_bytes>(rawType, buffer, 4, 4);
        get_bytes<uint32_t, reverse_bytes>(valueCount, buffer, 8, 4);

        // Inflate key

        buffer.resize(strSize);
        r = inflate_next(is, buffer.data(), buffer.size());

        if (r == COMPRESS_STREAM_END)
        {
            inflate_end(is);
            return PS_FILE_CORRUPT;
        }

        if (r != COMPRESS_SUCCESS)
        {
            inflate_end(is);
            return PS_FILE_CORRUPT;
        }

        // Set key

        std::string key;
        key.assign(reinterpret_cast<char*>(buffer.data()), strSize);

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
        uint32_t blockSize = block.count * size_of_type(block.type);

        block.data.resize(blockSize);

        if constexpr (reverse_bytes)
        {
            buffer.resize(block.data.size());
            r = inflate_next(is, buffer.data(), buffer.size());
        }
        else
        {
            r = inflate_next(is, block.data.data(), block.data.size());
        }

        if constexpr (reverse_bytes)
        {
            switch(block.type)
            {
                case PS_CHAR8:
                case PS_UINT8:
                case PS_INT8:
                    get_bytes<uint8_t , reverse_bytes>(block.data.data(), buffer, 0, buffer.size());
                    break;
                case PS_UINT16:
                case PS_INT16:
                    get_bytes<uint16_t, reverse_bytes>(block.data.data(), buffer, 0, buffer.size());
                    break;
                case PS_UINT32:
                case PS_INT32:
                case PS_FLOAT32:
                    get_bytes<uint32_t, reverse_bytes>(block.data.data(), buffer, 0, buffer.size());
                    break;
                case PS_UINT64:
                case PS_INT64:
                case PS_FLOAT64:
                    get_bytes<uint64_t, reverse_bytes>(block.data.data(), buffer, 0, buffer.size());
                    break;
                default:
                    return PS_FILE_CORRUPT;
            }
        }

        if (r == COMPRESS_STREAM_END)
        {
            break;
        }

        if (r != COMPRESS_SUCCESS)
        {
            inflate_end(is);
            return PS_FILE_CORRUPT;
        }
    }

    if (inflate_end(is) != COMPRESS_SUCCESS)
    {
        return PS_FILE_CORRUPT;
    }

    // Read checksum

    size_t size;
    if (checksum == PS_CHECKSUM_NONE)
    {
        inflate_read_back(is, size);

        if (size != 0)
        {
            return PS_FILE_CORRUPT;
        }
    }
    else
    {
        buffer.resize(4);

        // get from inflate readback
        auto ptr = inflate_read_back(is, size);
        if (size != 0)
        {
            if (size > 4)
            {
                return PS_FILE_CORRUPT;
            }

            memcpy(buffer.data(), ptr, size);
        }

        // read the rest
        if (size != 4)
        {
            size_t ds = 4 - size;

            if (file.read(buffer.data() + size, ds) != ds)
            {
                return PS_FILE_CORRUPT;
            }
        }

        // get checksum
        get_bytes<uint32_t, reverse_bytes>(check32, buffer, 0, 4);

        if (check32 != is.check32)
        {
            std::cout << "checks: " << check32 << ", " << is.check32 << ", checksum = " << checksum << "\n";
            return PS_FILE_CORRUPT;
        }
    }

    return PS_SUCCESS;
}

PsResult psLoadFile(PsSerializer* serializer, const char* fileName)
{
    File file(fileName, FM_READ);

    if (!file.is_open())
    {
        return PS_FILE_NOT_FOUND;
    }

    // Read header

    uint8_t header[16];

    if (file.read(header, sizeof(header)) != sizeof(header))
    {
        return PS_FILE_CORRUPT;
    }

    // PODS

    if (memcmp(header, cPODS, 4) != 0)
    {
        return PS_FILE_CORRUPT;
    }

    // Endianness

    PsEndian endian;

    if (memcmp(header + 4, cLEND, 4) == 0)
    {
        endian = PS_ENDIAN_LITTLE;
    }
    else if (memcmp(header + 4, cBEND, 4) == 0)
    {
        endian = PS_ENDIAN_BIG;
    }
    else
    {
        return PS_FILE_CORRUPT;
    }

    // Checksum

    PsChecksum checksum;

    if (memcmp(header + 8, cCR32, 4) == 0)
    {
        checksum = PS_CHECKSUM_CRC32;
    }
    else if (memcmp(header + 8, cAD32, 4) == 0)
    {
        checksum = PS_CHECKSUM_ADLER32;
    }
    else if (memcmp(header + 8, cNONE, 4) == 0)
    {
        checksum = PS_CHECKSUM_NONE;
    }
    else
    {
        return PS_FILE_CORRUPT;
    }

    // Calculate checksum

    uint32_t check32 = 0;

    if (checksum == PS_CHECKSUM_ADLER32)
    {
        check32 = adler32(check32, header, 16);
    }
    else if (checksum == PS_CHECKSUM_CRC32)
    {
        check32 = crc32(check32, header, 16);
    }

    std::cout << "[load] checksum @ header: " << check32 << "\n";

    // Read bytes

    bool requiresByteSwap =
        (endian == PS_ENDIAN_LITTLE && is_big_endian()) ||
        (endian == PS_ENDIAN_BIG && is_little_endian());

    PsResult result;

    if (requiresByteSwap)
    {
        result = readBytes<true>(serializer, file, checksum, check32);
    }
    else
    {
        result = readBytes<false>(serializer, file, checksum, check32);
    }

    return result;
}
