// PODserializer
// Kyle J Burgess

#include "PODserializer.h"
#include "PsBytes.h"
#include "PsTypes.h"

#include <limits>
#include <stdexcept>

const uint32_t MaxCountLookup[] =
    {
        0u,
        std::numeric_limits<uint32_t>::max() / 1u, // 1 byte
        std::numeric_limits<uint32_t>::max() / 2u, // 2 bytes
        0u,
        std::numeric_limits<uint32_t>::max() / 4u, // 4 bytes
        0u,
        0u,
        0u,
        std::numeric_limits<uint32_t>::max() / 8u, // 8 bytes
    };

PsSerializer* psCreateSerializer()
{
    return new PsSerializer;
}

void psDeleteSerializer(PsSerializer* serializer)
{
    delete serializer;
}

PsBlock* psGetBlock(PsSerializer* s, const char* key)
{
    std::string str;

    try
    {
        str.assign(key);
    }
    catch (const std::length_error&)
    {
        return nullptr;
    }

    // limit key size
    if (str.size() > std::numeric_limits<uint32_t>::max())
    {
        return nullptr;
    }

    auto& b = s->map[str];

    return &b;
}

PsResult psSetValues(PsBlock* b, const void* values, uint32_t count, PsType type)
{
    uint32_t maxCount = MaxCountLookup[size_of_type(type)];

    if (count > maxCount)
    {
        return PS_OUT_OF_RANGE;
    }

    size_t size = static_cast<size_t>(count) * size_of_type(type);

    b->data.resize(size);
    b->count = count;
    b->type = type;

    memcpy(b->data.data(), values, size);

    return PS_SUCCESS;
}

PsResult psTryCountValues(const PsBlock* b, uint32_t& valueCount)
{
    if (b->count == 0)
    {
        return PS_UNASSIGNED_BLOCK;
    }

    valueCount = b->count;

    return PS_SUCCESS;
}

PsResult psTryGetType(const PsBlock* b, PsType& valueType)
{
    if (b->count == 0)
    {
        return PS_UNASSIGNED_BLOCK;
    }

    valueType = b->type;

    return PS_SUCCESS;
}

PsResult psTryCopyValues(const PsBlock* b, void* dst, uint32_t count, PsType type)
{
    if (b->count == 0)
    {
        return PS_UNASSIGNED_BLOCK;
    }

    if (type != b->type)
    {
        return PS_TYPE_MISMATCH;
    }

    size_t size = count * size_of_type(type);
    if (size > b->data.size())
    {
        return PS_OUT_OF_RANGE;
    }

    memcpy(dst, b->data.data(), size);

    return PS_SUCCESS;
}
