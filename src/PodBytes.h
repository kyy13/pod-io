// pod-io
// Kyle J Burgess

#ifndef POD_BYTES_H
#define POD_BYTES_H

#include "PodTypes.h"
#include "bytes.h"

#include <cstdint>
#include <cassert>
#include <cstring>
#include <vector>

bool is_big_endian();

bool is_little_endian();

template<class T, bool reverse_bytes>
void set_bytes(std::vector<uint8_t>& dst, T src, size_t firstByte, size_t numBytes)
{
    // check that T is valid for n
    assert(sizeof(T) == numBytes);

    // check valid byte alignment
    assert((firstByte % sizeof(T)) == 0);

    // check destination size
    assert(dst.size() >= firstByte + numBytes);

    T* r = reinterpret_cast<T*>(dst.data() + firstByte);

    if constexpr (sizeof(T) == 1 || !reverse_bytes)
    {
        *r = src;
    }
    else
    {
        *r = k13::byteswap<T>(src);
    }
}

template<class T, bool reverse_bytes>
void set_bytes(std::vector<uint8_t>& dst, const void* src, size_t firstByte, size_t numBytes)
{
    // check that T is valid for n
    assert((numBytes % sizeof(T)) == 0);

    // check valid byte alignment
    assert((firstByte % sizeof(T)) == 0);

    // check destination size
    assert(dst.size() >= firstByte + numBytes);

    if constexpr (sizeof(T) == 1 || !reverse_bytes)
    {
        memcpy(dst.data() + firstByte, src, numBytes);
    }
    else
    {
        k13::byteswap<T>(
            reinterpret_cast<T*>(dst.data() + firstByte),
            reinterpret_cast<const T*>(src),
            numBytes / sizeof(T));
    }
}

template<class T, bool reverse_bytes>
void get_bytes(T& dst, std::vector<uint8_t>& src, size_t firstByte, size_t numBytes)
{
    // check that T is valid for n
    assert(sizeof(T) == numBytes);

    // check valid byte alignment
    assert((firstByte % numBytes) == 0);

    // check src size
    assert(src.size() >= firstByte + numBytes);

    // get
    const T* r = reinterpret_cast<const T*>(src.data() + firstByte);

    if constexpr (sizeof(T) == 1 || !reverse_bytes)
    {
        dst = *r;
    }
    else
    {
        dst = k13::byteswap<T>(*r);
    }
}

template<class T, bool reverse_bytes>
void get_bytes(void* dst, const std::vector<uint8_t>& src, size_t firstByte, size_t numBytes)
{
    // check that T is valid for n
    assert((numBytes % sizeof(T)) == 0);

    // check valid byte alignment
    assert((firstByte % numBytes) == 0);

    // check src size
    assert(src.size() >= firstByte + numBytes);

    // get
    if constexpr (sizeof(T) == 1 || !reverse_bytes)
    {
        memcpy(dst, src.data() + firstByte, numBytes);
    }
    else
    {
        k13::byteswap<T>(
            reinterpret_cast<T*>(dst),
            reinterpret_cast<const T*>(src.data() + firstByte),
            numBytes / sizeof(T));
    }
}

size_t size_of_type(pod_type_t type);

void pad_bytes(std::vector<uint8_t>& v, size_t firstByte, size_t numBytes);

size_t next_multiple_of(size_t val, size_t multiple);

#endif
