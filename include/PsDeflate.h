// PODstore
// Kyle J Burgess

#ifndef PS_DEFLATE_H
#define PS_DEFLATE_H

#include "PODstore.h"
#include "PsFile.h"
#include "zlib.h"

#include <vector>
#include <cstdint>
#include <fstream>
#include <cstdio>
#include <functional>

struct compress_stream
{
    z_stream zs;               // zlib stream handle
    uint8_t buffer[8 * 1024];  // temporary buffer used for reading and writing file data
    File* file;                // pointer to file
    PsChecksum checksum;       // checksum type
    uint32_t check32;          // 32-bit checksum
};

enum compress_result
{
    COMPRESS_SUCCESS,          // successful operation
    COMPRESS_STREAM_END,       // success operation and reached end of stream (inflate_next only)
    COMPRESS_ERROR,            // failed operation
};

// Initialize a deflate stream
// returns COMPRESS_SUCCESS on success
// and COMPRESS_ERROR on failure
compress_result deflate_init(compress_stream& cs, File* file, PsCompression compression, PsChecksum checksum, uint32_t check32);

// Finish deflating and write any extra bytes held by the stream
// returns COMPRESS_SUCCESS on success
// and COMPRESS_ERROR on failure
compress_result deflate_end(compress_stream& cs);

// Deflate until input buffer is consumed
// returns COMPRESS_SUCCESS on success
// and COMPRESS_ERROR on failure
compress_result deflate_next(compress_stream& cs, uint8_t* in, size_t in_size);

// Initialize an inflate stream
// returns COMPRESS_SUCCESS on success
// and COMPRESS_ERROR on failure
compress_result inflate_init(compress_stream& cs, File* file, PsChecksum checksum, uint32_t check32);

// Finish an inflate stream
// returns COMPRESS_SUCCESS on success
// and COMPRESS_ERROR on failure
// extra bytes may have been read from the file (see inflate_read_back)
compress_result inflate_end(compress_stream& cs);

// Inflate until the output buffer is filled
// returns COMPRESS_SUCCESS or COMPRESS_STREAM_END on success
// and COMPRESS_ERROR on failure
// COMPRESS_STREAM_END means that the stream has been fully read and any other calls will
// result in error
compress_result inflate_next(compress_stream& cs, uint8_t* out, size_t out_size);

// Returns a pointer to extra bytes read from the file by the inflate stream
// sets size to the number of extra bytes
void* inflate_read_back(compress_stream& cs, size_t& size);

#endif
