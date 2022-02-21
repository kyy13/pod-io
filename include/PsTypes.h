// PODserializer
// Kyle J Burgess

#ifndef PS_TYPES_H
#define PS_TYPES_H

#include "PODserializer.h"

#include <unordered_map>
#include <vector>
#include <string>

class PsBlock
{
public:

    PsBlock();

    void set(PsType type, size_t count, const void* values);

    void get(void* values) const;

    [[nodiscard]]
    PsType type() const;

    [[nodiscard]]
    size_t count() const;

    [[nodiscard]]
    size_t bytes() const;

    [[nodiscard]]
    size_t bytesPerValue() const;

    [[nodiscard]]
    const uint8_t* data() const;

protected:

    std::vector<uint8_t> m_values;

    size_t m_count;

    PsType m_type;

};

using PsMap = std::unordered_map<std::string, PsBlock>;

struct PsSerializer
{
    PsMap map;
};

uint32_t psGetTypeSize(PsType type);

#endif
