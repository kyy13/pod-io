// PODserializer
// Kyle J Burgess

#include "PsDeflate.h"
#include "PsBytes.h"
#include "zlib.h"

#include <stdexcept>

std::vector<uint64_t> deflate8(std::vector<uint64_t>& in, uint32_t& byteCount)
{
    constexpr size_t TEMP_BUFFER_SIZE = 128 * 1024;

    std::vector<uint64_t> out;
    byteCount = 0;

    z_stream zs =
        {
            .zalloc = Z_NULL,
            .zfree = Z_NULL,
            .opaque = Z_NULL
        };

    if (deflateInit(&zs, Z_BEST_COMPRESSION) != Z_OK)
    {
        throw std::runtime_error("could not deflate_init");
    }

    uint8_t buf[TEMP_BUFFER_SIZE];

    zs.avail_in = in.size() * sizeof(uint64_t);
    zs.next_in = reinterpret_cast<uint8_t*>(in.data());
    zs.avail_out = TEMP_BUFFER_SIZE;
    zs.next_out = buf;

    while (zs.avail_in != 0)
    {
        if (deflate(&zs, Z_NO_FLUSH) != Z_OK)
        {
            throw std::runtime_error("could not deflate");
        }

        if (zs.avail_out == 0)
        {
            out.resize(pad64(byteCount + TEMP_BUFFER_SIZE));
            memcpy(reinterpret_cast<uint8_t*>(out.data()) + byteCount, buf, TEMP_BUFFER_SIZE);
            byteCount += TEMP_BUFFER_SIZE;
        }
    }

    int res = Z_OK;
    while (res == Z_OK)
    {
        if (zs.avail_out == 0)
        {
            out.resize(pad64(byteCount + TEMP_BUFFER_SIZE));
            memcpy(reinterpret_cast<uint8_t*>(out.data()) + byteCount, buf, TEMP_BUFFER_SIZE);
            byteCount += TEMP_BUFFER_SIZE;

            zs.avail_out = TEMP_BUFFER_SIZE;
            zs.next_out = buf;
        }
        res = deflate(&zs, Z_FINISH);
    }

    if (res != Z_STREAM_END)
    {
        throw std::runtime_error("could not finish deflate");
    }

    uint32_t dSize = TEMP_BUFFER_SIZE - zs.avail_out;
    out.resize(pad64(byteCount + dSize));
    memcpy(reinterpret_cast<uint8_t*>(out.data()) + byteCount, buf, dSize);
    byteCount += dSize;

    deflateEnd(&zs);

    return out;
}
