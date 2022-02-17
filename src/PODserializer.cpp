// PODserializer
// Kyle J Burgess

#include "PODserializer.h"
#include "Bytes.h"
#include "PsTypes.h"

PsSerializer* psCreateSerializer()
{
    return new PsSerializer;
}

void psDeleteSerializer(PsSerializer* serializer)
{
    delete serializer;
}

PsBlock* psGetBlockByKey(PsSerializer* s, const char* key)
{
    auto& b = s->map[key];
    b.count = 0;
    return &b;
}

void psSetValues(PsBlock* b, const void* values, PsType type, uint32_t count)
{
    uint32_t size = count * psGetTypeSize(type);

    b->values.resize(pad64(size), 0);
    memcpy(b->values.data(), values, size);

    b->count = count;
    b->type = type;
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

    memcpy(dst, b->values.data(), b->count * psGetTypeSize(type));
    return PS_SUCCESS;
}
