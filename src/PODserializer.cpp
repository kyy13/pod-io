// PODserializer
// Kyle J Burgess

#include "PODserializer.h"
#include "PsBytes.h"
#include "PsTypes.h"

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
    auto& b = s->map[key];

    return &b;
}

void psSetValues(PsBlock* b, const void* values, PsType type, uint32_t count)
{
    size_t size = count * size_of_type(type);

    b->data.resize(size);
    b->count = count;
    b->type = type;

    memcpy(b->data.data(), values, size);
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

PsResult psTryCopyValues(const PsBlock* b, void* dst, PsType type)
{
    if (b->count == 0)
    {
        return PS_UNASSIGNED_BLOCK;
    }

    if (type != b->type)
    {
        return PS_TYPE_MISMATCH;
    }

    memcpy(dst, b->data.data(), b->data.size());

    return PS_SUCCESS;
}
