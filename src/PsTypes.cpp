// PODserializer
// Kyle J Burgess

#include "PsTypes.h"

PsBlock::PsBlock()
    : m_count(0)
    , m_type(PS_UINT8)
{}

size_t PsBlock::bytes() const
{
    return bytesPerValue() * m_count;
}

size_t PsBlock::bytesPerValue() const
{
    return m_type & 0xffffu;
}

const uint8_t* PsBlock::data() const
{
    return m_values.data();
}

void PsBlock::set(PsType type, size_t count, const void* values)
{
    m_type = type;
    m_count = count;
    m_values.resize(bytes());
    memcpy(m_values.data(), values, bytes());
}

void PsBlock::get(void* values) const
{
    memcpy(values, m_values.data(), bytes());
}

PsType PsBlock::type() const
{
    return m_type;
}

size_t PsBlock::count() const
{
    return m_count;
}
