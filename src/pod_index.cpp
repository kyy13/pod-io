// pod-index
// Kyle J Burgess

#include "pod_index.h"
#include "PxBytes.h"
#include "PxTypes.h"
#include "PxLookup.h"

#include <stdexcept>

PxContainer* pxCreateContainer()
{
    return new PxContainer;
}

void pxDeleteContainer(PxContainer* container)
{
    delete container;
}

PxItem* pxGetItem(PxContainer* container, const char* key)
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
        const PxData data =
            {
                .count = 0,
                .type = PX_UINT8,
            };

        it = map.insert(std::make_pair(std::move(str), data)).first;
    }

    return reinterpret_cast<PxItem*>(&(*it));
}

PxResult pxRemoveItem(PxContainer* container, PxItem* item)
{
    if ((container == nullptr) || (item == nullptr))
    {
        return PX_NULL_REFERENCE;
    }

    auto& key = reinterpret_cast<const std::pair<std::string,PxData>*>(item)->first;

    container->map.erase(key);

    return PX_SUCCESS;
}

PxItem* pxTryGetItem(PxContainer* container, const char* key)
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

    return reinterpret_cast<PxItem*>(&(*it));
}

PxResult pxSetValues(PxItem* item, const void* srcValueArray, uint32_t valueCount, PxType valueType)
{
    if (item == nullptr)
    {
        return PX_NULL_REFERENCE;
    }

    uint32_t maxCount = MaxCountLookup[size_of_type(valueType)];

    if (valueCount > maxCount)
    {
        return PX_OUT_OF_RANGE;
    }

    size_t size = static_cast<size_t>(valueCount) * size_of_type(valueType);

    auto& data = reinterpret_cast<std::pair<std::string,PxData>*>(item)->second;

    data.values.resize(size);
    data.count = valueCount;
    data.type = valueType;

    memcpy(data.values.data(), srcValueArray, size);

    return PX_SUCCESS;
}

PxResult pxTryCountValues(const PxItem* item, uint32_t& valueCount)
{
    if (item == nullptr)
    {
        return PX_NULL_REFERENCE;
    }

    auto& data = reinterpret_cast<const std::pair<std::string,PxData>*>(item)->second;

    valueCount = data.count;

    return PX_SUCCESS;
}

PxResult pxTryGetType(const PxItem* item, PxType& valueType)
{
    if (item == nullptr)
    {
        return PX_NULL_REFERENCE;
    }

    auto& data = reinterpret_cast<const std::pair<std::string,PxData>*>(item)->second;

    valueType = data.type;

    return PX_SUCCESS;
}

PxResult pxTryCopyValues(const PxItem* item, void* dstValueArray, uint32_t valueCount, PxType type)
{
    if (item == nullptr)
    {
        return PX_NULL_REFERENCE;
    }

    auto& data = reinterpret_cast<const std::pair<std::string,PxData>*>(item)->second;

    if (data.count == 0)
    {
        return PX_SUCCESS;
    }

    if (type != data.type)
    {
        return PX_TYPE_MISMATCH;
    }

    size_t size = valueCount * size_of_type(type);
    if (size > data.values.size())
    {
        return PX_OUT_OF_RANGE;
    }

    memcpy(dstValueArray, data.values.data(), size);

    return PX_SUCCESS;
}

PxResult pxTryCountKeyChars(const PxItem* item, uint32_t& count)
{
    if (item == nullptr)
    {
        return PX_NULL_REFERENCE;
    }

    auto& key = reinterpret_cast<const std::pair<std::string,PxData>*>(item)->first;

    count = static_cast<uint32_t>(key.size());

    return PX_SUCCESS;
}

PxResult pxTryCopyKey(const PxItem* item, char* buffer, uint32_t charCount)
{
    if (item == nullptr)
    {
        return PX_NULL_REFERENCE;
    }

    auto& key = reinterpret_cast<const std::pair<std::string,PxData>*>(item)->first;

    if (charCount != key.size())
    {
        return PX_OUT_OF_RANGE;
    }

    memcpy(buffer, key.data(), key.size());

    return PX_SUCCESS;
}

PxItem* pxGetFirstItem(PxContainer* container)
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

    return reinterpret_cast<PxItem*>(&(*it));
}

PxItem* pxGetNextItem(PxContainer* container, PxItem* item)
{
    if ((container == nullptr) || (item == nullptr))
    {
        return nullptr;
    }

    auto& key = reinterpret_cast<const std::pair<std::string,PxData>*>(item)->first;

    auto& map = container->map;

    auto it = map.find(key);

    if (it == map.end())
    {
        return nullptr;
    }

    return reinterpret_cast<PxItem*>(&(*it));
}
