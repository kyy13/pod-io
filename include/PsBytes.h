// PODserializer
// Kyle J Burgess

#ifndef PS_BYTES_H
#define PS_BYTES_H

#include "PsTypes.h"

#include <cstdint>
#include <cassert>
#include <cstring>
#include <vector>

bool is_big_endian();

bool is_little_endian();

template<class T>
T get_reverse_bytes(T value)
{
    T result;

    auto* src = reinterpret_cast<uint8_t*>(&value);
    auto* dst = reinterpret_cast<uint8_t*>(&result);

    for (size_t i = 0; i != sizeof(T); ++i)
    {
        constexpr size_t end = sizeof(T) - 1;
        dst[i] = src[end - i];
    }

    return result;
}

template<class T, bool reverse_bytes>
void set_bytes(std::vector<uint8_t>& v, size_t firstByte, size_t numBytes, T x)
{
    // check that T is valid for n
    assert(sizeof(T) == numBytes);

    // check valid byte alignment
    assert((firstByte % sizeof(T)) == 0);

    // check that there is enough space
    assert((firstByte + numBytes) <= v.size());

    // set
    T* r = reinterpret_cast<T*>(&v[firstByte]);

    if constexpr (sizeof(T) == 1 || !reverse_bytes)
    {
        *r = x;
    }
    else
    {
        *r = get_reverse_bytes(x);
    }
}

template<class T, bool reverse_bytes>
void set_bytes(std::vector<uint8_t>& v, size_t firstByte, size_t numBytes, const T* x)
{
    // check that T is valid for n
    assert((numBytes % sizeof(T)) == 0);

    // check valid byte alignment
    assert((firstByte % sizeof(T)) == 0);

    // check that there is enough space
    assert((firstByte + numBytes) <= v.size());

    // set
    if constexpr (sizeof(T) == 1)
    {
        memcpy(&v[firstByte], x, numBytes);
    }
    else
    {
        T* r = reinterpret_cast<T*>(&v[firstByte]);
        T* end = reinterpret_cast<T*>(&v[firstByte + numBytes]);

        if constexpr (reverse_bytes)
        {
            for (; r != end; ++r)
            {
                *r = get_reverse_bytes(*x);
                ++x;
            }
        }
        else
        {
            for (; r != end; ++r)
            {
                *r = *x;
                ++x;
            }
        }
    }
}

template<class T, bool reverse_bytes>
void get_bytes(const std::vector<uint8_t>& v, size_t firstByte, size_t numBytes, T& x)
{
    // check that T is valid for n
    assert(sizeof(T) == numBytes);

    // check valid byte alignment
    assert((firstByte % numBytes) == 0);

    // check that there is enough space
    assert((firstByte + numBytes) <= v.size());

    // get
    const T* r = reinterpret_cast<const T*>(&v[firstByte]);

    if constexpr (sizeof(T) == 1 || !reverse_bytes)
    {
        x = *r;
    }
    else
    {
        x = get_reverse_bytes(*r);
    }
}

template<class T, bool swap_bytes>
void get_bytes(const std::vector<uint8_t>& v, size_t firstByte, size_t numBytes, T* x)
{
    // check that T is valid for n
    assert((numBytes % sizeof(T)) == 0);

    // check valid byte alignment
    assert((firstByte % numBytes) == 0);

    // check that there is enough space
    assert((firstByte + numBytes) <= v.size());

    // get
    if constexpr (sizeof(T) == 1)
    {
        memcpy(x, &v[firstByte], numBytes);
    }
    else
    {
        const T* r = reinterpret_cast<const T*>(&v[firstByte]);
        const T* end = reinterpret_cast<const T*>(&v[firstByte + numBytes]);

        if constexpr (swap_bytes)
        {
            for (; r != end; ++r)
            {
                *x = reverse_bytes(*r);
                ++x;
            }
        }
        else
        {
            for (; r != end; ++r)
            {
                *x = *r;
                ++x;
            }
        }
    }
}

size_t size_of_type(PsType type);

void pad_bytes(std::vector<uint8_t>& v, size_t firstByte, size_t numBytes);

size_t next_multiple_of(size_t val, size_t multiple);

#endif
