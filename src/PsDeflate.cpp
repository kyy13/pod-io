// PODserializer
// Kyle J Burgess

#include "PsDeflate.h"
#include "PsBytes.h"

#include <stdexcept>

PsResult psDeflate(uint8_t* in, size_t in_size, std::vector<uint8_t>& out, PsChecksum checksum)
{
    constexpr size_t BUFFER_SIZE = 128 * 1024;

    z_stream zs =
        {
            .zalloc = Z_NULL,
            .zfree = Z_NULL,
            .opaque = Z_NULL
        };

    constexpr int windowBits = 15;

    switch(checksum)
    {
        case PS_CHECKSUM_NONE:
            if (deflateInit2(&zs, Z_BEST_COMPRESSION, Z_DEFLATED, -windowBits, 8, Z_DEFAULT_STRATEGY) != Z_OK)
            {
                return PS_ZLIB_ERROR;
            }
            break;
        case PS_CHECKSUM_ADLER32:
            if (deflateInit2(&zs, Z_BEST_COMPRESSION, Z_DEFLATED, windowBits, 8, Z_DEFAULT_STRATEGY) != Z_OK)
            {
                return PS_ZLIB_ERROR;
            }
            break;
        case PS_CHECKSUM_CRC32:
            if (deflateInit2(&zs, Z_BEST_COMPRESSION, Z_DEFLATED, windowBits + 16, 8, Z_DEFAULT_STRATEGY) != Z_OK)
            {
                return PS_ZLIB_ERROR;
            }
            break;
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
            return PS_ZLIB_ERROR;
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
        return PS_ZLIB_ERROR;
    }

    // The amount of output deflate() produced on the last call
    // (the call that returned Z_STREAM_END
    size_t ds = BUFFER_SIZE - zs.avail_out;

    // The final size of the output vector
    out.resize(out_size + ds);

    deflateEnd(&zs);

    return PS_SUCCESS;
}

bool inflate_init(inflate_stream& is, FILE* file, size_t size, PsChecksum checksum)
{
    constexpr int windowBits = 15;

    is.zs =
        {
            .next_in = Z_NULL,
            .avail_in = 0,
            .zalloc = Z_NULL,
            .zfree = Z_NULL,
            .opaque = Z_NULL,
        };

    is.file = file;
    is.avail = size;

    switch(checksum)
    {
        case PS_CHECKSUM_NONE:
            if (inflateInit2(&is.zs, -windowBits) != Z_OK)
            {
                return false;
            }
            break;
        case PS_CHECKSUM_ADLER32:
            if (inflateInit2(&is.zs, windowBits) != Z_OK)
            {
                return false;
            }
            break;
        case PS_CHECKSUM_CRC32:
            if (inflateInit2(&is.zs, windowBits + 16) != Z_OK)
            {
                return false;
            }
            break;
    }

    return true;
}

void inflate_end(inflate_stream& is)
{
    inflateEnd(&is.zs);
}

inflate_result inflate_next(inflate_stream& is, uint8_t* out, size_t out_size)
{
    int r;
    auto& zs = is.zs;

    zs.avail_out = out_size;
    zs.next_out = out;

    while (zs.avail_out != 0)
    {
        if (zs.avail_in == 0)
        {
            size_t avail = (is.avail < sizeof(is.buffer))
                ? is.avail
                : sizeof(is.buffer);

            zs.avail_in = fread(is.buffer, 1, avail, is.file);
            is.avail -= avail;

            if (zs.avail_in != avail)
            {
                return inflate_error;
            }

            zs.next_in = is.buffer;
        }

        auto prev_in = zs.avail_in;
        auto prev_out = zs.avail_out;

        r = inflate(&zs, Z_NO_FLUSH);

        if (zs.avail_in == prev_in && zs.avail_out == prev_out)
        {
            return inflate_error;
        }

        if (r != Z_OK && r != Z_STREAM_END)
        {
            return inflate_error;
        }
    }

    if (r == Z_OK)
    {
        return inflate_ok;
    }

    return inflate_stream_end;
}
