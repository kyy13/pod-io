// PODserializer
// Kyle J Burgess

#include "PsDeflate.h"
#include "PsBytes.h"

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

    deflateEnd(&zs);

    return PS_SUCCESS;
}

bool inflate_init(inflate_stream& is)
{
    is.zs =
        {
            .next_in = Z_NULL,
            .avail_in = 0,
            .zalloc = Z_NULL,
            .zfree = Z_NULL,
            .opaque = Z_NULL,
        };

    is.file = nullptr;

    return inflateInit(&is.zs) == Z_OK;
}

void inflate_end(inflate_stream& is)
{
    inflateEnd(&is.zs);
}

inflate_result inflate_next(inflate_stream& is, uint8_t* out, size_t out_size)
{
    int r;
    z_stream& zs = is.zs;

    zs.avail_out = out_size;
    zs.next_out = out;

    do
    {

        zs.avail_in = fread(is.buffer, 1, sizeof(is.buffer), is.file);

        if (zs.avail_in != sizeof(is.buffer))
        {
            return inflate_error;
        }

        zs.next_in = is.buffer;

        r = inflate(&zs, Z_NO_FLUSH);

        if (r != Z_OK && r != Z_STREAM_END)
        {
            return inflate_error;
        }

    } while (zs.avail_out == 0);

    if (r == Z_OK)
    {
        return inflate_ok;
    }

    return inflate_stream_end;
}
