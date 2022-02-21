// PODserializer
// Kyle J Burgess

#include "PsBytes.h"

PsBytes::PsBytes() : m_data(nullptr), m_size(0), m_capacity(0)
{}

PsBytes::PsBytes(size_t size) : m_data(nullptr), m_size(size), m_capacity(size)
{
    if (size > 0u)
    {
        m_data = new uint8_t[size];
    }
}

PsBytes::PsBytes(const PsBytes& o) : m_data(nullptr), m_size(o.m_size), m_capacity(o.m_size)
{
    if (m_size > 0)
    {
        m_data = new uint8_t[m_capacity];
        memcpy(m_data, o.m_data, m_size);
    }
}

PsBytes::PsBytes(PsBytes&& o) noexcept : m_data(o.m_data), m_size(o.m_size), m_capacity(o.m_capacity)
{
    o.m_data = nullptr;
    o.m_size = 0;
    o.m_capacity = 0;
}

PsBytes& PsBytes::operator=(const PsBytes& o)
{
    if (this == &o)
    {
        return *this;
    }

    m_data = nullptr;
    m_size = o.m_size;
    m_capacity = o.m_size;

    if (m_size > 0)
    {
        m_data = new uint8_t[m_capacity];
        memcpy(m_data, o.m_data, m_size);
    }

    return *this;
}

PsBytes& PsBytes::operator=(PsBytes&& o) noexcept
{
    if (this != &o)
    {
        delete[] m_data;

        m_data = o.m_data;
        m_size = o.m_size;
        m_capacity = o.m_capacity;

        o.m_data = nullptr;
        o.m_size = 0;
        o.m_capacity = 0;
    }

    return *this;
}

PsBytes::~PsBytes()
{
    delete[] m_data;
}

void PsBytes::resize(size_t n)
{
    size_t r = m_capacity;

    if (n > r)
    {
        while (r < n)
        {
            r = (r < 4)
                ? (r + 1)
                : (r + (r >> 1u));
        }

        auto* data = new uint8_t[r];
        if (m_size > 0)
        {
            memcpy(data, m_data, m_size);
            delete[] m_data;
        }
        m_data = data;
        m_capacity = r;
    }

    m_size = n;
}

void PsBytes::pad(size_t firstByte, size_t numBytes)
{
    // check that there is enough space
    assert((firstByte + numBytes) <= m_size);

    if (numBytes != 0)
    {
        memset(&m_data[firstByte], 0, numBytes);
    }
}

size_t PsBytes::nextMultipleOf(size_t val, size_t multiple)
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
