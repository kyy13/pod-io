// pod-index
// Kyle J Burgess

#include "pod_index.h"
#include "PxBytes.h"
#include "PxTypes.h"
#include "PxDeflate.h"
#include "PxLookup.h"

#include <cstring>
#include <fstream>

template<bool reverse_bytes>
PxResult readBytes(PxContainer* container, File& file, PxChecksum checksum, uint32_t check32)
{
    int r;
    auto& map = container->map;

    // Start inflating

    compress_stream is {};
    if (inflate_init(is, &file, checksum, check32) != COMPRESS_SUCCESS)
    {
        return PX_ZLIB_ERROR;
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
            return PX_FILE_CORRUPT;
        }

        // Set sizes

        uint32_t strSize, rawType, valueCount;

        get_bytes<uint32_t, reverse_bytes>(strSize, buffer, 0, 4);
        get_bytes<uint32_t, reverse_bytes>(valueCount, buffer, 4, 4);
        get_bytes<uint32_t, reverse_bytes>(rawType, buffer, 8, 4);

        // Inflate key

        buffer.resize(strSize);
        r = inflate_next(is, buffer.data(), buffer.size());

        if (r == COMPRESS_STREAM_END)
        {
            inflate_end(is);
            return PX_FILE_CORRUPT;
        }

        if (r != COMPRESS_SUCCESS)
        {
            inflate_end(is);
            return PX_FILE_CORRUPT;
        }

        // Set key

        std::string key;
        key.assign(reinterpret_cast<char*>(buffer.data()), strSize);

        // Setup data

        auto& data = map[key];
        data.count = valueCount;

        switch (rawType)
        {
            case PX_ASCII_CHAR8:
                data.type = PX_ASCII_CHAR8;
                break;
            case PX_UINT8:
                data.type = PX_UINT8;
                break;
            case PX_UINT16:
                data.type = PX_UINT16;
                break;
            case PX_UINT32:
                data.type = PX_UINT32;
                break;
            case PX_UINT64:
                data.type = PX_UINT64;
                break;
            case PX_INT8:
                data.type = PX_INT8;
                break;
            case PX_INT16:
                data.type = PX_INT16;
                break;
            case PX_INT32:
                data.type = PX_INT32;
                break;
            case PX_INT64:
                data.type = PX_INT64;
                break;
            case PX_FLOAT32:
                data.type = PX_FLOAT32;
                break;
            case PX_FLOAT64:
                data.type = PX_FLOAT64;
                break;
            default:
                return PX_FILE_CORRUPT;
        }

        // Inflate values
        uint32_t blockSize = data.count * size_of_type(data.type);

        data.values.resize(blockSize);

        if constexpr (reverse_bytes)
        {
            buffer.resize(data.values.size());
            r = inflate_next(is, buffer.data(), buffer.size());
        }
        else
        {
            r = inflate_next(is, data.values.data(), data.values.size());
        }

        if constexpr (reverse_bytes)
        {
            switch(data.type)
            {
                case PX_ASCII_CHAR8:
                case PX_UINT8:
                case PX_INT8:
                    get_bytes<uint8_t , reverse_bytes>(data.values.data(), buffer, 0, buffer.size());
                    break;
                case PX_UINT16:
                case PX_INT16:
                    get_bytes<uint16_t, reverse_bytes>(data.values.data(), buffer, 0, buffer.size());
                    break;
                case PX_UINT32:
                case PX_INT32:
                case PX_FLOAT32:
                    get_bytes<uint32_t, reverse_bytes>(data.values.data(), buffer, 0, buffer.size());
                    break;
                case PX_UINT64:
                case PX_INT64:
                case PX_FLOAT64:
                    get_bytes<uint64_t, reverse_bytes>(data.values.data(), buffer, 0, buffer.size());
                    break;
                default:
                    return PX_FILE_CORRUPT;
            }
        }

        if (r == COMPRESS_STREAM_END)
        {
            break;
        }

        if (r != COMPRESS_SUCCESS)
        {
            inflate_end(is);
            return PX_FILE_CORRUPT;
        }
    }

    if (inflate_end(is) != COMPRESS_SUCCESS)
    {
        return PX_FILE_CORRUPT;
    }

    // Read checksum

    size_t size;
    if (checksum == PX_CHECKSUM_NONE)
    {
        inflate_read_back(is, size);

        if (size != 0)
        {
            return PX_FILE_CORRUPT;
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
                return PX_FILE_CORRUPT;
            }

            memcpy(buffer.data(), ptr, size);
        }

        // read the rest
        if (size != 4)
        {
            size_t ds = 4 - size;

            if (file.read(buffer.data() + size, ds) != ds)
            {
                return PX_FILE_CORRUPT;
            }
        }

        // get checksum
        get_bytes<uint32_t, reverse_bytes>(check32, buffer, 0, 4);

        if (check32 != is.check32)
        {
            return PX_FILE_CORRUPT;
        }
    }

    return PX_SUCCESS;
}

PxResult pxLoadFile(PxContainer* container, const char* fileName, PxChecksum checksum, uint32_t checksumValue)
{
    File file(fileName, FM_READ);

    if (!file.is_open())
    {
        return PX_FILE_NOT_FOUND;
    }

    // Read header

    uint8_t header[16];

    if (file.read(header, sizeof(header)) != sizeof(header))
    {
        return PX_FILE_CORRUPT;
    }

    // PODS

    if (memcmp(header, cPODX, 4) != 0)
    {
        return PX_FILE_CORRUPT;
    }

    // Endianness

    PxEndian endian;

    if (memcmp(header + 4, cLITE, 4) == 0)
    {
        endian = PX_ENDIAN_LITTLE;
    }
    else if (memcmp(header + 4, cBIGE, 4) == 0)
    {
        endian = PX_ENDIAN_BIG;
    }
    else
    {
        return PX_FILE_CORRUPT;
    }

    // Checksum

    if (memcmp(header + 8, cCR32, 4) == 0)
    {
        if (checksum != PX_CHECKSUM_CRC32)
        {
            return PX_FILE_CORRUPT;
        }
    }
    else if (memcmp(header + 8, cAD32, 4) == 0)
    {
        if (checksum != PX_CHECKSUM_ADLER32)
        {
            return PX_FILE_CORRUPT;
        }
    }
    else if (memcmp(header + 8, cNONE, 4) == 0)
    {
        if (checksum != PX_CHECKSUM_NONE)
        {
            return PX_FILE_CORRUPT;
        }
    }
    else
    {
        return PX_FILE_CORRUPT;
    }

    // Reserved
    if (memcmp(header + 12, cNONE, 4) != 0)
    {
        return PX_FILE_CORRUPT;
    }

    // Calculate checksum

    if (checksum == PX_CHECKSUM_ADLER32)
    {
        checksumValue = adler32(checksumValue, header, 16);
    }
    else if (checksum == PX_CHECKSUM_CRC32)
    {
        checksumValue = crc32(checksumValue, header, 16);
    }

    // Read bytes

    bool requiresByteSwap =
        (endian == PX_ENDIAN_LITTLE && is_big_endian()) ||
        (endian == PX_ENDIAN_BIG && is_little_endian());

    PxResult result;

    if (requiresByteSwap)
    {
        result = readBytes<true>(container, file, checksum, checksumValue);
    }
    else
    {
        result = readBytes<false>(container, file, checksum, checksumValue);
    }

    return result;
}
