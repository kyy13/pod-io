// PODserializer
// Kyle J Burgess

#include "PsBytes.h"

bool is_little_endian()
{
    union
    {
        uint32_t i;
        uint8_t c[4];
    };

    i = 0x01020304u;

    return c[0] == 4;
}

bool is_big_endian()
{
    union
    {
        uint32_t i;
        uint8_t c[4];
    };

    i = 0x01020304u;

    return c[0] == 1;
}

size_t size_of_type(PsType type)
{
    return type & 0xffffu;
}

void pad_bytes(std::vector<uint8_t>& v, size_t firstByte, size_t numBytes)
{
    // check that there is enough space
    assert((firstByte + numBytes) <= v.size());

    if (numBytes != 0)
    {
        memset(&v[firstByte], 0, numBytes);
    }
}

size_t next_multiple_of(size_t val, size_t multiple)
{
    if (multiple == 0)
    {
        return val;
    }

    size_t remainder = val % multiple;

    if (remainder == 0)
    {
        return val;
    }

    return val + multiple - remainder;
}
