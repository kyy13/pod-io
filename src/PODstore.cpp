// PODstore
// Kyle J Burgess

#include "PODstore.h"
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

PsContainer* psCreateContainer()
{
    return new PsContainer;
}

void psDeleteContainer(PsContainer* container)
{
    delete container;
}

PsItem* psGetItem(PsContainer* container, const char* key)
{
    if (container == nullptr)
    {
        return nullptr;
    }

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

    auto& map = container->map;

    auto it = map.find(key);

    if (it == map.end())
    {
        const PsData data =
            {
                .count = 0,
                .type = PS_UINT8,
            };

        it = map.insert(std::make_pair(std::move(str), data)).first;
    }

    return reinterpret_cast<PsItem*>(&(*it));
}

PsResult psRemoveItem(PsContainer* container, PsItem* item)
{
    if ((container == nullptr) || (item == nullptr))
    {
        return PS_NULL_REFERENCE;
    }

    auto& key = reinterpret_cast<const std::pair<std::string,PsData>*>(item)->first;

    container->map.erase(key);

    return PS_SUCCESS;
}

PsItem* psTryGetItem(PsContainer* container, const char* key)
{
    if (container == nullptr)
    {
        return nullptr;
    }

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

    auto& map = container->map;

    auto it = map.find(key);

    if (it == map.end())
    {
        return nullptr;
    }

    return reinterpret_cast<PsItem*>(&(*it));
}

PsResult psSetValues(PsItem* b, const void* values, uint32_t count, PsType type)
{
    if (b == nullptr)
    {
        return PS_NULL_REFERENCE;
    }

    uint32_t maxCount = MaxCountLookup[size_of_type(type)];

    if (count > maxCount)
    {
        return PS_OUT_OF_RANGE;
    }

    size_t size = static_cast<size_t>(count) * size_of_type(type);

    auto& data = reinterpret_cast<std::pair<std::string,PsData>*>(b)->second;

    data.values.resize(size);
    data.count = count;
    data.type = type;

    memcpy(data.values.data(), values, size);

    return PS_SUCCESS;
}

PsResult psTryCountValues(const PsItem* b, uint32_t& valueCount)
{
    if (b == nullptr)
    {
        return PS_NULL_REFERENCE;
    }

    auto& data = reinterpret_cast<const std::pair<std::string,PsData>*>(b)->second;

    valueCount = data.count;

    return PS_SUCCESS;
}

PsResult psTryGetType(const PsItem* b, PsType& valueType)
{
    if (b == nullptr)
    {
        return PS_NULL_REFERENCE;
    }

    auto& data = reinterpret_cast<const std::pair<std::string,PsData>*>(b)->second;

    valueType = data.type;

    return PS_SUCCESS;
}

PsResult psTryCopyValues(const PsItem* b, void* dst, uint32_t count, PsType type)
{
    if (b == nullptr)
    {
        return PS_NULL_REFERENCE;
    }

    auto& data = reinterpret_cast<const std::pair<std::string,PsData>*>(b)->second;

    if (data.count == 0)
    {
        return PS_SUCCESS;
    }

    if (type != data.type)
    {
        return PS_TYPE_MISMATCH;
    }

    size_t size = count * size_of_type(type);
    if (size > data.values.size())
    {
        return PS_OUT_OF_RANGE;
    }

    memcpy(dst, data.values.data(), size);

    return PS_SUCCESS;
}

PsResult psTryCountKeyChars(const PsItem* item, uint32_t& count)
{
    if (item == nullptr)
    {
        return PS_NULL_REFERENCE;
    }

    auto& key = reinterpret_cast<const std::pair<std::string,PsData>*>(item)->first;

    count = static_cast<uint32_t>(key.size());

    return PS_SUCCESS;
}

PsResult psTryCopyKey(const PsItem* item, char* buffer, uint32_t count)
{
    if (item == nullptr)
    {
        return PS_NULL_REFERENCE;
    }

    auto& key = reinterpret_cast<const std::pair<std::string,PsData>*>(item)->first;

    if (count != key.size())
    {
        return PS_OUT_OF_RANGE;
    }

    memcpy(buffer, key.data(), key.size());

    return PS_SUCCESS;
}

PsItem* psGetFirstItem(PsContainer* container)
{
    if (container == nullptr)
    {
        return nullptr;
    }

    auto& map = container->map;

    auto it = map.begin();

    if (it == map.end())
    {
        return nullptr;
    }

    return reinterpret_cast<PsItem*>(&(*it));
}

PsItem* psGetNextItem(PsContainer* container, PsItem* item)
{
    if ((container == nullptr) || (item == nullptr))
    {
        return nullptr;
    }

    auto& key = reinterpret_cast<const std::pair<std::string,PsData>*>(item)->first;

    auto& map = container->map;

    auto it = map.find(key);

    if (it == map.end())
    {
        return nullptr;
    }

    return reinterpret_cast<PsItem*>(&(*it));
}
