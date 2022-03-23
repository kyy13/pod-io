// pod-index
// Kyle J Burgess

#include "pod_index.h"
#include "PxBytes.h"
#include "PxTypes.h"
#include "PxDeflate.h"
#include "PxLookup.h"

#include <cstring>

#include "PxFile.h"

template<bool reverse_bytes>
PxResult writeBytes(PxContainer* container, File& file, PxCompression compression, PxChecksum checksum, uint32_t check32)
{
    auto& map = container->map;

    std::vector<uint8_t> buffer;

    compress_stream cs {};
    if (deflate_init(cs, &file, compression, checksum, check32) != COMPRESS_SUCCESS)
    {
        return PX_ZLIB_ERROR;
    }

    for (auto& pair : map)
    {
        const auto& key = pair.first;
        auto& data = pair.second;

        // Write header

        // 8 byte-aligned header
        //    [4] key size
        //    [4] value count
        //    [4] type
        //    [?] key

        size_t headerSize = 12 + key.size();
        buffer.resize(headerSize);

        set_bytes<uint32_t, reverse_bytes>(buffer, static_cast<uint32_t>(key.size()), 0, 4);
        set_bytes<uint32_t, reverse_bytes>(buffer, static_cast<uint32_t>(data.count), 4, 4);
        set_bytes<uint32_t, reverse_bytes>(buffer, static_cast<uint32_t>(data.type), 8, 4);
        set_bytes<uint8_t , reverse_bytes>(buffer, key.data(), 12, key.size());

        if (deflate_next(cs, buffer.data(), buffer.size()) != COMPRESS_SUCCESS)
        {
            return PX_ZLIB_ERROR;
        }

        // Write data

        // 8 byte-aligned data
        //    [?] data

        if constexpr (reverse_bytes)
        {
            buffer.resize(data.values.size());

            switch(data.type)
            {
                case PX_ASCII_CHAR8:
                case PX_UTF8_CHAR8:
                case PX_UINT8:
                case PX_INT8:
                    set_bytes<uint8_t , reverse_bytes>(buffer, data.values.data(), 0, data.values.size());
                    break;
                case PX_UINT16:
                case PX_INT16:
                    set_bytes<uint16_t, reverse_bytes>(buffer, data.values.data(), 0, data.values.size());
                    break;
                case PX_UINT32:
                case PX_INT32:
                case PX_FLOAT32:
                    set_bytes<uint32_t, reverse_bytes>(buffer, data.values.data(), 0, data.values.size());
                    break;
                case PX_UINT64:
                case PX_INT64:
                case PX_FLOAT64:
                    set_bytes<uint64_t, reverse_bytes>(buffer, data.values.data(), 0, data.values.size());
                    break;
            }

            if (deflate_next(cs, buffer.data(), buffer.size()) != COMPRESS_SUCCESS)
            {
                return PX_ZLIB_ERROR;
            }
        }
        else
        {
            if (deflate_next(cs, data.values.data(), data.values.size()) != COMPRESS_SUCCESS)
            {
                return PX_ZLIB_ERROR;
            }
        }
    }

    if (deflate_end(cs) != COMPRESS_SUCCESS)
    {
        return PX_ZLIB_ERROR;
    }

    // Write checksum
    if (checksum != PX_CHECKSUM_NONE)
    {
        buffer.resize(4);
        set_bytes<uint32_t, reverse_bytes>(buffer, cs.check32, 0, 4);
        file.write(buffer.data(), buffer.size());
    }

    return PX_SUCCESS;
}

PxResult pxSaveFile(PxContainer* container, const char* fileName, PxCompression compression, PxChecksum checksum, uint32_t checksumValue, PxEndian endianness)
{
    // Open File

    File file(fileName, FM_WRITE);

    if (!file.is_open())
    {
        return PX_FILE_NOT_FOUND;
    }

    // Write header

    uint8_t header[16];

    // "PODS"

    memcpy(header, cPODX, 4);

    // Endian

    switch(endianness)
    {
        case PX_ENDIAN_LITTLE:
            memcpy(header + 4, cLITE, 4);
            break;
        case PX_ENDIAN_BIG:
            memcpy(header + 4, cBIGE, 4);
            break;
        case PX_ENDIAN_NATIVE:
            if (is_little_endian())
            {
                memcpy(header + 4, cLITE, 4);
            }
            else if (is_big_endian())
            {
                memcpy(header + 4, cBIGE, 4);
            }
            else
            {
                return PX_ARGUMENT_ERROR;
            }
            break;
        default:
            return PX_ARGUMENT_ERROR;
    }

    // Checksum

    switch(checksum)
    {
        case PX_CHECKSUM_NONE:
            memcpy(header + 8, cNONE, 4);
            break;
        case PX_CHECKSUM_ADLER32:
            memcpy(header + 8, cAD32, 4);
            break;
        case PX_CHECKSUM_CRC32:
            memcpy(header + 8, cCR32, 4);
            break;
        default:
            return PX_ARGUMENT_ERROR;
    }

    // Reserved Bytes
    memcpy(header + 12, cNONE, 4);

    file.write(header, 16);

    // Compute checksum
    if (checksum == PX_CHECKSUM_ADLER32)
    {
        checksumValue = adler32(checksumValue, header, 16);
    }
    else if (checksum == PX_CHECKSUM_CRC32)
    {
        checksumValue = crc32(checksumValue, header, 16);
    }

    // Write endian-dependent blocks

    bool requiresByteSwap =
        (endianness == PX_ENDIAN_LITTLE && is_big_endian()) ||
        (endianness == PX_ENDIAN_BIG && is_little_endian());

    PxResult result;

    if (requiresByteSwap)
    {
        result = writeBytes<true>(container, file, compression, checksum, checksumValue);
    }
    else
    {
        result = writeBytes<false>(container, file, compression, checksum, checksumValue);
    }

    if (result != PX_SUCCESS)
    {
        return result;
    }

    return PX_SUCCESS;
}
