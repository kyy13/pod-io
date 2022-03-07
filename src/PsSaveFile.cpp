// PODserializer
// Kyle J Burgess

#include "PODserializer.h"
#include "PsBytes.h"
#include "PsTypes.h"
#include "PsDeflate.h"
#include "PsLookup.h"

#include <cstring>
#include <iostream>

#include "PsFile.h"

template<bool reverse_bytes>
PsResult writeBytes(PsSerializer* serializer, File& file, PsChecksum checksum, uint32_t check32)
{
    auto& map = serializer->map;

    std::vector<uint8_t> buffer;

    compress_stream cs {};
    if (deflate_init(cs, &file, checksum, check32) != COMPRESS_SUCCESS)
    {
        return PS_ZLIB_ERROR;
    }

    for (auto& pair : map)
    {
        const auto& key = pair.first;
        auto& block = pair.second;

        // Write header

        // 8 byte-aligned header
        //    [4] key size
        //    [4] type
        //    [4] value count
        //    [?] key
        //    [.] 0 padding to reach next alignment

        size_t headerSize = 12 + key.size();
        buffer.resize(headerSize);

        set_bytes<uint32_t, reverse_bytes>(buffer, static_cast<uint32_t>(key.size()), 0, 4);
        set_bytes<uint32_t, reverse_bytes>(buffer, static_cast<uint32_t>(block.type), 4, 4);
        set_bytes<uint32_t, reverse_bytes>(buffer, static_cast<uint32_t>(block.count), 8, 4);
        set_bytes<uint8_t , reverse_bytes>(buffer, key.data(), 12, key.size());

        if (deflate_next(cs, buffer.data(), buffer.size()) != COMPRESS_SUCCESS)
        {
            return PS_ZLIB_ERROR;
        }

        // Write data

        // 8 byte-aligned data
        //    [?] data
        //    [.] 0 padding to reach next alignment

        if constexpr (reverse_bytes)
        {
            buffer.resize(block.data.size());

            switch(block.type)
            {
                case PS_CHAR8:
                case PS_UINT8:
                case PS_INT8:
                    set_bytes<uint8_t , reverse_bytes>(buffer, block.data.data(), 0, block.data.size());
                    break;
                case PS_UINT16:
                case PS_INT16:
                    set_bytes<uint16_t, reverse_bytes>(buffer, block.data.data(), 0, block.data.size());
                    break;
                case PS_UINT32:
                case PS_INT32:
                case PS_FLOAT32:
                    set_bytes<uint32_t, reverse_bytes>(buffer, block.data.data(), 0, block.data.size());
                    break;
                case PS_UINT64:
                case PS_INT64:
                case PS_FLOAT64:
                    set_bytes<uint64_t, reverse_bytes>(buffer, block.data.data(), 0, block.data.size());
                    break;
            }

            if (deflate_next(cs, buffer.data(), buffer.size()) != COMPRESS_SUCCESS)
            {
                return PS_ZLIB_ERROR;
            }
        }
        else
        {
            if (deflate_next(cs, block.data.data(), block.data.size()) != COMPRESS_SUCCESS)
            {
                return PS_ZLIB_ERROR;
            }
        }
    }

    if (deflate_end(cs) != COMPRESS_SUCCESS)
    {
        return PS_ZLIB_ERROR;
    }

    // Write checksum
    if (checksum != PS_CHECKSUM_NONE)
    {
        buffer.resize(4);
        set_bytes<uint32_t, reverse_bytes>(buffer, cs.check32, 0, 4);
        file.write(buffer.data(), buffer.size());
    }

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

    // Write header

    uint8_t header[16];

    // "PODS"

    memcpy(header, cPODS, 4);

    // Endian

    switch(endian)
    {
        case PS_ENDIAN_LITTLE:
            memcpy(header + 4, cLEND, 4);
            break;
        case PS_ENDIAN_BIG:
            memcpy(header + 4, cBEND, 4);
            break;
        case PS_ENDIAN_NATIVE:
            if (is_little_endian())
            {
                memcpy(header + 4, cLEND, 4);
            }
            else if (is_big_endian())
            {
                memcpy(header + 4, cBEND, 4);
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
            memcpy(header + 8, cNONE, 4);
            break;
        case PS_CHECKSUM_ADLER32:
            memcpy(header + 8, cAD32, 4);
            break;
        case PS_CHECKSUM_CRC32:
            memcpy(header + 8, cCR32, 4);
            break;
        default:
            return PS_UNSUPPORTED_ENDIANNESS;
    }

    // Padding
    memset(header + 12, 0, 4);

    file.write(header, 16);

    // Compute checksum
    uint32_t check32 = 0;

    if (checksum == PS_CHECKSUM_ADLER32)
    {
        check32 = adler32(check32, header, 16);
    }
    else if (checksum == PS_CHECKSUM_CRC32)
    {
        check32 = crc32(check32, header, 16);
    }

    std::cout << "[save] checksum @ header: " << check32 << "\n";

    // Write endian-dependent blocks

    bool requiresByteSwap =
        (endian == PS_ENDIAN_LITTLE && is_big_endian()) ||
        (endian == PS_ENDIAN_BIG && is_little_endian());

    PsResult result;

    if (requiresByteSwap)
    {
        result = writeBytes<true>(serializer, file, checksum, check32);
    }
    else
    {
        result = writeBytes<false>(serializer, file, checksum, check32);
    }

    if (result != PS_SUCCESS)
    {
        return result;
    }

    return PS_SUCCESS;
}
