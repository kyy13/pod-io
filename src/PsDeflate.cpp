// PODserializer
// Kyle J Burgess

#include "PsDeflate.h"
#include "PsBytes.h"
#include "zlib.h"

#include <stdexcept>

std::vector<uint8_t> deflate(uint8_t* in, size_t size)
{
    constexpr size_t TEMP_BUFFER_SIZE = 128 * 1024;

    std::vector<uint8_t> out;

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

    zs.avail_in = size;
    zs.next_in = in;
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
            size_t n = out.size();
            out.resize(n + TEMP_BUFFER_SIZE);
            memcpy(out.data() + n, buf, TEMP_BUFFER_SIZE);
        }
    }

    int res = Z_OK;
    while (res == Z_OK)
    {
        if (zs.avail_out == 0)
        {
            size_t n = out.size();
            out.resize(n + TEMP_BUFFER_SIZE);
            memcpy(out.data() + n, buf, TEMP_BUFFER_SIZE);

            zs.avail_out = TEMP_BUFFER_SIZE;
            zs.next_out = buf;
        }
        res = deflate(&zs, Z_FINISH);
    }

    if (res != Z_STREAM_END)
    {
        throw std::runtime_error("could not finish deflate");
    }

    size_t dSize = TEMP_BUFFER_SIZE - zs.avail_out;
    size_t n = out.size();
    out.resize(n + dSize);
    memcpy(out.data() + n, buf, dSize);

    deflateEnd(&zs);

    return out;
}
