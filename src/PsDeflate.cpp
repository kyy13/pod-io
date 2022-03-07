// PODserializer
// Kyle J Burgess

#include "PsDeflate.h"
#include "PsBytes.h"

compress_result deflate_init(compress_stream& is, File* file, PsChecksum checksum)
{
    auto& zs = is.zs;

    zs =
        {
            .zalloc = Z_NULL,
            .zfree = Z_NULL,
            .opaque = Z_NULL
        };

    is.file = file;

    constexpr int windowBits = 15;

    switch(checksum)
    {
        case PS_CHECKSUM_NONE:
            if (deflateInit2(&zs, Z_BEST_COMPRESSION, Z_DEFLATED, -windowBits, 8, Z_DEFAULT_STRATEGY) != Z_OK)
            {
                return COMPRESS_ERROR;
            }
            break;
        case PS_CHECKSUM_ADLER32:
            if (deflateInit2(&zs, Z_BEST_COMPRESSION, Z_DEFLATED, windowBits, 8, Z_DEFAULT_STRATEGY) != Z_OK)
            {
                return COMPRESS_ERROR;
            }
            break;
        case PS_CHECKSUM_CRC32:
            if (deflateInit2(&zs, Z_BEST_COMPRESSION, Z_DEFLATED, windowBits + 16, 8, Z_DEFAULT_STRATEGY) != Z_OK)
            {
                return COMPRESS_ERROR;
            }
            break;
    }

    zs.avail_out = sizeof(is.buffer);
    zs.next_out = is.buffer;

    return COMPRESS_SUCCESS;
}

compress_result deflate_end(compress_stream& is)
{
    auto& zs = is.zs;

    int res = Z_OK;
    while (res == Z_OK)
    {
        if (zs.avail_out == 0)
        {
            is.file->write(is.buffer, sizeof(is.buffer));

            zs.avail_out = sizeof(is.buffer);
            zs.next_out = is.buffer;
        }

        res = deflate(&zs, Z_FINISH);
    }

    if (res != Z_STREAM_END)
    {
        return COMPRESS_ERROR;
    }

    // Write the amount of output deflate() produced on the last call
    // (the call that returned Z_STREAM_END
    size_t ds = sizeof(is.buffer) - zs.avail_out;
    if (ds != 0)
    {
        is.file->write(is.buffer, sizeof(is.buffer) - zs.avail_out);
    }

    if (deflateEnd(&zs) != Z_OK)
    {
        return COMPRESS_ERROR;
    }

    return COMPRESS_SUCCESS;
}

compress_result deflate_next(compress_stream& is, uint8_t* in, size_t in_size)
{
    auto& zs = is.zs;

    zs.avail_in = in_size;
    zs.next_in = in;

    // Process all input, writing to file as necessary
    while (zs.avail_in != 0)
    {
        assert(zs.next_in != nullptr);

        if (deflate(&zs, Z_NO_FLUSH) != Z_OK)
        {
            return COMPRESS_ERROR;
        }

        if (zs.avail_out == 0)
        {
            is.file->write(is.buffer, sizeof(is.buffer));

            zs.avail_out = sizeof(is.buffer);
            zs.next_out = is.buffer;
        }
    }

    return COMPRESS_SUCCESS;
}

compress_result inflate_init(compress_stream& is, File* file, PsChecksum checksum)
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

    switch(checksum)
    {
        case PS_CHECKSUM_NONE:
            if (inflateInit2(&is.zs, -windowBits) != Z_OK)
            {
                return COMPRESS_ERROR;
            }
            break;
        case PS_CHECKSUM_ADLER32:
            if (inflateInit2(&is.zs, windowBits) != Z_OK)
            {
                return COMPRESS_ERROR;
            }
            break;
        case PS_CHECKSUM_CRC32:
            if (inflateInit2(&is.zs, windowBits + 16) != Z_OK)
            {
                return COMPRESS_ERROR;
            }
            break;
    }

    return COMPRESS_SUCCESS;
}

compress_result inflate_end(compress_stream& cs)
{
    if (inflateEnd(&cs.zs) != Z_OK)
    {
        return COMPRESS_ERROR;
    }

    return COMPRESS_SUCCESS;
}

compress_result inflate_next(compress_stream& is, uint8_t* out, size_t out_size)
{
    int r;
    auto& zs = is.zs;

    zs.avail_out = out_size;
    zs.next_out = out;

    while (zs.avail_out != 0)
    {
        if (zs.avail_in == 0)
        {
            zs.avail_in = is.file->read(is.buffer, sizeof(is.buffer));
            zs.next_in = is.buffer;
        }

        auto prev_in = zs.avail_in;
        auto prev_out = zs.avail_out;

        r = inflate(&zs, Z_NO_FLUSH);

        if (zs.avail_in == prev_in && zs.avail_out == prev_out)
        {
            return COMPRESS_ERROR;
        }

        if (r == Z_STREAM_END)
        {
            return COMPRESS_STREAM_END;
        }

        if (r != Z_OK)
        {
            return COMPRESS_ERROR;
        }
    }

    return COMPRESS_SUCCESS;
}

void* inflate_read_back(compress_stream& cs, size_t& size)
{
    size = cs.zs.avail_in;
    return cs.buffer;
}
