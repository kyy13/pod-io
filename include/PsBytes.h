// PODserializer
// Kyle J Burgess

#ifndef PS_BYTES_H
#define PS_BYTES_H

#include <cstdint>
#include <cassert>
#include <cstring>

class PsBytes
{
public:

    PsBytes();

    PsBytes(size_t size);

    PsBytes(const PsBytes& o);

    PsBytes(PsBytes&& o) noexcept;

    PsBytes& operator=(const PsBytes& o);

    PsBytes& operator=(PsBytes&& o) noexcept;

    ~PsBytes();

    template<class T, bool swap_bytes>
    void set(size_t firstByte, size_t numBytes, T x)
    {
        // check that T is valid for n
        assert(sizeof(T) == numBytes);

        // check valid byte alignment
        assert((firstByte % numBytes) == 0);

        // check that there is enough space
        assert((firstByte + numBytes) <= m_size);

        // set
        T* r = reinterpret_cast<T*>(&m_data[firstByte]);

        if constexpr (sizeof(T) == 1 || !swap_bytes)
        {
            *r = x;
        }
        else
        {
            *r = swapBytes(x);
        }
    }

    template<class T, bool swap_bytes>
    void set(size_t firstByte, size_t numBytes, const T* x)
    {
        // check that T is valid for n
        assert((numBytes % sizeof(T)) == 0);

        // check valid byte alignment
        assert((firstByte % numBytes) == 0);

        // check that there is enough space
        assert((firstByte + numBytes) <= m_size);

        // set
        if constexpr (sizeof(T) == 1)
        {
            memcpy(&m_data[firstByte], x, numBytes);
        }
        else
        {
            T* r = reinterpret_cast<T*>(&m_data[firstByte]);
            T* end = reinterpret_cast<T*>(&m_data[firstByte + numBytes]);

            if constexpr (swap_bytes)
            {
                for (; r != end; ++r)
                {
                    *r = swapBytes(x);
                    ++x;
                }
            }
            else
            {
                for (; r != end; ++r)
                {
                    *r = x;
                    ++x;
                }
            }
        }
    }

    template<class T, bool swap_bytes>
    void get(size_t firstByte, size_t numBytes, T& x)
    {
        // check that T is valid for n
        assert(sizeof(T) == numBytes);

        // check valid byte alignment
        assert((firstByte % numBytes) == 0);

        // check that there is enough space
        assert((firstByte + numBytes) <= m_size);

        // get
        const T* r = reinterpret_cast<const T*>(&m_data[firstByte]);

        if constexpr (sizeof(T) == 1 || !swap_bytes)
        {
            x = *r;
        }
        else
        {
            x = swapBytes(*r);
        }
    }

    template<class T, bool swap_bytes>
    void get(size_t firstByte, size_t numBytes, T* x)
    {
        // check that T is valid for n
        assert((numBytes % sizeof(T)) == 0);

        // check valid byte alignment
        assert((firstByte % numBytes) == 0);

        // check that there is enough space
        assert((firstByte + numBytes) <= m_size);

        // get
        if constexpr (sizeof(T) == 1)
        {
            memcpy(x, &m_data[firstByte], numBytes);
        }
        else
        {
            const T* r = reinterpret_cast<const T*>(&m_data[firstByte]);
            const T* end = reinterpret_cast<const T*>(&m_data[firstByte + numBytes]);

            if constexpr (swap_bytes)
            {
                for (; r != end; ++r)
                {
                    x = swapBytes(*r);
                    ++x;
                }
            }
            else
            {
                for (; r != end; ++r)
                {
                    x = *r;
                    ++x;
                }
            }
        }
    }

    void resize(size_t n);

    void pad(size_t firstByte, size_t numBytes);

    static size_t nextMultipleOf(size_t val, size_t multiple);

protected:
    uint8_t* m_data;
    size_t m_size;
    size_t m_capacity;

    template<class T>
    T swapBytes(T value)
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
};


#endif
