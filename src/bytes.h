// k13
// Kyle J Burgess

#ifndef K13_BYTES_H
#define K13_BYTES_H

#include <cstring>
#include <cstdint>
#include <type_traits>

// Check for gcc bswap support
#undef K13_GCC_BSWAP_SUPPORT
#if (__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 3))
#define K13_GCC_BSWAP_SUPPORT
#endif

// Check for msc bswap support
#undef K13_MSC_BSWAP_SUPPORT
#ifdef _MSC_VER
#define K13_MSC_BSWAP_SUPPORT
#endif

// Inline Expression
#undef K13_INLINE_ATTRIBUTE
#ifdef _MSC_VER
#define K13_INLINE_ATTRIBUTE __forceinline
#else
#define K13_INLINE_ATTRIBUTE __attribute__((always_inline)) __attribute__((flatten)) inline
#endif

namespace k13
{
    template<class T>
    K13_INLINE_ATTRIBUTE
    T byteswap(T x)
    {
        if constexpr (!std::is_unsigned<T>::value)
        {
            if constexpr (sizeof(T) == 2u)
            {
                auto r = byteswap(reinterpret_cast<uint16_t&>(x));
                return reinterpret_cast<T&>(r);
            }

            if constexpr (sizeof(T) == 4u)
            {
                auto r = byteswap(reinterpret_cast<uint32_t&>(x));
                return reinterpret_cast<T&>(r);
            }

            if constexpr (sizeof(T) == 8u)
            {
                auto r = byteswap(reinterpret_cast<uint64_t&>(x));
                return reinterpret_cast<T&>(r);
            }
        }

        if constexpr (sizeof(T) == 8)
        {
        #if   defined K13_MSC_BSWAP_SUPPORT
            return _byteswap_uint64(x);
        #elif defined K13_GCC_BSWAP_SUPPORT
            return __builtin_bswap64(x);
        #else
            return
                ((x << 56) & 0xff00000000000000ull) |
                ((x << 40) & 0x00ff000000000000ull) |
                ((x << 24) & 0x0000ff0000000000ull) |
                ((x <<  8) & 0x000000ff00000000ull) |
                ((x >>  8) & 0x00000000ff000000ull) |
                ((x >> 24) & 0x0000000000ff0000ull) |
                ((x >> 40) & 0x000000000000ff00ull) |
                ( x >> 56);
        #endif
        }

        if constexpr (sizeof(T) == 4)
        {
        #if   defined K13_MSC_BSWAP_SUPPORT
            return _byteswap_ulong(x);
        #elif defined K13_GCC_BSWAP_SUPPORT
            return __builtin_bswap32(x);
        #else
            return
                ((x << 24) & 0xff000000u) |
                ((x <<  8) & 0x00ff0000u) |
                ((x >>  8) & 0x0000ff00u) |
                ( x >> 24);
        #endif
        }

        if constexpr (sizeof(T) == 2)
        {
        #if   defined K13_MSC_BSWAP_SUPPORT
            return _byteswap_ushort(x);
        #elif defined K13_GCC_BSWAP_SUPPORT
            return __builtin_bswap16(x);
        #else
            return
                ((x << 8) & 0xff00u) |
                ( x >> 8);
        #endif
        }

        return x;
    }

    template<class T>
    void byteswap(T* x, size_t n)
    {
        if constexpr (sizeof(T) != 1)
        {
            T* end = x + n;

            for (; x != end; ++x)
            {
                *x = byteswap(*x);
            }
        }
    }

    template<class T>
    void byteswap(T* dst, const T* src, size_t n)
    {
        if constexpr (sizeof(T) == 1)
        {
            memcpy(dst, src, n);
        }
        else
        {
            T* end = dst + n;

            for (; dst != end; ++dst)
            {
                *dst = byteswap(*src);
                ++src;
            }
        }
    }
}

#endif
