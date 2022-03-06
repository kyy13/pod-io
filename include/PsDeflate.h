// PODserializer
// Kyle J Burgess

#ifndef PS_DEFLATE_H
#define PS_DEFLATE_H

#include "PODserializer.h"
#include "zlib.h"

#include <vector>
#include <cstdint>
#include <fstream>
#include <cstdio>
#include <functional>

PsResult psDeflate(uint8_t* in, size_t in_size, std::vector<uint8_t>& out, PsChecksum checksum);

struct inflate_stream
{
    z_stream zs;
    uint8_t buffer[8 * 1024];
    FILE* file;
    size_t avail;
};

enum inflate_result
{
    inflate_ok,
    inflate_stream_end,
    inflate_error,
};

bool inflate_init(inflate_stream& is, FILE* file, size_t size, PsChecksum checksum);

void inflate_end(inflate_stream& is);

inflate_result inflate_next(inflate_stream& is, uint8_t* out, size_t out_size);

#endif
