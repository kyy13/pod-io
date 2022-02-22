// PODserializer
// Kyle J Burgess

#include "PsDeflate.h"
#include "PsBytes.h"
#include "zlib.h"

#include <stdexcept>

PsResult psDeflate(uint8_t* in, size_t in_size, std::vector<uint8_t>& out)
{
    constexpr size_t BUFFER_SIZE = 128 * 1024;

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

    size_t out_size = 0;
    out.resize(BUFFER_SIZE);

    zs.avail_in = in_size;
    zs.next_in = in;
    zs.avail_out = BUFFER_SIZE;
    zs.next_out = out.data();

    // Process all input, resizing the output buffer as necessary
    while (zs.avail_in != 0)
    {
        if (deflate(&zs, Z_NO_FLUSH) != Z_OK)
        {
            throw std::runtime_error("could not deflate");
        }

        if (zs.avail_out == 0)
        {
            out_size = out.size();

            out.resize(out_size + BUFFER_SIZE);
            zs.avail_out = BUFFER_SIZE;
            zs.next_out = out.data() + out_size;
        }
    }

    // Process remaining (after processing input) until Z_STREAM_END
    int res = Z_OK;
    while (res == Z_OK)
    {
        if (zs.avail_out == 0)
        {
            out_size = out.size();

            out.resize(out_size + BUFFER_SIZE);
            zs.avail_out = BUFFER_SIZE;
            zs.next_out = out.data() + out_size;
        }

        res = deflate(&zs, Z_FINISH);
    }

    if (res != Z_STREAM_END)
    {
        throw std::runtime_error("could not finish deflate");
    }

    // The amount of output deflate() produced on the last call
    // (the call that returned Z_STREAM_END
    size_t ds = BUFFER_SIZE - zs.avail_out;

    // The final size of the output vector
    out.resize(out_size + ds);

//    zs.avail_out = ds;
//    zs.next_out = out.data() + out_size;

    deflateEnd(&zs);

    return PS_SUCCESS;
}

PsResult psInflate(uint8_t* in, size_t in_size, std::vector<uint8_t>& out)
{
    z_stream zs =
        {
            .next_in = Z_NULL,
            .avail_in = 0,
            .zalloc = Z_NULL,
            .zfree = Z_NULL,
            .opaque = Z_NULL,
        };

    if (inflateInit(&zs) != Z_OK)
    {
        throw std::runtime_error("could not inflate_init");
    }

    
}
